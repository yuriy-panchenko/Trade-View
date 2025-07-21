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
		BestModels GetBest()const;
		INFO GetInfo()const;

	private:
		static UINT DoWork(LPVOID);
		void OnWaitThreadFinished();
		void UpdateBest(BestModels&&);
		void SetInfoTotalScanCount(size_t total);

	private:
		CScanSettingsDlg const* m_pSets;
		std::vector<TradeFile const*> m_Files;
		CWinThread* m_pWaitTh;
		mutable CCriticalSection m_CS;
		BOOL m_bStopping;
		Scan::CompareObj m_Comp;
		INFO m_Info;
	};
}
