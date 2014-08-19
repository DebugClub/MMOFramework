#include "stdafx.h"
#include "XLib/XMutex.h"

XMutex::XMutex()
{
	pthread_mutex_init(&m_Mutex, NULL);
}

XMutex::~XMutex()
{
	pthread_mutex_destroy(&m_Mutex);
}

void XMutex::Lock()
{
	pthread_mutex_lock(&m_Mutex);
}

void XMutex::Unlock()
{
	pthread_mutex_unlock(&m_Mutex);
}
