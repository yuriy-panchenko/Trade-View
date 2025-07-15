#include "pch.h"
#include "SortListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSortListCtrl::CSortListCtrl()
{
	m_iNumColumns = 0;
	m_iSortColumn = -1;
	m_HeaderHwnd = NULL;
}

CSortListCtrl::~CSortListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSortListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSortListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortListCtrl message handlers

void CSortListCtrl::PreSubclassWindow()
{
	// the list control must have the report style.
	ASSERT(GetStyle() & LVS_REPORT);

	CListCtrl::PreSubclassWindow();
	if (GetHeaderCtrl()->GetSafeHwnd())
	{
		m_ctlHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());
		m_HeaderHwnd = m_ctlHeader.GetSafeHwnd();
	}
}

// the heading text is in the format column 1 text,column 1 width;column 2 text,column 3 width;etc.

bool IsNumber(LPCTSTR pszText)
{
	ASSERT_VALID_STRING(pszText);

	for (int i = 0; i < lstrlen(pszText); i++)
	{
		if ((pszText[i] < '0' || pszText[i]>'9') && pszText[i] != '.' && pszText[i] != '-')return false;
	}
	return true;
}


int NumberCompare(LPCTSTR pszNumber1, LPCTSTR pszNumber2)
{
	ASSERT_VALID_STRING(pszNumber1);
	ASSERT_VALID_STRING(pszNumber2);

	const double iNumber1 = std::stof(pszNumber1);
	const double iNumber2 = std::stof(pszNumber2);

	if (iNumber1 < iNumber2)
		return -1;

	if (iNumber1 > iNumber2)
		return 1;

	return 0;
}


bool IsDate(LPCTSTR pszText)
{
	ASSERT_VALID_STRING(pszText);

	// format should be 99/99/9999.

	if (lstrlen(pszText) != 10)
		return false;

	return _istdigit(pszText[0])
		&& _istdigit(pszText[1])
		&& pszText[2] == _T('/')
		&& _istdigit(pszText[3])
		&& _istdigit(pszText[4])
		&& pszText[5] == _T('/')
		&& _istdigit(pszText[6])
		&& _istdigit(pszText[7])
		&& _istdigit(pszText[8])
		&& _istdigit(pszText[9]);
}


int DateCompare(const CString& strDate1, const CString& strDate2)
{
	const int iYear1 = std::stoi(strDate1.Mid(6, 4).GetString());
	const int iYear2 = std::stoi(strDate2.Mid(6, 4).GetString());

	if (iYear1 < iYear2)
		return -1;

	if (iYear1 > iYear2)
		return 1;

	const int iMonth1 = std::stoi(strDate1.Mid(3, 2).GetString());
	const int iMonth2 = std::stoi(strDate2.Mid(3, 2).GetString());

	if (iMonth1 < iMonth2)
		return -1;

	if (iMonth1 > iMonth2)
		return 1;

	const int iDay1 = std::stoi(strDate1.Mid(0, 2).GetString());
	const int iDay2 = std::stoi(strDate2.Mid(0, 2).GetString());

	if (iDay1 < iDay2)
		return -1;

	if (iDay1 > iDay2)
		return 1;

	return 0;
}


int CALLBACK CSortListCtrl::CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData)
{
	CSortListCtrl* pListCtrl = reinterpret_cast<CSortListCtrl*>(lParamData);
	ASSERT(pListCtrl->IsKindOf(RUNTIME_CLASS(CListCtrl)));

	CString pszText1 = pListCtrl->GetItemText((int)lParam1, pListCtrl->m_iSortColumn);
	CString pszText2 = pListCtrl->GetItemText((int)lParam2, pListCtrl->m_iSortColumn);

	ASSERT_VALID_STRING(pszText1);
	ASSERT_VALID_STRING(pszText2);

	if (IsNumber(pszText1))
		return pListCtrl->m_AccArr[pListCtrl->m_iSortColumn] ? NumberCompare(pszText1, pszText2) : NumberCompare(pszText2, pszText1);
	else if (IsDate(pszText1) && pszText2.GetLength() >= 6)
		return pListCtrl->m_AccArr[pListCtrl->m_iSortColumn] ? DateCompare(pszText1, pszText2) : DateCompare(pszText2, pszText1);
	else
		// text.
		return pListCtrl->m_AccArr[pListCtrl->m_iSortColumn] ? lstrcmp(pszText1, pszText2) : lstrcmp(pszText2, pszText1);
}


void CSortListCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	const int iColumn = pNMListView->iSubItem;

	// if it's a second click on the same column then reverse the sort order,
	// otherwise sort the new column in ascending order.
	CArray<DWORD_PTR> ar;
	int sel = GetSelectionMark();
	ar.SetSize(GetItemCount());
	for (int n = 0; n < GetItemCount(); n++)
	{
		ar[n] = GetItemData(n);
		SetItemData(n, n);
	}
	Sort(iColumn, m_AccArr[iColumn]);
	for (auto n = 0; n < ar.GetSize(); n++)SetItemData(n, ar[GetItemData(n)]);
	if (sel > -1)EnsureVisible(sel, FALSE);
	*pResult = 0;
}


void CSortListCtrl::Sort(int iColumn, BOOL bAscending)
{
	if (m_iSortColumn == iColumn)m_AccArr[iColumn] = !bAscending;
	m_iSortColumn = iColumn;

	// show the appropriate arrow in the header control.
	m_ctlHeader.SetSortArrow(m_iSortColumn, m_AccArr[iColumn]);

	VERIFY(SortItems(CompareFunction, (DWORD_PTR)this));
}


void CSortListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (&pDispInfo->item != NULL)
	{
		SetItem(&pDispInfo->item);
	}
	*pResult = 0;
}

void CSortListCtrl::AutoSizeColumns()
{
	for (int n = 0; n < GetHeaderCtrl()->GetItemCount(); n++)SetColumnWidth(n, LVSCW_AUTOSIZE);
}

int CSortListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_HeaderHwnd)m_ctlHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());
	return 0;
}


int CSortListCtrl::InsertColumn(int nCol, const LVCOLUMN* pColumn)
{
	m_AccArr.InsertAt(nCol, FALSE);
	return CListCtrl::InsertColumn(nCol, pColumn);
}

int CSortListCtrl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = nFormat;
	lvc.cx = nWidth;
	lvc.pszText = (LPTSTR)lpszColumnHeading;
	lvc.iSubItem = nSubItem;
	return InsertColumn(nCol, &lvc);
}

BOOL CSortListCtrl::DeleteColumn(int nCol)
{
	m_AccArr.RemoveAt(nCol);
	return CListCtrl::DeleteColumn(nCol);
}
