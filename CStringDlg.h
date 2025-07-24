#pragma once
#include "afxdialogex.h"


// CStringDlg dialog

class CStringDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStringDlg)

public:
	CStringDlg(CString title=_T("String Dialog"), CWnd* pParent = nullptr);   // standard constructor
	virtual ~CStringDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STRING_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_Text;

private:
	CString m_TitleText;
public:
	virtual BOOL OnInitDialog();
};
