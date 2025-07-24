// ChildView.cpp : implementation of the CChildView class
//
#include "pch.h"
#include "framework.h"
#include "Trade View.h"
#include "ChildView.h"
#include "DrawChart.h"
#include "CStringDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

constexpr auto
SECTION_SETTINGS{ _T("Settings") },
ENTRY_SHOW_INFO{ _T("ShowInfo") };

constexpr COLORREF model_colors[] = { RGB(0,255,0), RGB(220,130,0), RGB(0,130,220), };

// CChildView
CChildView::CChildView()
	:m_isScanningMode{ FALSE }
	, m_doShowInfo{ FALSE }
	, m_iListWidth{ 400 }
	, m_SetsDlg{ this }
{
	LOGFONT lf{};

	lf.lfHeight = 26;
	lf.lfWeight = FW_BOLD;
	wcscpy_s(lf.lfFaceName, _T("Arial"));
	m_fontScanning.CreateFontIndirect(&lf);

	lf.lfHeight = 16;
	lf.lfWeight = FW_NORMAL;
	wcscpy_s(lf.lfFaceName, _T("Times New Roman"));
	m_InfoFont.CreateFontIndirect(&lf);

	//lf.lfWeight = FW_BOLD;
	//m_InfoFontBold.CreateFontIndirect(&lf);

	m_doShowInfo = theApp.GetProfileInt(SECTION_SETTINGS, ENTRY_SHOW_INFO, FALSE);
}

CChildView::~CChildView()
{
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_ADD_FOLDER, &CChildView::OnAddFolder)
	ON_UPDATE_COMMAND_UI(ID_ADD_FOLDER, &CChildView::OnUpdateAddFolder)
	ON_COMMAND(ID_SCAN_TRADES, &CChildView::OnScanTrades)
	ON_UPDATE_COMMAND_UI(ID_SCAN_TRADES, &CChildView::OnUpdateScanTrades)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, ID_LIST_CTRL, OnListItemChanged)
	ON_WM_DESTROY()
	ON_MESSAGE((UINT)Message::WM_SCAN_FINISHED, OnScanFinished)
	ON_MESSAGE((UINT)Message::WM_BETTER_RESULT, OnBetterResult)
	//ON_MESSAGE(WM_BETTER_RESULT, OnBetterResult)
	ON_COMMAND(ID_REMOVE_SELECTED, &CChildView::OnRemoveSelected)
	ON_UPDATE_COMMAND_UI(ID_REMOVE_SELECTED, &CChildView::OnUpdateRemoveSelected)
	ON_COMMAND(ID_FILE_SAVE, &CChildView::OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CChildView::OnUpdateFileSave)
	ON_COMMAND(ID_SWAP_TABLES, &CChildView::OnSwapTables)
	ON_UPDATE_COMMAND_UI(ID_SWAP_TABLES, &CChildView::OnUpdateSwapTables)
	ON_COMMAND(ID_SHOW_INFO, &CChildView::OnShowInfo)
	ON_UPDATE_COMMAND_UI(ID_SHOW_INFO, &CChildView::OnUpdateShowInfo)
END_MESSAGE_MAP()

// CChildView message handlers
BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

void CChildView::OnPaint()
{
	constexpr auto margin{ 15 };

	CPaintDC dc(this); // device context for painting

	dc.FillSolidRect(m_Canvas, RGB(20, 20, 20));
	auto rect{ m_Canvas };
	rect.DeflateRect(margin, margin, margin, margin);

	if (m_isScanningMode)
	{
		auto const info{ m_Scan.GetInfo() };
		if (m_Scan.IsScanning())
		{
			int const iSave{ dc.SaveDC() };
			CString str;
			str.Format(_T("%s ... %I64u / %I64u, %.1f%%"),
				info.isStopping ? _T("STOPPING") : _T("Scanning"),
				info.finished,
				info.total,
				info.total ? info.finished * 100. / info.total : .0);
			dc.SetTextColor(RGB(200, 200, 0));
			dc.SetBkMode(TRANSPARENT);
			dc.SelectObject(&m_fontScanning);
			dc.DrawText(str, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			dc.RestoreDC(iSave);
		}
		else
		{
			int i{ 0 };
			constexpr int color_num{ sizeof(model_colors) / sizeof(COLORREF) };

			auto pos{ m_List.GetFirstSelectedItemPosition() };

			Draw::CummulativeChart draw;
			while (pos)
			{
				int ind = m_List.GetNextSelectedItem(pos);
				auto data = m_List.GetItemData(ind);

				assert(ind > -1 && ind < m_Best.size());
				assert(data > -1 && data < m_Best.size());

				for (auto p : m_Best[data].GetSubModels())
					draw.Add(*p, model_colors[i++ % color_num]);

			}
			draw.Draw(dc, rect);
		}
	}
	else
	{
		int i{ 0 };
		constexpr int color_num{ sizeof(model_colors) / sizeof(COLORREF) };
		auto pos{ m_List.GetFirstSelectedItemPosition() };

		Draw::CummulativeChart draw;
		while (pos)
		{
			int ind = m_List.GetNextSelectedItem(pos);
			auto data = m_List.GetItemData(ind);
			assert(ind > -1 && ind < m_Best.size());
			assert(data > -1 && data < m_Best.size());
			draw.Add(m_Files[data], model_colors[i++ % color_num]);
		}
		draw.Draw(dc, rect);
	}

	if (m_doShowInfo)
		DrawInfo(dc, rect);
}

void CChildView::DrawInfo(CDC& dc, CRect const& canvas)
{
	int i{ 0 };
	constexpr int color_num{ sizeof(model_colors) / sizeof(COLORREF) };
	auto pos{ m_List.GetFirstSelectedItemPosition() };
	auto rect{ canvas };
	auto const iSave{ dc.SaveDC() };
	dc.SelectObject(&m_InfoFont);

	auto draw_item = [&dc, this](SetFile::value_type& val, COLORREF color, CRect& r)
		{
			dc.SetTextColor(color);
			dc.DrawText(val.first.c_str(), (int)val.first.length(), r, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT);
			dc.DrawText(val.first.c_str(), (int)val.first.length(), r, DT_LEFT | DT_TOP | DT_SINGLELINE);
			r.left = r.right + 5;

			dc.SetTextColor(RGB(200, 200, 200));
			dc.DrawText(_T("="), 1, r, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT);
			dc.DrawText(_T("="), 1, r, DT_LEFT | DT_TOP | DT_SINGLELINE);
			r.left = r.right + 5;

			//dc.SelectObject(&m_InfoFontBold);
			dc.SetTextColor(color);
			dc.DrawText(val.second.c_str(), (int)val.second.length(), r, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_CALCRECT);
			dc.DrawText(val.second.c_str(), (int)val.second.length(), r, DT_LEFT | DT_TOP | DT_SINGLELINE);

			r.top = r.bottom;
		};

	auto draw_text = [&](SetFile&& sets, COLORREF color)
		{
			auto r{ rect };

			if (sets.empty())
			{
				CString const str{ _T("No settings file found!") };
				dc.SetTextColor(color);
				dc.DrawText(str, r, DT_LEFT | DT_TOP | DT_CALCRECT);
				dc.DrawText(str, r, DT_LEFT | DT_TOP);
			}
			else for (auto& item : sets)
			{
				draw_item(item, color, r);
				r.left = rect.left;
			}

			rect.top = r.bottom + 5;
		};

	while (pos)
	{
		int const index{ m_List.GetNextSelectedItem(pos) };
		auto const data{ m_List.GetItemData(index) };

		if (m_isScanningMode)
			for (auto pFile : m_Best[data].GetSubModels())
				draw_text(pFile->GetSettings(), model_colors[i++ % color_num]);
		else
			draw_text(m_Files[data].GetSettings(), model_colors[i++ % color_num]);
	}

	dc.RestoreDC(iSave);
}

void CChildView::OnAddFolder()
{
	BROWSEINFO info = {};
	WCHAR path[MAX_PATH + 1];

	auto pidl = ::SHBrowseForFolder(&info);
	if (pidl && ::SHGetPathFromIDList(pidl, path))
	{
		BeginWaitCursor();
		LoadFolder(path);
		LoadFiles();
		Invalidate();
		EndWaitCursor();
	}
}

void CChildView::LoadFiles()
{
	m_List.SetRedraw(FALSE);

	for (auto& file : m_Files)
		AddListItem(file.GetStats());

	AutosizeColumns();
	m_List.SetRedraw();
}

void CChildView::OnUpdateAddFolder(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_Scan.IsScanning());
}

void CChildView::LoadFolder(fs::path const& path)
{

	auto proc = [](fs::path entry)->TradeFile { return { entry }; };

	auto load_files = [&]
		{
			std::vector<std::future<TradeFile>> ths;

			for (auto const& entry : fs::directory_iterator(path))
				if (entry.is_regular_file())
					if (entry.path().extension() == L".csv")//
						ths.push_back(std::async(std::launch::async, proc, entry));
			return ths;
		};

	for (auto& f : load_files())
	{
		f.wait();
		m_Files.push_back(f.get());
		m_Files.back().SetID((int)m_Files.size());
	}
}

void ResizeListCtrlToFitColumns(CListCtrl* pListCtrl)
{
	if (!pListCtrl || !::IsWindow(pListCtrl->GetSafeHwnd()))
		return;

	CHeaderCtrl* pHeader = pListCtrl->GetHeaderCtrl();
	if (!pHeader)
		return;

	int totalColumnWidth = 0;
	int columnCount = pHeader->GetItemCount();

	for (int i = 0; i < columnCount; ++i)
		totalColumnWidth += pListCtrl->GetColumnWidth(i);

	// Add vertical scroll bar width if it's visible
	SCROLLINFO si = { sizeof(si), SIF_RANGE | SIF_PAGE };
	if (::GetScrollInfo(pListCtrl->GetSafeHwnd(), SB_VERT, &si) &&
		(si.nMax - si.nMin + 1 > (int)si.nPage))
	{
		totalColumnWidth += ::GetSystemMetrics(SM_CXVSCROLL);
	}

	// Add a little extra padding to avoid rounding issues
	totalColumnWidth += 4;

	// Get current position and height of list control
	CRect rect;
	pListCtrl->GetWindowRect(&rect);
	pListCtrl->GetParent()->ScreenToClient(&rect);

	// Resize control
	pListCtrl->SetWindowPos(nullptr, rect.left, rect.top, totalColumnWidth, rect.Height(), SWP_NOZORDER);
}


void CChildView::AutosizeColumns()
{
	for (int i = 0; i < m_List.GetHeaderCtrl()->GetItemCount(); i++)
		m_List.SetColumnWidth(i, LVSCW_AUTOSIZE);

	m_iListWidth = 0;
	for (int i = 0; i < m_List.GetHeaderCtrl()->GetItemCount(); i++)
		m_iListWidth += m_List.GetColumnWidth(i);

	m_iListWidth = max(m_iListWidth, 50);
	m_iListWidth += ::GetSystemMetrics(SM_CXVSCROLL) + 4;

	UpdateLayout();
}

void CChildView::UpdateLayout()
{
	GetClientRect(m_Canvas);
	m_Canvas.left = min(m_Canvas.Width() / 3, m_iListWidth);
	m_List.MoveWindow(0, 0, m_Canvas.left, m_Canvas.Height());
}

void CChildView::OnScanTrades()
{
	if (m_Scan.IsScanning())
	{
		m_Scan.Stop(FALSE);
		Invalidate();
	}
	else if (m_SetsDlg.DoModal() == IDOK)
	{
		PrepareList(TRUE);
		if (!m_Scan.Start(m_Files, m_SetsDlg))
		{
			PrepareList(FALSE);
			LoadFiles();
		}
		Invalidate();
	}
}

void CChildView::OnUpdateScanTrades(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_Files.size() > 1 && !m_Scan.IsStoppingScan() /*&& !m_isScanningMode*/);
	pCmdUI->SetCheck(m_Scan.IsScanning());
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(rect);
	rect.right = rect.left + rect.Width() / 3;

	if (!m_List.Create(WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS, rect, this, ID_LIST_CTRL))
		return -1;
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	PrepareList(FALSE);

	return 0;
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	UpdateLayout();
}

void CChildView::OnListItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	auto pNMLV{ reinterpret_cast<LPNMLISTVIEW>(pNMHDR) };

	if (pNMLV->uChanged & LVIF_STATE)
	{

		if (pNMLV->uNewState & LVIS_SELECTED)// != pNMLV->uOldState & LVIS_SELECTED
			InvalidateRect(m_Canvas);
	}

	*pResult = 0;
}

void CChildView::AddListItem(TRADES_STATISTIC const& stat)
{
	CString str;
	LVITEM item{};

	//m_List.InsertColumn(col++, _T("#"));
	item.iItem = m_List.GetItemCount();
	item.mask = LVIF_TEXT | LVIF_PARAM;
	str.Format(_T("%d"), item.iItem + 1);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	item.lParam = item.iItem;
	m_List.InsertItem(&item);

	//m_List.InsertColumn(col++, _T("NET"));
	item.mask = LVIF_TEXT;
	++item.iSubItem;
	str.Format(_T("%.2f"), (stat.profit - stat.loss) / 100.);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	//m_List.InsertColumn(col++, _T("Profit"));
	++item.iSubItem;
	str.Format(_T("%.2f"), stat.profit / 100.);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	//m_List.InsertColumn(col++, _T("Loss"));
	++item.iSubItem;
	str.Format(_T("%.2f"), stat.loss / 100.);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	//m_List.InsertColumn(col++, _T("Factor"));
	++item.iSubItem;
	str.Format(_T("%.2f"), double(stat.profit) / stat.loss);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	//m_List.InsertColumn(col++, _T("Trades"));
	++item.iSubItem;
	str.Format(_T("%d"), stat.iWon + stat.iLost);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	//m_List.InsertColumn(col++, _T("Won"));
	++item.iSubItem;
	str.Format(_T("%d"), stat.iWon);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	//m_List.InsertColumn(col++, _T("Lost"));
	++item.iSubItem;
	str.Format(_T("%d"), stat.iLost);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	//m_List.InsertColumn(col++, _T("std error"));
	++item.iSubItem;
	str.Format(_T("%.4g"), stat.custom);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	++item.iSubItem;
	str.Format(_T("%.2f"), stat.timeTotal ? stat.timeIn * 100. / stat.timeTotal : .0);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);
}

void CChildView::PrepareList(BOOL const bList4Scan)
{
	m_List.DeleteAllItems();
	while (m_List.GetHeaderCtrl()->GetItemCount())
		m_List.DeleteColumn(0);

	int col{ 0 };
	if (m_isScanningMode = bList4Scan)
	{
		m_List.InsertColumn(col++, _T("#"), LVCFMT_LEFT, 20);
		m_List.InsertColumn(col++, _T("NET"), LVCFMT_RIGHT, 50);
		m_List.InsertColumn(col++, _T("Factor"), LVCFMT_RIGHT, 50);
		m_List.InsertColumn(col++, _T("Custom"), LVCFMT_RIGHT, 50);
		m_List.InsertColumn(col++, _T("Pair"), LVCFMT_CENTER, 50);
		m_List.InsertColumn(col++, _T("Trades"), LVCFMT_RIGHT, 50);
		m_List.InsertColumn(col++, _T("InTime, %"), LVCFMT_RIGHT);
		m_List.InsertColumn(col++, _T("Con, %"), LVCFMT_RIGHT);
		m_List.InsertColumn(col++, _T("De, %"), LVCFMT_RIGHT);
	}
	else
	{
		m_List.InsertColumn(col++, _T("#"));
		m_List.InsertColumn(col++, _T("NET"), LVCFMT_RIGHT);
		m_List.InsertColumn(col++, _T("Profit"), LVCFMT_RIGHT);
		m_List.InsertColumn(col++, _T("Loss"), LVCFMT_RIGHT);
		m_List.InsertColumn(col++, _T("Factor"), LVCFMT_RIGHT);
		m_List.InsertColumn(col++, _T("Trades"), LVCFMT_RIGHT);
		m_List.InsertColumn(col++, _T("Won"), LVCFMT_RIGHT);
		m_List.InsertColumn(col++, _T("Lost"), LVCFMT_RIGHT);
		m_List.InsertColumn(col++, _T("Custom"), LVCFMT_RIGHT);
		m_List.InsertColumn(col++, _T("InTime, %"), LVCFMT_RIGHT);
	}
}

void CChildView::UpdateListScanResults(size_t const index)
{
	auto& model{ m_Best[index] };

	LVITEM item{};
	CString str;

	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = (int)index;
	item.lParam = index;
	str.Format(_T("%I64u"), index + 1);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	item.mask = LVIF_TEXT;
	++item.iSubItem;
	str.Format(_T("%.0f"), model.GetNet());
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	++item.iSubItem;
	str.Format(_T("%.2f"), model.GetFactor());
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	++item.iSubItem;
	str.Format(_T("%.2f"), model.GetCustom());
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	++item.iSubItem;
	str.Format(_T("%d : %d"), model.GetSubModels().front()->GetID(), model.GetSubModels().back()->GetID());
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	auto& inters{ model.GetIntervals() };
	auto const in_time{ inters.GetInTime() }, con{ inters.GetConstructive() }, de{ inters.GetDestructive() };

	++item.iSubItem;
	str.Format(_T("%I64u"), inters.GetTradeCount());
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	++item.iSubItem;
	str.Format(_T("%.2f"), in_time * 100 / (double)inters.GetTotalTime());
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	++item.iSubItem;
	str.Format(_T("%.2f"), con * 100 / (double)in_time);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);

	++item.iSubItem;
	str.Format(_T("%.2f"), de * 100 / (double)in_time);
	item.pszText = (LPTSTR)(LPCTSTR)str;
	m_List.SetItem(&item);
}

void CChildView::UpdateScanningList()
{
	m_List.SetRedraw(FALSE);

	while (m_List.GetItemCount() < m_Best.size())
		m_List.InsertItem(0, _T(""));

	while (m_List.GetItemCount() > m_Best.size())
		m_List.DeleteItem(0);

	for (size_t i = 0; i < m_Best.size(); i++)
		UpdateListScanResults(i);

	AutosizeColumns();
	m_List.SetRedraw();
}

void CChildView::OnDestroy()
{
	CWnd::OnDestroy();
	if (m_Scan.IsScanning())
		m_Scan.Stop(TRUE);
}

LRESULT CChildView::OnScanFinished(WPARAM wParam, LPARAM)
{
	if (wParam)	//	all scans
	{

	}
	else	//	single scan
	{
	}

	Invalidate();

	return 0;
}

LRESULT CChildView::OnBetterResult(WPARAM, LPARAM)
{
	m_Best = m_Scan.GetBest();
	UpdateScanningList();
	Invalidate();
	return 0;
}

std::set<int> CChildView::GetSelectedIndexes()
{
	std::set<int> sels;

	auto pos{ m_List.GetFirstSelectedItemPosition() };
	while (pos)
	{
		auto index{ m_List.GetNextSelectedItem(pos) };
		auto data{ (int)m_List.GetItemData(index) };
		sels.insert(data);
	}

	return sels;
}

void CChildView::OnRemoveSelected()
{
	auto const sels{ GetSelectedIndexes() };
	assert(!sels.empty());

	if (m_isScanningMode)
	{
		decltype(m_Best) temp;
		for (int i = 0; i < (int)m_Best.size(); i++)
			if (!sels.contains(i))
				temp.push_back(std::move(m_Best[i]));

		std::swap(m_Best, temp);
	}
	else
	{
		decltype(m_Files) temp;
		for (int i = 0; i < (int)m_Files.size(); i++)
			if (!sels.contains(i))
				temp.push_back(std::move(m_Files[i]));

		std::swap(m_Files, temp);
	}
	LoadList();
}

void CChildView::OnUpdateRemoveSelected(CCmdUI* pCmdUI)
{
	BOOL canRemove;

	if (m_isScanningMode)
		canRemove = !m_Best.empty();
	else canRemove = !m_Files.empty() && m_Best.empty();

	pCmdUI->Enable(canRemove && m_List.GetSelectedCount());
}

void CChildView::OnFileSave()
{

	BROWSEINFO info = {};
	info.ulFlags = BIF_USENEWUI;
	WCHAR path[MAX_PATH + 1];

	auto save_one_file = [&path](fs::path src, CString const& new_name)
		{
			fs::path dst{ path };
			if (new_name.IsEmpty())
			{
				dst /= src.filename();
				dst.replace_extension();
			}
			else dst /= new_name.GetString();
			src.replace_extension();

			try
			{
				auto from{ src }, to{ dst };
				from += ".csv", to += ".csv";
				fs::copy_file(from, to);
				from.replace_extension(".set"), to.replace_extension(".set");
				fs::copy_file(from, to);
			}
			catch (std::exception&) {}
		};

	auto save_files = [&path, save_one_file](fs::path src, CString const& new_name, char ex)
		{
			fs::path dst{ path };
			if (new_name.IsEmpty())
			{
				dst /= src.filename();
				dst.replace_extension();
			}
			else dst /= new_name.GetString();
			src.replace_extension();

			dst += "_";
			dst += ex;

			try
			{
				auto from{ src }, to{ dst };
				from += ".csv", to += ".csv";
				fs::copy_file(from, to);
				from.replace_extension(".set"), to.replace_extension(".set");
				fs::copy_file(from, to);
			}
			catch (std::exception&) {}
		};

	CStringDlg dlg;
	auto pidl = ::SHBrowseForFolder(&info);
	if (pidl
		&& ::SHGetPathFromIDList(pidl, path)
		&& dlg.DoModal() == IDOK)
	{
		auto const sels{ GetSelectedIndexes() };
		assert(!sels.empty());

		for (auto ind : sels)
			if (m_isScanningMode)
			{
				auto& mods{ m_Best[ind].GetSubModels() };
				for (size_t i = 0; i < mods.size(); i++)
					save_files(mods[i]->GetFilepath(), dlg.m_Text, 'a' + (char)i);
			}
			else
				save_one_file(m_Files[ind].GetFilepath(), dlg.m_Text);
	}
}

void CChildView::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	auto const sel{ m_List.GetSelectedCount() };
	pCmdUI->Enable(sel == 1);
}

void CChildView::OnSwapTables()
{
	BeginWaitCursor();
	LoadList(!m_isScanningMode);
	EndWaitCursor();
}

void CChildView::OnUpdateSwapTables(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_isScanningMode);
	//pCmdUI->Enable(!m_Files.empty() && m_Best.empty());
}

void CChildView::LoadBest()
{
	m_List.SetRedraw(FALSE);
	for (int i = 0; i < (int)m_Best.size(); i++)
	{
		m_List.InsertItem(i, _T(""));
		UpdateListScanResults(i);
	}
	AutosizeColumns();
	m_List.SetRedraw();
}

void CChildView::LoadList(BOOL const isScanning)
{
	PrepareList(isScanning);
	isScanning ? LoadBest() : LoadFiles();
	Invalidate();
}

void CChildView::LoadList()
{
	LoadList(m_isScanningMode);
}
void CChildView::OnShowInfo()
{
	m_doShowInfo = !m_doShowInfo;
	Invalidate();
	theApp.WriteProfileInt(SECTION_SETTINGS, ENTRY_SHOW_INFO, m_doShowInfo);
}

void CChildView::OnUpdateShowInfo(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_doShowInfo);
	pCmdUI->Enable(m_List.GetSelectedCount());
}
