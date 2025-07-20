// ChildView.h : interface of the CChildView class
//
#pragma once
#include "ScanDefines.h"
#include "SortListCtrl.h"
#include "CScanSettingsDlg.h"
#include "Models.h"

// CChildView window

class CChildView : public CWnd
{
public:
	enum class Message :UINT
	{
		WM_SCAN_FINISHED = WM_USER + 0x0001,
		WM_BETTER_RESULT,
	};
	// Construction
public:
	CChildView();

	// Attributes
public:

	// Operations
public:

	// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnAddFolder();
	afx_msg void OnUpdateAddFolder(CCmdUI* pCmdUI);
	afx_msg void OnScanTrades();
	afx_msg void OnUpdateScanTrades(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnListItemChanged(NMHDR*, LRESULT*);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnScanFinished(WPARAM,LPARAM);
	afx_msg LRESULT OnBetterResult(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()

private:
	void LoadFolder(fs::path const&);
	void AddListItem(TRADES_STATISTIC const&);
	void PrepareList(BOOL const bList4Scan);
	void UpdateScanningList();
	void UpdateListScanResults(size_t const index);

	CSortListCtrl m_List;
	CRect m_Canvas;
	std::vector<TradeFile> m_Files;
	Scan::BestModels m_Best;
	BOOL m_isScanningMode;
	Scan::Models m_Scan;
	CScanSettingsDlg m_SetsDlg;
	CFont m_fontScanning;
};

