#pragma once



// CWorketThread

class CWorketThread : public CWinThread
{
	DECLARE_DYNCREATE(CWorketThread)

protected:
	CWorketThread();           // protected constructor used by dynamic creation
	virtual ~CWorketThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};


