#pragma once
#include "afxdialogex.h"


// CScanSettingsDlg dialog

class CScanSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScanSettingsDlg)

public:
	CScanSettingsDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CScanSettingsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCAN_SETTINGS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_Scan_Count;
	double m_Min_Profit;
	int m_Min_Trades;
	int m_Model_Count;
	BOOL m_Scan4_Custom;
	BOOL m_Scan4_Factor;
	BOOL m_Scan4_Net;
	BOOL m_UseMinProfit;
	BOOL m_UseMinTrades;
	virtual void OnOK();
};
