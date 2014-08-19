#pragma once
// 
// #ifdef __cplusplus
// extern "C" {
// #endif
// 
// int Log();
// 
// #ifdef __cplusplus
// }
// #endif

#define PLATFORM_API

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#define MAX_PATH    260
#define GAME_FPS    8

typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef unsigned char BYTE;

#include <XLib/ToolMacros.h>
#include <XLib/XMutex.h>
#include <XLib/Log.h>
#include "XLib/XApplication.h"

DWORD XGetMSTime();
