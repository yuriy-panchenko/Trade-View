#include "pch.h"
#include "Models.h"
namespace Scan
{
	UINT Models::ScanProc(LPVOID pData)
	{
		auto& view{ *static_cast<CChildView*>(pData) };

		std::vector<TradeFile*> files;
		files.reserve(view.m_Files.size());

		for (auto& file : view.m_Files)
		{
			auto& stat{ file.GetStats() };
			if (view.m_SetsDlg.m_UseMinProfit && (stat.profit - stat.loss) / 100. < view.m_SetsDlg.m_Min_Profit)
				continue;
			if (view.m_SetsDlg.m_UseMinTrades && (stat.iWon + stat.iLost) < view.m_SetsDlg.m_Min_Trades)
				continue;
			files.push_back(&file);
		}

		if (files.size() > 1)
		{
			CHive hive;
			using comp_helper = std::unique_ptr<CompareHelper>;
			std::vector<comp_helper> helpers;
			int scans{ 0 };
			if (view.m_SetsDlg.m_Scan4_Net)
				scans += view.m_SetsDlg.m_Scan_Count;
			if (view.m_SetsDlg.m_Scan4_Factor)
				scans += view.m_SetsDlg.m_Scan_Count;
			if (view.m_SetsDlg.m_Scan4_Custom)
				scans += view.m_SetsDlg.m_Scan_Count;
			scans = max(scans, view.m_SetsDlg.m_Scan_Count);

			Scan::CompareObj comp{ scans, view.m_SetsDlg.m_Scan4_Net, view.m_SetsDlg.m_Scan4_Factor, view.m_SetsDlg.m_Scan4_Custom };

			////////////////////////////////////////////////////////////////////
			auto find_helper = [&helpers](CWinThread* pTh)
				{
					auto iter{ helpers.begin() };
					for (; iter != helpers.end(); iter++)
						if (iter->get()->pTh == pTh)
							break;
					return iter;
				};

			////////////////////////////////////////////////////////////////////
			size_t const total_scans{ files.size() - 1 };
			size_t finished_scans{ 0 };

			auto finish_thread = [&](CWinThread* pTh)
				{
					auto iter{ find_helper(pTh) };
					if (iter != helpers.end())
					{
						bool doUpdate{ false };

						for (auto& res : iter->get()->comp.GetBest())
							doUpdate |= comp.Test(res);

						if (doUpdate)
						{
							auto best{ comp.GetBest() };
							view.SendMessage(WM_BETTER_RESULT, (WPARAM)&best);
							//view.FoundBetterResults(comp.GetBest());
						}

						helpers.erase(iter);
					}
					++finished_scans;
					view.PostMessage(WM_SCAN_FINISHED, (WPARAM)finished_scans, (LPARAM)total_scans);
				};


			for (auto iter{ files.begin() }; iter != std::prev(files.end()); ++iter)
			{
				if (view.IsStoppingScan())
					break;
				helpers.push_back(std::make_unique<CompareHelper>(view.m_SetsDlg, iter, files.end()));
				if (auto pTh = hive.Add(helpers.back()->Start()))
					finish_thread(pTh);
			}

			while (auto pTh = hive.Wait())
				finish_thread(pTh);
		}
		else ::AfxMessageBox(_T("There nothing to scan!"), MB_OK | MB_ICONERROR);

		view.ScanThreadFinished();

		return 0;
	}

}