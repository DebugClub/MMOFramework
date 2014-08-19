#include "stdafx.h"
#include "XLib/Log.h"

#define LOG_DIR            "logs"
#define LOG_LINE_MAX        (1024 * 2)

struct LogParam
{
	FILE*           pFile;
	int             nLineCount;
	int             nMaxFileLine;
	void*           pvUsrData;
	XLogCallback*   pCallback;
	XMutex          Mutex;
	char            szDir[MAX_PATH];
};

static LogParam*    s_pLog = NULL;
static const char*  s_pszLevel[] =
{
	"Err",
	"War",
	"Inf",
	"Dbg"
};

bool LogOpen(const char cszName[], int nMaxLine)
{
	bool    bResult = false;
	int     nRetCode = false;
	char*   pszRet = NULL;

	assert(s_pLog == NULL);

	s_pLog = new LogParam;

	nRetCode = snprintf(s_pLog->szDir, sizeof(s_pLog->szDir), "%s/%s", LOG_DIR, cszName);
	XY_FAILED_JUMP(nRetCode > 0 && nRetCode < (int)sizeof(s_pLog->szDir));

	s_pLog->pFile = NULL;
	s_pLog->nLineCount = 0;
	s_pLog->nMaxFileLine = nMaxLine;
	s_pLog->pCallback = NULL;
	s_pLog->pvUsrData = NULL;

	bResult = true;
Exit0:
	if (!bResult)
	{
		XY_DELETE(s_pLog);
	}
	return bResult;
}

void LogClose()
{
	if (s_pLog == NULL)
		return;

	if (s_pLog->pFile)
	{
		fclose(s_pLog->pFile);
		s_pLog->pFile = NULL;
	}

	XY_DELETE(s_pLog);
}

static FILE* CreateLogFile(const char cszPath[])
{
	FILE*   pFile		= NULL;
	int     nRetCode	= 0;
	char*   pszBuffer	= NULL;
	char*   pszPos		= NULL;
	int     nPreChar	= 0;

	pszBuffer = strdup(cszPath);
	XY_FAILED_JUMP(pszBuffer);

	pszPos = pszBuffer;

	while (*pszPos)
	{
		if (*pszPos == '/' || *pszPos == '\\')
		{
			*pszPos = '\0';

			if (nPreChar != 0 && nPreChar != ':')
			{
				nRetCode = mkdir(pszBuffer, 0777);
				XY_FAILED_JUMP(nRetCode != -1 || (errno == EEXIST));
			}

			*pszPos = '/';
		}

		nPreChar = *pszPos++;
	}

	pFile = fopen(pszBuffer, "w");
Exit0:
	XY_FREE(pszBuffer);
	return pFile;
}

static bool ResetLogFile()
{
	bool    bResult		= false;
	int     nRetCode	= false;
	time_t  uTimeNow	= 0;
	tm*     pTimeNow	= NULL;
	FILE*   pFile		= NULL;
	char    szPath[MAX_PATH];

	uTimeNow = time(NULL);
	pTimeNow = localtime(&uTimeNow);

	assert(pTimeNow);
	assert(s_pLog);

	nRetCode = snprintf(
		szPath, sizeof(szPath),
		"%s/%2.2d_%2.2d_%2.2d_%2.2d_%2.2d.log",
		s_pLog->szDir,
		pTimeNow->tm_mon + 1,
		pTimeNow->tm_mday,
		pTimeNow->tm_hour,
		pTimeNow->tm_min,
		pTimeNow->tm_sec
		);
	XY_FAILED_JUMP(nRetCode > 0 && nRetCode < (int)sizeof(szPath));

	pFile = CreateLogFile(szPath);
	XY_FAILED_JUMP(pFile);

	if (s_pLog->pFile)
	{
		fclose(s_pLog->pFile);
		s_pLog->pFile = NULL;
	}

	s_pLog->pFile = pFile;
	pFile = NULL;

	s_pLog->nLineCount = 0;

	bResult = true;
Exit0:
	if (pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}
	return bResult;
}

static void InternalLog(LogType eType, const char* pszLogContent, size_t uLogSize)
{
    int             nRetCode        = 0;
    time_t          uTimeNow        = 0;
    tm*             pTimeNow        = NULL;
    pthread_t       dwThread        = pthread_self();
	int             nLogLen         = 0;
	XLogCallback*   pCallback	    = NULL;
    static char     szLog[LOG_LINE_MAX];

	XY_FAILED_JUMP(s_pLog);

	assert(eType >= eLogError);
	assert(eType <= eLogDebug);

	uTimeNow = time(NULL);
	pTimeNow = localtime(&uTimeNow);

	assert(pTimeNow);

	nLogLen = snprintf(
		szLog, sizeof(szLog),
		"%2.2d-%2.2d,%2.2d:%2.2d:%2.2d<%s,%u>: ",
		pTimeNow->tm_mon + 1,
		pTimeNow->tm_mday,
		pTimeNow->tm_hour,
		pTimeNow->tm_min,
		pTimeNow->tm_sec,
		s_pszLevel[eType],
		dwThread
		);

	assert(nLogLen > 0);
	assert(nLogLen < sizeof(szLog));
    assert(uLogSize < sizeof(szLog) - nLogLen);

    memcpy(szLog + nLogLen, pszLogContent, uLogSize);
    nLogLen += uLogSize;

	if (nLogLen + 1 < sizeof(szLog) && szLog[nLogLen - 1] != '\n')
	{
		szLog[nLogLen++] = '\n';
		szLog[nLogLen] = '\0';
	}

	pCallback = s_pLog->pCallback;
	if (pCallback != NULL)
	{
		nRetCode = (*pCallback)(s_pLog->pvUsrData, eType, szLog);
		XY_FAILED_JUMP(nRetCode);
	}

	if (s_pLog->pFile == NULL || s_pLog->nLineCount >= s_pLog->nMaxFileLine)
	{
		ResetLogFile();
	}

	if (s_pLog->pFile)
	{
		fwrite(szLog, nLogLen, 1, s_pLog->pFile);
		fflush(s_pLog->pFile);
		s_pLog->nLineCount++;
	}

	fwrite(szLog, nLogLen, 1, stdout);
	fflush(stdout);

Exit0:
    ;
}

void LogHook(void* pvUsrData, XLogCallback* pCallback)
{
	s_pLog->pvUsrData = pvUsrData;
	s_pLog->pCallback = pCallback;
}

void Log(LogType eType, const char cszFormat[], ...)
{
    int         nRetCode = 0;
    va_list     marker;
    static char szLog[LOG_LINE_MAX];

    s_pLog->Mutex.Lock();
    va_start(marker, cszFormat);
    nRetCode = vsnprintf(szLog, sizeof(szLog), cszFormat, marker);
    va_end(marker);
    InternalLog(eType, szLog, nRetCode);
    s_pLog->Mutex.Unlock();
}

void Log(const char cszFormat[], ...)
{
    int         nRetCode = 0;
    va_list     marker;
    static char szLog[LOG_LINE_MAX];

    s_pLog->Mutex.Lock();
    va_start(marker, cszFormat);
    nRetCode = vsnprintf(szLog, sizeof(szLog), cszFormat, marker);
    va_end(marker);
    InternalLog(eLogInfo, szLog, nRetCode);
    s_pLog->Mutex.Unlock();
}

void LogDebug(const char cszFormat[], ...)
{
    int         nRetCode = 0;
    va_list     marker;
    static char szLog[LOG_LINE_MAX];

    s_pLog->Mutex.Lock();
    va_start(marker, cszFormat);
    nRetCode = vsnprintf(szLog, sizeof(szLog), cszFormat, marker);
    va_end(marker);
    InternalLog(eLogDebug, szLog, nRetCode);
    s_pLog->Mutex.Unlock();
}

void LogWarning(const char cszFormat[], ...)
{
    int         nRetCode = 0;
    va_list     marker;
    static char szLog[LOG_LINE_MAX];

    s_pLog->Mutex.Lock();
    va_start(marker, cszFormat);
    nRetCode = vsnprintf(szLog, sizeof(szLog), cszFormat, marker);
    va_end(marker);
    InternalLog(eLogWarning, szLog, nRetCode);
    s_pLog->Mutex.Unlock();
}

void LogError(const char cszFormat[], ...)
{
    int         nRetCode = 0;
    va_list     marker;
    static char szLog[LOG_LINE_MAX];

    s_pLog->Mutex.Lock();
    va_start(marker, cszFormat);
    nRetCode = vsnprintf(szLog, sizeof(szLog), cszFormat, marker);
    va_end(marker);
    InternalLog(eLogError, szLog, nRetCode);
    s_pLog->Mutex.Unlock();
}
