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

		m_Comp.Init(dlg.m_Scan_Count, dlg.m_Scan4_Net, dlg.m_Scan4_Factor, dlg.m_Scan4_Custom);

		m_Files = dlg.Filter(files);

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
					mods.UpdateBest(ret->GetBest());
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
			mods.UpdateBest(pTh->GetBest());

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

		static_cast<CMainFrame*>(theApp.GetMainWnd())->GetChildView().PostMessage((UINT)CChildView::Message::WM_SCAN_FINISHED, 1);
	}

	void Models::UpdateBest(BestModels&& mods)
	{
		CSingleLock _o{ &m_CS, TRUE };

		m_Info.isStopping = m_bStopping;
		++m_Info.finished;

		bool bUpdate{ false };

		for (auto& m : mods)
			bUpdate |= m_Comp.Test(m);

		UINT const msg{ (UINT)(bUpdate ? CChildView::Message::WM_BETTER_RESULT : CChildView::Message::WM_SCAN_FINISHED) };
		static_cast<CMainFrame*>(theApp.GetMainWnd())->GetChildView().PostMessage(msg);
	}

	void Models::SetInfoTotalScanCount(size_t total)
	{
		CSingleLock _o{ &m_CS, TRUE };
		m_Info.total = total;
	}

	BestModels Models::GetBest()const
	{
		return m_Comp.GetBest();
	}

	Models::INFO Models::GetInfo() const
	{
		return m_Info;
	}
}