// ImgPro.h : main header file for the IMGPRO application
//

#if !defined(AFX_IMGPRO_H__B48E5738_3B20_4C4C_8A61_8A8247F45F39__INCLUDED_)
#define AFX_IMGPRO_H__B48E5738_3B20_4C4C_8A61_8A8247F45F39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CImgProApp:
// See ImgPro.cpp for the implementation of this class
//

class CImgProApp : public CWinApp
{
public:
	CImgProApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImgProApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CImgProApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMGPRO_H__B48E5738_3B20_4C4C_8A61_8A8247F45F39__INCLUDED_)
