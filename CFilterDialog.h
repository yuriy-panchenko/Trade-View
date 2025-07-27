#pragma once
#include "afxdialogex.h"


// CFilterDialog dialog

class CFilterDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CFilterDialog)

public:
	CFilterDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFilterDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILTER_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()

public:
	void UpdateInfo();
	void UpdateControls();

	BOOL m_UseMinProfit;
	BOOL m_UseMinTrades;
	double m_Min_Profit;
	int m_Min_Trades;
	CString m_Info_Text;
	BOOL m_Filter_Duplicates;
};
