#pragma once

enum LogType
{
	eLogError = 0,
	eLogWarning,
	eLogInfo,
	eLogDebug
};


// LogOpen应该只在程序的Main函数里面调一次,也就是说,不要在dll之类的地方去调用
// 注意Hook函数自己需要处理线程安全问题,如果在Hook函数里面加锁,则要小心死锁
// 简单的建议就是: 不要在Hook函数里面再写log:)
// 日志文件: ./logs/$(cszName)/$(time).log
PLATFORM_API bool LogOpen(const char cszName[], int nMaxLine);
PLATFORM_API void LogClose();

// Log无需写换行符,内部会自动添加!
// 若输出char*数据，控制符用%hs
// 若输出wchar_t*数据，控制符用%ls
PLATFORM_API void Log(LogType eLevel, const char cszFormat[], ...);
PLATFORM_API void Log(const char cszFormat[], ...);
PLATFORM_API void LogDebug(const char cszFormat[], ...);
PLATFORM_API void LogWarning(const char cszFormat[], ...);
PLATFORM_API void LogError(const char cszFormat[], ...);

typedef bool(XLogCallback)(void* pvUsrData, LogType eLevel, const char cszMsg[]);

PLATFORM_API void LogHook(void* pvUsrData, XLogCallback* pCallback);
