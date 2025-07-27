
// Trade View.h : main header file for the Trade View application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CTradeViewApp:
// See Trade View.cpp for the implementation of this class
//

class CTradeViewApp : public CWinApp
{
public:
	CTradeViewApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

CString InsertApostrofie(size_t val);

extern CTradeViewApp theApp;
