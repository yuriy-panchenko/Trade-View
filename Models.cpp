#include "pch.h"
#include "Models.h"
#include "CScanSettingsDlg.h"
#include "CWorketThread.h"
#include "Trade View.h"
#include "MainFrm.h"

#ifdef DEBUG
#define DEBUG_SINGLE_THREAD
#endif // DEBUG


namespace Scan
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Models::Models()
		:m_pSets{ nullptr }
		, m_pWaitTh{ nullptr }
		, m_bStopping{ FALSE }
	{
	}

	BOOL Models::Start(std::vector<TradeFile> const& files, CScanSettingsDlg const& dlg)
	{
		m_pSets = &dlg;
		m_Files.clear();
		m_Files.reserve(files.size());
		m_bStopping = FALSE;
		ZeroMemory(&m_Info, sizeof INFO);

		/*int scans{ 0 };
		if (dlg.m_Scan4_Net)
			scans += dlg.m_Scan_Count;
		if (dlg.m_Scan4_Factor)
			scans += dlg.m_Scan_Count;
		if (dlg.m_Scan4_Custom)
			scans += dlg.m_Scan_Count;
		scans = max(scans, dlg.m_Scan_Count);*/

		ModelBunch::InTimeMax = dlg.m_InTime_Max;
		ModelBunch::InTimeMin = dlg.m_InTime_Min;
		ModelBunch::TradesMax = dlg.m_Trades_Max;
		ModelBunch::TradesMin = dlg.m_Trades_Min;
		ModelBunch::ConMax = dlg.m_Con_Max;
		ModelBunch::ConMin = dlg.m_Con_Min;

		//m_Comp.Init(dlg.m_Scan_Count, dlg.m_Scan4_Net, dlg.m_Scan4_Factor, dlg.m_Scan4_Custom);

		m_Files = dlg.Filter(files);
		auto total_scan_count{ m_Files.size() };
		if (total_scan_count)
		{
			total_scan_count *= total_scan_count - 1;
			total_scan_count >>= 1;
		}
		m_NetTable.reserve(total_scan_count);
		m_FactorTable.reserve(total_scan_count);
		m_CustomTable.reserve(total_scan_count);

		if (m_Files.size() < 2)
			::AfxMessageBox(_T("There nothing to scan!"), MB_OK | MB_ICONERROR);
		else m_pWaitTh = ::AfxBeginThread(DoWork, this);

		return m_Files.size() > 1;
	}

	void Models::Stop(BOOL bWait2Finish)
	{
		if (m_pWaitTh)
		{
			CSingleLock _o{ &m_CS, TRUE };
			m_bStopping = TRUE;
			_o.Unlock();

			if (bWait2Finish)
				if (*m_pWaitTh != INVALID_HANDLE_VALUE)
					::WaitForSingleObject(*m_pWaitTh, INFINITE);
		}
	}

	BOOL Models::IsScanning() const
	{
		CSingleLock _o{ &m_CS, TRUE };
		return m_pWaitTh != nullptr;
	}

	BOOL Models::IsStoppingScan() const
	{
		CSingleLock _o{ &m_CS, TRUE };
		return m_bStopping;
	}

	UINT Models::DoWork(LPVOID pData)
	{
		auto& mods{ *reinterpret_cast<Models*>(pData) };

		////////////////////////////////////////////////////////////////////
		size_t const total_scans{ mods.m_Files.size() - 1 };
		mods.SetInfoTotalScanCount(total_scans);

		std::vector<CWorkerThread*> ths;

		auto wait = [&ths]
			{
				std::vector<CWorkerThread*> working_threads;
				for (auto p : ths)
					if (p->IsWorking())
						working_threads.push_back(p);

				std::vector<HANDLE> hs;
				for (auto p : working_threads)
					hs.push_back(p->GetWorkingSyncObj());

				if (hs.empty())
					return (CWorkerThread*)nullptr;

				auto const res{ ::WaitForMultipleObjects((DWORD)hs.size(),hs.data(),FALSE, INFINITE) };
				if (res >= WAIT_OBJECT_0 && res < WAIT_OBJECT_0 + hs.size())
					return working_threads[res - WAIT_OBJECT_0];

				assert(false);
				return (CWorkerThread*)nullptr;
			};

		auto get_thread = [&]
			{
				CWorkerThread* ret;
#ifdef DEBUG_SINGLE_THREAD
				int maxThs = 1;
#else
				int maxThs = max(1, std::thread::hardware_concurrency() - 1);
#endif // DEBUG_SINGLE_THREAD

				if (ths.size() >= maxThs)
				{
					ret = wait();
					mods.CollectResults(ret);
				}
				else
				{
					ret = static_cast<CWorkerThread*> (::AfxBeginThread(RUNTIME_CLASS(CWorkerThread)));
					ret->Init(*mods.m_pSets, mods.m_Files.end());
					ths.push_back(ret);
				}
				return ret;
			};


		for (auto iter{ mods.m_Files.begin() }; iter != std::prev(mods.m_Files.end()) && !mods.IsStoppingScan(); ++iter)
			get_thread()->Start(iter);

		while (auto pTh{ wait() })
			mods.CollectResults(pTh);

		std::vector<HANDLE> hs;
		for (auto p : ths)
			hs.push_back(*p);
		for (auto p : ths)
			p->PostThreadMessage(WM_QUIT, 0, 0);
		::WaitForMultipleObjects((DWORD)hs.size(), hs.data(), TRUE, INFINITE);

		mods.OnWaitThreadFinished();
		return 0;
	}

	void Models::OnWaitThreadFinished()
	{
		{
			CSingleLock _o{ &m_CS, TRUE };
			m_pWaitTh = nullptr;
			m_bStopping = FALSE;
		}

		auto sort_results = [](auto& arr)
			{
				std::sort(arr.begin(), arr.end(), [](const auto& a, const auto& b) { return get<0>(a) > get<0>(b); });
			};

		sort_results(m_NetTable);
		sort_results(m_FactorTable);
		sort_results(m_CustomTable);

		static_cast<CMainFrame*>(theApp.GetMainWnd())->GetChildView().PostMessage((UINT)CChildView::Message::WM_SCAN_FINISHED, 1);
	}

	void operator+=(CWorkerThread::ScanResults& dst, CWorkerThread::ScanResults const& src)
	{
		dst.insert(dst.end(), src.begin(), src.end());
	}

	void Models::CollectResults(CWorkerThread* pTh)
	{
		CSingleLock _o{ &m_CS, TRUE };

		m_Info.isStopping = m_bStopping;
		++m_Info.finished;

		bool bUpdate{ false };

		if (m_pSets->m_Scan4_Net)
			m_NetTable += pTh->GetResults<BestModels::Net>();
		if (m_pSets->m_Scan4_Factor)
			m_FactorTable += pTh->GetResults<BestModels::Factor>();
		if (m_pSets->m_Scan4_Custom)
			m_CustomTable += pTh->GetResults<BestModels::Custom>();

		//UINT const msg{ (UINT)(bUpdate ? CChildView::Message::WM_BETTER_RESULT : CChildView::Message::WM_SCAN_FINISHED) };
		static_cast<CMainFrame*>(theApp.GetMainWnd())->GetChildView().PostMessage((UINT)CChildView::Message::WM_SCAN_FINISHED);
	}

	void Models::SetInfoTotalScanCount(size_t total)
	{
		CSingleLock _o{ &m_CS, TRUE };
		m_Info.total = total;
	}

	std::vector<model_pair> Models::Unite(pair_vector const& arr, size_t const K) const
	{
		std::set<TradeFile const*> used;
		std::vector<model_pair> selected;

		for (const auto& [score, p] : arr)
			if (!used.count(p.first) && !used.count(p.second))
			{
				selected.push_back(p);
				used.insert(p.first);
				used.insert(p.second);
				if (selected.size() == K)
					break;
			}

		return selected;
	}

	void operator+=(std::vector<model_pair>& dst, std::vector<model_pair> const& src)
	{
		dst.insert(dst.end(), src.begin(), src.end());
	}

	std::vector<model_pair> Models::Unite(size_t const K) const
	{
		std::vector<model_pair> ret, un;
		un += Unite(m_NetTable, K);
		un += Unite(m_FactorTable, K);
		un += Unite(m_CustomTable, K);

		for (auto p : un)
			if (std::find(ret.begin(), ret.end(), p) == ret.end())
				ret.push_back(p);
#ifdef DEBUG
			else
			{
				int y = 0;
			}
#endif // DEBUG


		return ret;
	}

	Models::INFO Models::GetInfo() const
	{
		CSingleLock _o{ &m_CS, TRUE };
		return m_Info;
	}

	BestModels Models::CollectBest(size_t const K) const
	{
		auto to_bunch = [](auto p)
			{
				ModelBunch mb;
				mb.Add(p.first);
				mb.Add(p.second);
				assert(mb.Calculate());
				return mb;
			};
	
		BestModels ret;

		for (auto model : Unite(K))
			ret.push_back(to_bunch(model));

		return ret;;
	}
}