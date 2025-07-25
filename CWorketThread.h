#pragma once
#include "CScanSettingsDlg.h"
#include "Trades.h"
#include "ScanDefines.h"

#define WM_START_SCAN		(WM_APP+0x0001)

// CWorketThread
class CWorkerThread : public CWinThread
{
	VOID OnStartScan(WPARAM, LPARAM);
	DECLARE_DYNCREATE(CWorkerThread)

public:
	using ScanResults = Scan::pair_vector;

protected:
	CWorkerThread();           // protected constructor used by dynamic creation
	virtual ~CWorkerThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void Init(CScanSettingsDlg const&, std::vector<TradeFile const*>::const_iterator itEnd);
	BOOL IsWorking()const;
	CEvent& GetWorkingSyncObj() { return m_evFinished; }
	void Start(std::vector<TradeFile const*>::const_iterator);
	template<Scan::BestModels::Type MType>
	ScanResults const& GetResults()const;

protected:
	DECLARE_MESSAGE_MAP()

private:
	std::vector<TradeFile const*>::const_iterator m_itFrom, m_itEnd;
	CScanSettingsDlg const* m_pSets;
	std::unique_ptr<ScanResults> m_NetResults, m_FactorResults, m_CustomResults;

	BOOL m_bWorking;
	CEvent m_evFinished;
	mutable CCriticalSection m_CS;
};

template<Scan::BestModels::Type MType>
CWorkerThread::ScanResults const& CWorkerThread::GetResults()const
{
	switch (MType)
	{
	case Scan::BestModels::Net:assert(m_NetResults); break;
	case Scan::BestModels::Factor:assert(m_FactorResults); break;
	case Scan::BestModels::Custom:assert(m_CustomResults); break;
	default:assert(false);
	}

	switch (MType)
	{
	case Scan::BestModels::Net:return *m_NetResults;
	case Scan::BestModels::Factor:return *m_FactorResults;
	default:
		assert(false);
	case Scan::BestModels::Custom:return *m_CustomResults;
	}
}


