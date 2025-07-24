#pragma once
#include "CScanSettingsDlg.h"
#include "Trades.h"
#include "Models.h"

#define WM_START_SCAN		(WM_APP+0x0001)

// CWorketThread

class CWorkerThread : public CWinThread
{
	VOID OnStartScan(WPARAM, LPARAM);
	DECLARE_DYNCREATE(CWorkerThread)

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
	Scan::BestModels GetBest()const;

protected:
	DECLARE_MESSAGE_MAP()

private:
	std::vector<TradeFile const*>::const_iterator m_itFrom, m_itEnd;
	CScanSettingsDlg const* m_pSets;
	Scan::CompareObj m_objComp;

	BOOL m_bWorking;
	int m_Model_Count;
	CEvent m_evFinished;
	mutable CCriticalSection m_CS;
};


