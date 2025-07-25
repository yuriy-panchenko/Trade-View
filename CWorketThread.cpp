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

	if (m_BestNet)
		*m_BestNet = {};
	if (m_BestFactor)
		*m_BestFactor = {};
	if (m_BestCustom)
		*m_BestCustom = {};

	for (auto iter{ std::next(m_itFrom) }; iter != m_itEnd; ++iter)
	{
		auto bunch{ origin };
		bunch.Add(*iter);
		if (bunch.Calculate())
		{
			if (m_BestNet && m_BestNet->GetNet() < bunch.GetNet())
				*m_BestNet = bunch;

			if (m_BestFactor && m_BestFactor->GetFactor() < bunch.GetFactor())
				*m_BestFactor = bunch;

			if (m_BestCustom && m_BestCustom->GetCustom() < bunch.GetCustom())
				*m_BestCustom = bunch;
		}
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
	return TRUE;
}

int CWorkerThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

void CWorkerThread::Init(CScanSettingsDlg const& dlg, std::vector<TradeFile const*>::const_iterator itEnd)
{
	m_itEnd = itEnd;
	m_pSets = &dlg;

	m_BestNet.reset(), m_BestFactor.reset(), m_BestCustom.reset();

	if (dlg.m_Scan4_Net)
		m_BestNet = std::make_unique<Scan::ModelBunch>();
	if (dlg.m_Scan4_Factor)
		m_BestFactor = std::make_unique<Scan::ModelBunch>();
	if (dlg.m_Scan4_Custom)
		m_BestCustom = std::make_unique<Scan::ModelBunch>();
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
	Scan::BestModels ret;
	if (m_BestNet)
		ret.push_back(*m_BestNet);
	if (m_BestFactor)
		ret.push_back(*m_BestFactor);
	if (m_BestCustom)
		ret.push_back(*m_BestCustom);
	return ret;
}

BEGIN_MESSAGE_MAP(CWorkerThread, CWinThread)
	ON_THREAD_MESSAGE(WM_START_SCAN, OnStartScan)
END_MESSAGE_MAP()


// CWorketThread message handlers
