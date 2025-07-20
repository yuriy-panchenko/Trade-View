// CWorketThread.cpp : implementation file
//

#include "pch.h"
#include "Trade View.h"
#include "CWorketThread.h"

// CWorketThread

IMPLEMENT_DYNCREATE(CWorkerThread, CWinThread)

VOID CWorkerThread::OnStartScan(WPARAM, LPARAM)
{
	Scan::ModelBunch origin;
	origin.Add(*m_itFrom);
	m_objComp.Reset();

	for (auto iter{ std::next(m_itFrom) }; iter != m_itEnd; ++iter)
	{
		auto bunch{ origin };
		bunch.Add(*iter);
		bunch.Calculate();
		m_objComp.Test(std::move(bunch));
	}

	{
		CSingleLock _o{ &m_CS, TRUE };
		m_bWorking = FALSE;
	}
	
	m_evFinished.PulseEvent();
}

CWorkerThread::CWorkerThread()
	:m_evFinished{ FALSE, FALSE }
	, m_bWorking{ FALSE }
{
}

CWorkerThread::~CWorkerThread()
{
}

BOOL CWorkerThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CWorkerThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

void CWorkerThread::Init(CScanSettingsDlg const& dlg, std::vector<TradeFile const*>::const_iterator itEnd)
{
	m_itEnd = itEnd;
	m_pSets = &dlg;
	m_objComp.Init(dlg.m_Model_Count, dlg.m_Scan4_Net, dlg.m_Scan4_Factor, dlg.m_Scan4_Custom);
}

BOOL CWorkerThread::IsWorking() const
{
		CSingleLock _o{ &m_CS, TRUE };
	return m_bWorking;
}

void CWorkerThread::Start(std::vector<TradeFile const*>::const_iterator const itFrom)
{
	m_itFrom = itFrom;
	m_bWorking = TRUE;
	PostThreadMessage(WM_START_SCAN, 0, 0);
}

Scan::BestModels CWorkerThread::GetBest()const
{
	return m_objComp.GetBest();
}

BEGIN_MESSAGE_MAP(CWorkerThread, CWinThread)
	ON_THREAD_MESSAGE(WM_START_SCAN, OnStartScan)
END_MESSAGE_MAP()


// CWorketThread message handlers
