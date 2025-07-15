// CScanSettingsDlg.cpp : implementation file
//

#include "pch.h"
#include "Trade View.h"
#include "afxdialogex.h"
#include "CScanSettingsDlg.h"

#define KEY_SETTINGS			_T("Settings")
#define ENTRY_SCAN_COUNT		_T("ScanCount")
#define ENTRY_MIN_PROFIT		_T("MinProfit")
#define ENTRY_MIN_TRADES		_T("MinTrades")
#define ENTRY_MODEL_COUNT	_T("ModelCount")
#define ENTRY_SCAN4_CUSTOM	_T("Scan4Custom")
#define ENTRY_SCAN4_FACTOR	_T("Scan4Factor")
#define ENTRY_SCAN4_NET		_T("Scan4Net")
#define ENTRY_USE_MIN_PROFIT		_T("UseMinProfit")
#define ENTRY_USE_MIN_TRADES		_T("UseMinTrades")
// CScanSettingsDlg dialog

IMPLEMENT_DYNAMIC(CScanSettingsDlg, CDialogEx)

CScanSettingsDlg::CScanSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SCAN_SETTINGS_DLG, pParent)
	, m_Scan_Count(0)
	, m_Min_Profit(0)
	, m_Min_Trades(0)
	, m_Model_Count(0)
	, m_Scan4_Custom(FALSE)
	, m_Scan4_Factor(FALSE)
	, m_Scan4_Net(FALSE)
	, m_UseMinProfit(FALSE)
	, m_UseMinTrades(FALSE)
{
	m_Scan_Count = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_SCAN_COUNT, 10);
	m_Min_Profit = std::stof(theApp.GetProfileString(KEY_SETTINGS, ENTRY_MIN_PROFIT, _T("0.0")).GetString());
	m_Min_Trades = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_MIN_TRADES, 100);
	m_Model_Count = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_MODEL_COUNT, 2);
	m_Scan4_Custom = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_SCAN4_CUSTOM, FALSE);
	m_Scan4_Factor = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_SCAN4_FACTOR, FALSE);
	m_Scan4_Net = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_SCAN4_NET, TRUE);
	m_UseMinProfit = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_USE_MIN_PROFIT, TRUE);
	m_UseMinTrades = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_USE_MIN_TRADES, TRUE);
}

CScanSettingsDlg::~CScanSettingsDlg()
{
}

void CScanSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BEST_SCAN_COUNT, m_Scan_Count);
	DDX_Text(pDX, IDC_MINIMUM_PROFIT, m_Min_Profit);
	DDX_Text(pDX, IDC_MINIMUM_TRADES, m_Min_Trades);
	DDX_Text(pDX, IDC_MODEL_COUNT, m_Model_Count);
	DDX_Check(pDX, IDC_SCAN4_CUSTOM, m_Scan4_Custom);
	DDX_Check(pDX, IDC_SCAN4_FACTOR, m_Scan4_Factor);
	DDX_Check(pDX, IDC_SCAN4_NET, m_Scan4_Net);
	DDX_Check(pDX, IDC_USE_MINIMUM_PROFIT, m_UseMinProfit);
	DDX_Check(pDX, IDC_USE_MINIMUM_TRADES, m_UseMinTrades);
}


BEGIN_MESSAGE_MAP(CScanSettingsDlg, CDialogEx)
END_MESSAGE_MAP()


// CScanSettingsDlg message handlers

void CScanSettingsDlg::OnOK()
{
	if (UpdateData())
	{
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_SCAN_COUNT, m_Scan_Count);
		CString str;
		str.Format(_T("%.2f"), m_Min_Profit);
		theApp.WriteProfileString(KEY_SETTINGS, ENTRY_MIN_PROFIT, str);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_MIN_TRADES, m_Min_Trades);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_MODEL_COUNT, m_Model_Count);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_SCAN4_CUSTOM, m_Scan4_Custom);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_SCAN4_FACTOR, m_Scan4_Factor);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_SCAN4_NET, m_Scan4_Net);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_USE_MIN_PROFIT, m_UseMinProfit);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_USE_MIN_TRADES, m_UseMinTrades);
	}

	CDialogEx::OnOK();
}
