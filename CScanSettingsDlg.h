#pragma once
#include "afxdialogex.h"
#include "Trades.h"


// CScanSettingsDlg dialog

class CScanSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScanSettingsDlg)

public:
	CScanSettingsDlg(CWnd* pParent);   // standard constructor
	virtual ~CScanSettingsDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCAN_SETTINGS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	afx_msg void DefaultProc();
	afx_msg void OnBnClickedScan4Net();
	afx_msg void OnBnClickedScan4Factor();
	afx_msg void OnBnClickedScan4Custom();
	DECLARE_MESSAGE_MAP()

public:
	void UpdateControls();

	int m_Scan_Count;
	int m_Model_Count;
	BOOL m_Scan4_Custom;
	BOOL m_Scan4_Factor;
	BOOL m_Scan4_Net;
	CWnd* m_pView;
	int m_Con_Max;
	int m_Con_Min;
	int m_InTime_Max;
	int m_InTime_Min;
	int m_Trades_Max;
	int m_Trades_Min;
};
