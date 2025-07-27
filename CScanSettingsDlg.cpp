// CScanSettingsDlg.cpp : implementation file
//

#include "pch.h"
#include "Trade View.h"
#include "afxdialogex.h"
#include "CScanSettingsDlg.h"
#include "ChildView.h"


#define ENTRY_SCAN_COUNT		_T("ScanCount")
#define ENTRY_MODEL_COUNT	_T("ModelCount")
#define ENTRY_SCAN4_CUSTOM	_T("Scan4Custom")
#define ENTRY_SCAN4_FACTOR	_T("Scan4Factor")
#define ENTRY_SCAN4_NET		_T("Scan4Net")
#define ENTRY_CON_MAX			_T("ConMax")
#define ENTRY_CON_MIN			_T("ConMin")
#define ENTRY_INTIME_MAX		_T("InTimeMax")
#define ENTRY_INTIME_MIN		_T("InTimeMin")
#define ENTRY_TRADES_MAX		_T("TradesMax")
#define ENTRY_TRADES_MIN		_T("TradesMin")
// CScanSettingsDlg dialog

IMPLEMENT_DYNAMIC(CScanSettingsDlg, CDialogEx)

CScanSettingsDlg::CScanSettingsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SCAN_SETTINGS_DLG, pParent)
	, m_pView{ pParent }
	, m_Scan_Count(0)
	, m_Model_Count(0)
	, m_Scan4_Custom(FALSE)
	, m_Scan4_Factor(FALSE)
	, m_Scan4_Net(FALSE)
	, m_Con_Max(0)
	, m_Con_Min(0)
	, m_InTime_Max(0)
	, m_InTime_Min(0)
	, m_Trades_Max(0)
	, m_Trades_Min(0)
{
	m_Scan_Count = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_SCAN_COUNT, 10);
	m_Scan4_Custom = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_SCAN4_CUSTOM, FALSE);
	m_Scan4_Factor = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_SCAN4_FACTOR, FALSE);
	m_Scan4_Net = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_SCAN4_NET, TRUE);
	m_Model_Count = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_MODEL_COUNT, 2);
	m_Con_Max = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_CON_MAX, 100);
	m_Con_Min = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_CON_MIN, 0);
	m_InTime_Max = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_INTIME_MAX, 100);
	m_InTime_Min = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_INTIME_MIN, 0);
	m_Trades_Max = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_TRADES_MAX, 100);
	m_Trades_Min = theApp.GetProfileInt(KEY_SETTINGS, ENTRY_TRADES_MIN, 0);
}

CScanSettingsDlg::~CScanSettingsDlg()
{
}

void CScanSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BEST_SCAN_COUNT, m_Scan_Count);
	DDX_Text(pDX, IDC_MODEL_COUNT, m_Model_Count);
	DDX_Check(pDX, IDC_SCAN4_CUSTOM, m_Scan4_Custom);
	DDX_Check(pDX, IDC_SCAN4_FACTOR, m_Scan4_Factor);
	DDX_Check(pDX, IDC_SCAN4_NET, m_Scan4_Net);
	DDX_Text(pDX, IDC_CON_MAX, m_Con_Max);
	DDV_MinMaxInt(pDX, m_Con_Max, 0, 100);
	DDX_Text(pDX, IDC_CON_MIN, m_Con_Min);
	DDV_MinMaxInt(pDX, m_Con_Min, 0, 100);
	DDX_Text(pDX, IDC_INTIME_MAX, m_InTime_Max);
	DDV_MinMaxInt(pDX, m_InTime_Max, 0, 100);
	DDX_Text(pDX, IDC_INTIME_MIN, m_InTime_Min);
	DDV_MinMaxInt(pDX, m_InTime_Min, 0, 100);
	DDX_Text(pDX, IDC_TRADES_MAX, m_Trades_Max);
	DDX_Text(pDX, IDC_TRADES_MIN, m_Trades_Min);
}


BEGIN_MESSAGE_MAP(CScanSettingsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_USE_MINIMUM_PROFIT, DefaultProc)
	ON_BN_CLICKED(IDC_USE_MINIMUM_TRADES, DefaultProc)
	ON_EN_CHANGE(IDC_MINIMUM_PROFIT, DefaultProc)
	ON_EN_CHANGE(IDC_MINIMUM_TRADES, DefaultProc)
	ON_BN_CLICKED(IDC_SCAN4_NET, &CScanSettingsDlg::OnBnClickedScan4Net)
	ON_BN_CLICKED(IDC_SCAN4_FACTOR, &CScanSettingsDlg::OnBnClickedScan4Factor)
	ON_BN_CLICKED(IDC_SCAN4_CUSTOM, &CScanSettingsDlg::OnBnClickedScan4Custom)
END_MESSAGE_MAP()


// CScanSettingsDlg message handlers

void CScanSettingsDlg::OnOK()
{
	if (UpdateData())
	{
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_SCAN_COUNT, m_Scan_Count);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_MODEL_COUNT, m_Model_Count);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_SCAN4_CUSTOM, m_Scan4_Custom);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_SCAN4_FACTOR, m_Scan4_Factor);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_SCAN4_NET, m_Scan4_Net);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_CON_MAX, m_Con_Max);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_CON_MIN, m_Con_Min);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_INTIME_MAX, m_InTime_Max);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_INTIME_MIN, m_InTime_Min);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_TRADES_MAX, m_Trades_Max);
		theApp.WriteProfileInt(KEY_SETTINGS, ENTRY_TRADES_MIN, m_Trades_Min);
	}

	CDialogEx::OnOK();
}

void CScanSettingsDlg::DefaultProc()
{
	if (UpdateData())
	{
		UpdateControls();
		UpdateData(FALSE);
	}
}

void CScanSettingsDlg::UpdateControls()
{
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

BOOL CScanSettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	UpdateData(FALSE);
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CScanSettingsDlg::OnBnClickedScan4Net()
{
	if (UpdateData())
		UpdateControls();
}

void CScanSettingsDlg::OnBnClickedScan4Factor()
{
	if (UpdateData())
		UpdateControls();
}

void CScanSettingsDlg::OnBnClickedScan4Custom()
{
	if (UpdateData())
		UpdateControls();
}
