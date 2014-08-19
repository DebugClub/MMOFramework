#pragma once

class XMutex
{
public:
	XMutex();
	~XMutex();
	void Lock();
	void Unlock();

private:
	pthread_mutex_t m_Mutex;
};
