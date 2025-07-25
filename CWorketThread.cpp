// CWorketThread.cpp : implementation file
//

#include "pch.h"
#include "Trade View.h"
#include "CWorketThread.h"
#include "Models.h"

// CWorketThread

IMPLEMENT_DYNCREATE(CWorkerThread, CWinThread)

VOID CWorkerThread::OnStartScan(WPARAM, LPARAM)
{
	auto scan_count{ std::distance(m_itFrom,m_itEnd) };
	if (scan_count)
		--scan_count;

	if (m_NetResults)
	{
		m_NetResults->clear();
		m_NetResults->reserve(scan_count);
	}
	if (m_FactorResults)
	{
		m_FactorResults->clear();
		m_FactorResults->reserve(scan_count);
	}
	if (m_CustomResults)
	{
		m_CustomResults->clear();
		m_CustomResults->reserve(scan_count);
	}

	Scan::ModelBunch origin;
	origin.Add(*m_itFrom);

	for (auto iter{ std::next(m_itFrom) }; iter != m_itEnd; ++iter)
	{
		auto bunch{ origin };
		bunch.Add(*iter);
		if (bunch.Calculate())
		{
			if (m_NetResults)
				m_NetResults->push_back({ bunch.GetNet(), { bunch.GetSubModels()[0], bunch.GetSubModels()[1] } });

			if (m_FactorResults)
				m_FactorResults->push_back({ bunch.GetFactor(), { bunch.GetSubModels()[0], bunch.GetSubModels()[1] } });

			if (m_CustomResults)
				m_CustomResults->push_back({ bunch.GetCustom(), { bunch.GetSubModels()[0], bunch.GetSubModels()[1] } });
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

	m_NetResults.reset(), m_FactorResults.reset(), m_CustomResults.reset();

	if (dlg.m_Scan4_Net)
		m_NetResults = std::make_unique<ScanResults>();
	if (dlg.m_Scan4_Factor)
		m_FactorResults = std::make_unique<ScanResults>();
	if (dlg.m_Scan4_Custom)
		m_CustomResults = std::make_unique<ScanResults>();
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

//Scan::BestModels CWorkerThread::GetBest()const
//{
//	Scan::BestModels ret;
//	if (m_BestNet)
//		ret.push_back(*m_BestNet);
//	if (m_BestFactor)
//		ret.push_back(*m_BestFactor);
//	if (m_BestCustom)
//		ret.push_back(*m_BestCustom);
//	return ret;
//}

BEGIN_MESSAGE_MAP(CWorkerThread, CWinThread)
	ON_THREAD_MESSAGE(WM_START_SCAN, OnStartScan)
END_MESSAGE_MAP()


// CWorketThread message handlers
