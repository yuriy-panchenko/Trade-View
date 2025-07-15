#pragma once

namespace Scan
{
	class CompareHelper
	{
		std::vector<TradeFile*>::const_iterator itFrom, itTo;

	public:
		CompareHelper(CScanSettingsDlg const& dlg, std::vector<TradeFile*>::const_iterator i, std::vector<TradeFile*>::const_iterator to)
			: comp{ dlg.m_Scan_Count, dlg.m_Scan4_Net, dlg.m_Scan4_Factor, dlg.m_Scan4_Custom }
			, itFrom{ i }
			, itTo{ to }
		{
		}
		CWinThread* Start()
		{
			return pTh = ::AfxBeginThread(&Proc, this);
		}

		CWinThread* pTh;
		Scan::CompareObj comp;

	private:
		static UINT Proc(LPVOID pData) { return static_cast<CompareHelper*>(pData)->DoWork(); }
		UINT DoWork()
		{
			for (auto iter{ std::next(itFrom) }; iter != itTo; ++iter)
			{
				Scan::ModelBunch bunch;
				bunch.Add(*iter);
				bunch.Add(*itFrom);
				bunch.Calculate();

				comp.Test(std::move(bunch));
			}

			return 0;
		}
	};

	class Models
	{
	public:
		BOOL IsScanning()const;

	private:
		//BOOL m_bStopScanningThread;
		//size_t m_scanFinished, m_scanTotal;
	};
}
