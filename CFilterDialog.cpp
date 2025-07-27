// CFilterDialog.cpp : implementation file
//

#include "pch.h"
#include "Trade View.h"
#include "afxdialogex.h"
#include "CFilterDialog.h"
#include "MainFrm.h"

#define ENTRY_USE_MIN_PROFIT		_T("UseMinProfit")
#define ENTRY_USE_MIN_TRADES		_T("UseMinTrades")
#define ENTRY_MIN_PROFIT		_T("MinProfit")
#define ENTRY_MIN_TRADES		_T("MinTrades")
#define ENTRY_FILTER_DUPLICATES		_T("FilterDuplcates")

// CFilterDialog dialog

IMPLEMENT_DYNAMIC(CFilterDialog, CDialogEx)

CFilterDialog::CFilterDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILTER_DLG, pParent)
	, m_Min_Profit(0)
	, m_Min_Trades(0)
	, m_UseMinProfit(FALSE)
	, m_UseMinTrades(FALSE)
	, m_Info_Text(_T(""))
	, m_Filter_Duplicates(FALSE)
{
	m_UseMinProfit = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_USE_MIN_PROFIT, TRUE);
	m_UseMinTrades = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_USE_MIN_TRADES, TRUE);
	m_Min_Profit = std::stof(theApp.GetProfileString(KEY_SETTINGS, ENTRY_MIN_PROFIT, _T("0.0")).GetString());
	m_Min_Trades = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_MIN_TRADES, 100);
	m_Filter_Duplicates = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_FILTER_DUPLICATES, 100);
}

CFilterDialog::~CFilterDialog()
{
}

void CFilterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_USE_MINIMUM_PROFIT, m_UseMinProfit);
	DDX_Check(pDX, IDC_USE_MINIMUM_TRADES, m_UseMinTrades);
	DDX_Text(pDX, IDC_MINIMUM_PROFIT, m_Min_Profit);
	DDX_Text(pDX, IDC_MINIMUM_TRADES, m_Min_Trades);
	DDX_Text(pDX, IDC_INFO_TEXT, m_Info_Text);
	DDX_Check(pDX, IDC_FILTER_DUPLICATES, m_Filter_Duplicates);
}


BEGIN_MESSAGE_MAP(CFilterDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CFilterDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CFilterDialog message handlers

void CFilterDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}

void CFilterDialog::OnOK()
{
	if (UpdateData())
	{
		CString str;
		str.Format(_T("%.2f"), m_Min_Profit);
		theApp.WriteProfileString(KEY_SETTINGS, ENTRY_MIN_PROFIT, str);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_MIN_TRADES, m_Min_Trades);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_USE_MIN_PROFIT, m_UseMinProfit);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_USE_MIN_TRADES, m_UseMinTrades);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_FILTER_DUPLICATES, m_Filter_Duplicates);
	}

	CDialogEx::OnOK();
}

void CFilterDialog::UpdateInfo()
{
	auto& view{ static_cast<CMainFrame*>(theApp.GetMainWnd())->GetChildView() };
	//m_Info_Text.Format(_T("%s models, %s pairs"),
	//	InsertApostrofie(m_uModelCount),
	//	InsertApostrofie(((m_uModelCount * m_uModelCount) - m_uModelCount / 2)));
}

void CFilterDialog::UpdateControls()
{
	GetDlgItem(IDC_MINIMUM_PROFIT)->EnableWindow(m_UseMinProfit);
	GetDlgItem(IDC_MINIMUM_TRADES)->EnableWindow(m_UseMinTrades);
}