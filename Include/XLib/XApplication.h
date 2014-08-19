#pragma once

class XApplication
{
public:
    XApplication();
    virtual ~XApplication();

public:
    virtual bool    Init(const char* pszAppName, bool bRunInBackground);
    virtual bool    UnInit();
    virtual void    Run();
    virtual void    Exit();
    virtual void    Activate();
    virtual void    OnFree();
    virtual float   GetFreeCost();
    virtual float   GetActiveCost();

public:
    virtual int     GetFrame();

protected:
    int64_t     m_n64Frame;
    bool        m_bExitFlag;
    float       m_fActiveCost;
    int         m_fFreeCost;
    char*       m_pszAppName;
};
