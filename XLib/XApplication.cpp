#include "stdafx.h"
#include "XLib/XApplication.h"

static XApplication* s_pApplication = NULL;

static void OnQuitSignal(int nSignal)
{
    assert(s_pApplication);
    s_pApplication->Exit();
}

static bool SetLimit()
{
    bool    bResult     = false;
    int     nRetCode    = 0;
    rlimit  ResLimit;

    nRetCode = getrlimit(RLIMIT_NOFILE, &ResLimit);
    XYLOG_FAILED_JUMP(nRetCode == 0);

    ResLimit.rlim_cur = ResLimit.rlim_max;

    nRetCode = setrlimit(RLIMIT_NOFILE, &ResLimit);
    XYLOG_FAILED_JUMP(nRetCode == 0);

    nRetCode = getrlimit(RLIMIT_CORE, &ResLimit);
    XYLOG_FAILED_JUMP(nRetCode == 0);

    ResLimit.rlim_cur = ResLimit.rlim_max;

    nRetCode = setrlimit(RLIMIT_CORE, &ResLimit);
    XYLOG_FAILED_JUMP(nRetCode == 0);

    bResult = true;

Exit0:
    return bResult;
}

static bool CloseStdio()
{
    bool    bResult     = false;
    int     nNullFile   = -1;

    nNullFile = open("/dev/null", O_WRONLY);
    XYLOG_FAILED_JUMP(nNullFile != -1);

    dup2(nNullFile, STDIN_FILENO);
    dup2(nNullFile, STDOUT_FILENO);
    dup2(nNullFile, STDERR_FILENO);

    bResult = true;
Exit0:
    if (nNullFile != -1)
    {
        close(nNullFile);
        nNullFile = -1;
    }
    return bResult;
}

static bool SetExitSignal()
{
    signal(SIGINT, OnQuitSignal);
    signal(SIGQUIT, OnQuitSignal);
    signal(SIGTERM, OnQuitSignal);
    return true;
}

XApplication::XApplication()
{
    assert(s_pApplication == NULL);
    s_pApplication = this;
}

XApplication::~XApplication()
{
    s_pApplication = NULL;
}

bool XApplication::Init(const char* pszAppName, bool bRunInBackground)
{
    m_n64Frame      = 0;
    m_bExitFlag     = false;
    m_fActiveCost   = 0;
    m_fFreeCost     = 0;
    m_pszAppName    = new char[strlen(pszAppName) + 1];
    strcpy(m_pszAppName, pszAppName);

    LogOpen(pszAppName, 65536);

    SetLimit();
    SetExitSignal();

    if (bRunInBackground)
    {
        CloseStdio();
    }

    Log("%s server startup ... ... [OK]", pszAppName);
    Log("Press [Ctrl + C] to exit !");
}

bool XApplication::UnInit()
{
    Log("%s server exit ... ... [OK]", m_pszAppName);
    XY_DELETE_ARRAY(m_pszAppName);
    LogClose();
}

void XApplication::Run()
{
    int64_t     n64TimeNow          = 0;
    int64_t     n64BeginTime        = XGetMSTime();
    int64_t     n64FreeTime         = 0;
    int64_t     n64FreeCost         = 0;
    int64_t     n64ActiveCost       = 0;
    int64_t     n64FreeTotalCost    = 0;
    int64_t     n64ActiveTotalCost  = 0;
    int         nIdleCount          = 0;

    while (!m_bExitFlag)
    {
        n64TimeNow = XGetMSTime();
        n64FreeTime = m_n64Frame * 1000 - (n64TimeNow - n64BeginTime) * GAME_FPS;
        if (n64FreeTime > 0)
        {
            n64FreeCost = XGetMSTime();
            OnFree();
            n64FreeCost = XGetMSTime() - n64FreeCost;
        }

        while (m_n64Frame * 1000 > (XGetMSTime() - n64BeginTime) * GAME_FPS)
        {
            usleep(10 * 1000);
        }

        n64ActiveCost = XGetMSTime();
        Activate();
        n64ActiveCost = XGetMSTime() - n64ActiveCost;

        m_n64Frame++;
        m_fFreeCost = m_fFreeCost * 0.9f + n64FreeCost * 0.1f;
        m_fActiveCost = m_fActiveCost * 0.9f + n64ActiveCost * 0.1f;
    }
}

void XApplication::Exit()
{
    m_bExitFlag = true;
}

void XApplication::Activate()
{

}

void XApplication::OnFree()
{

}

int XApplication::GetFrame()
{
    return (int)m_n64Frame;
}

float XApplication::GetFreeCost()
{
    return m_fFreeCost * GAME_FPS / 1000;
}

float XApplication::GetActiveCost()
{
    return m_fActiveCost * GAME_FPS / 1000;
}
