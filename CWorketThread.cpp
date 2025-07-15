// CWorketThread.cpp : implementation file
//

#include "pch.h"
#include "Trade View.h"
#include "CWorketThread.h"


// CWorketThread

IMPLEMENT_DYNCREATE(CWorketThread, CWinThread)

CWorketThread::CWorketThread()
{
}

CWorketThread::~CWorketThread()
{
}

BOOL CWorketThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CWorketThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CWorketThread, CWinThread)
END_MESSAGE_MAP()


// CWorketThread message handlers
