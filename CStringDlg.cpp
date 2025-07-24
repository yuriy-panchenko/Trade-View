// CStringDlg.cpp : implementation file
//

#include "pch.h"
#include "Trade View.h"
#include "afxdialogex.h"
#include "CStringDlg.h"


// CStringDlg dialog

IMPLEMENT_DYNAMIC(CStringDlg, CDialogEx)

CStringDlg::CStringDlg(CString title, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STRING_DLG, pParent)
	, m_Text(_T(""))
	, m_TitleText(title)
{

}

CStringDlg::~CStringDlg()
{
}

void CStringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT, m_Text);
}


BEGIN_MESSAGE_MAP(CStringDlg, CDialogEx)
END_MESSAGE_MAP()


// CStringDlg message handlers

BOOL CStringDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(m_TitleText);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
