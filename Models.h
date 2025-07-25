#pragma once
#include "Trades.h"
#include "ScanDefines.h"
class CWorkerThread;
class CScanSettingsDlg;

namespace Scan
{
	class Models
	{
	public:
		struct INFO
		{
			BOOL isStopping;
			size_t finished, total;
		};

	public:
		Models();
		BOOL Start(std::vector<TradeFile> const&, CScanSettingsDlg const&);
		void Stop(BOOL bWait2Finish);

		BOOL IsScanning()const;
		BOOL IsStoppingScan()const;
		//BestModels GetBest()const;
		INFO GetInfo()const;
		BestModels CollectBest(size_t)const;

	private:
		static UINT DoWork(LPVOID);
		void OnWaitThreadFinished();
		void CollectResults(CWorkerThread*);
		void SetInfoTotalScanCount(size_t total);
		std::vector<model_pair> Unite(Scan::pair_vector const&, size_t)const;
		std::vector<model_pair> Unite(size_t)const;

	private:
		CScanSettingsDlg const* m_pSets;
		std::vector<TradeFile const*> m_Files;
		CWinThread* m_pWaitTh;
		mutable CCriticalSection m_CS;
		BOOL m_bStopping;
		//Scan::CompareObj m_Comp;
		Scan::pair_vector m_NetTable, m_FactorTable, m_CustomTable;
		INFO m_Info;
	};
}
