#pragma once

enum LogType
{
	eLogError = 0,
	eLogWarning,
	eLogInfo,
	eLogDebug
};


// LogOpenӦ��ֻ�ڳ����Main���������һ��,Ҳ����˵,��Ҫ��dll֮��ĵط�ȥ����
// ע��Hook�����Լ���Ҫ�����̰߳�ȫ����,�����Hook�����������,��ҪС������
// �򵥵Ľ������: ��Ҫ��Hook����������дlog:)
// ��־�ļ�: ./logs/$(cszName)/$(time).log
PLATFORM_API bool LogOpen(const char cszName[], int nMaxLine);
PLATFORM_API void LogClose();

// Log����д���з�,�ڲ����Զ����!
// �����char*���ݣ����Ʒ���%hs
// �����wchar_t*���ݣ����Ʒ���%ls
PLATFORM_API void Log(LogType eLevel, const char cszFormat[], ...);
PLATFORM_API void Log(const char cszFormat[], ...);
PLATFORM_API void LogDebug(const char cszFormat[], ...);
PLATFORM_API void LogWarning(const char cszFormat[], ...);
PLATFORM_API void LogError(const char cszFormat[], ...);

typedef bool(XLogCallback)(void* pvUsrData, LogType eLevel, const char cszMsg[]);

PLATFORM_API void LogHook(void* pvUsrData, XLogCallback* pCallback);
