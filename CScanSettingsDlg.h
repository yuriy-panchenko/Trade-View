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

	std::vector<TradeFile const*> Filter(std::vector<TradeFile> const& files)const;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCAN_SETTINGS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	afx_msg void DefaultProc();
	afx_msg void DefaultEditProc();
	afx_msg void OnBnClickedScan4Net();
	afx_msg void OnBnClickedScan4Factor();
	afx_msg void OnBnClickedScan4Custom();
	DECLARE_MESSAGE_MAP()

public:
	void UpdateControls();
	void UpdateInfo();
	static CString InsertApostrofie(size_t val);

	int m_Scan_Count;
	double m_Min_Profit;
	int m_Min_Trades;
	int m_Model_Count;
	BOOL m_Scan4_Custom;
	BOOL m_Scan4_Factor;
	BOOL m_Scan4_Net;
	BOOL m_UseMinProfit;
	BOOL m_UseMinTrades;
	CString m_Info_Text;
	CWnd* m_pView;
	size_t m_uModelCount;
};
