#include "stdafx.h"
#include "XLib/XLib.h"

static XMutex s_StartTickCountLock;

DWORD XGetMSTime()
{
    DWORD           dwResult = 0;
    int             nRetCode = 0;
    static uint64_t s_uBaseTime = 0;
    uint64_t        uTime = 0;
    timespec        timeVal;

    nRetCode = clock_gettime(CLOCK_MONOTONIC, &timeVal);
    if (nRetCode == 0)
    {
        uTime = ((uint64_t)timeVal.tv_sec) * 1000 + timeVal.tv_nsec / 1000 / 1000;
        if (s_uBaseTime == 0)
        {
            s_StartTickCountLock.Lock();
            if (s_uBaseTime == 0)
            {
                s_uBaseTime = uTime;
            }
            s_StartTickCountLock.Unlock();
        }
        dwResult = (DWORD)(uTime - s_uBaseTime) + 1;
    }

    return dwResult;
}
