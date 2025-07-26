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
	inline std::vector<TradeFile> const& GetFiles()const { return m_Files; }

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
	afx_msg LRESULT OnScanFinished(WPARAM, LPARAM);
	afx_msg LRESULT OnBetterResult(WPARAM, LPARAM);
	afx_msg void OnRemoveSelected();
	afx_msg void OnUpdateRemoveSelected(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnSwapTables();
	afx_msg void OnUpdateSwapTables(CCmdUI* pCmdUI);
	afx_msg void OnShowInfo();
	afx_msg void OnUpdateShowInfo(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnUpdateIdsListItemCount(CCmdUI* pCmdUI);
	afx_msg void OnClear();
	afx_msg void OnUpdateClear(CCmdUI* pCmdUI);
	afx_msg void OnFilterModels();
	afx_msg void OnUpdateFilterModels(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

private:
	void LoadFolder(fs::path const&);
	void AddListItem(TRADES_STATISTIC const&);
	void PrepareList(BOOL const bList4Scan);
	void UpdateScanningList();
	void UpdateListScanResults(size_t const index);
	void AutosizeColumns();
	void UpdateLayout();
	void LoadFiles();
	void LoadBest();
	void LoadList(BOOL const isScanning);
	void LoadList();
	std::set<int> GetSelectedIndexes();
	void DrawInfo(CDC&,CRect const&);
	void Draw(CDC&, CRect const&);

	CSortListCtrl m_List;
	CRect m_Canvas;
	std::vector<TradeFile> m_Files;
	Scan::BestModels m_Best;
	BOOL m_isScanningMode,m_doShowInfo;
	Scan::Models m_Scan;
	CScanSettingsDlg m_SetsDlg;
	CFont m_fontScanning;
	int m_iListWidth;
	CFont m_InfoFont;// , m_InfoFontBold;
};

