// ImgProDoc.cpp : implementation of the CImgProDoc class
//

#include "stdafx.h"
#include "ImgPro.h"

#include "ImgProDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImgProDoc

IMPLEMENT_DYNCREATE(CImgProDoc, CDocument)

BEGIN_MESSAGE_MAP(CImgProDoc, CDocument)
	//{{AFX_MSG_MAP(CImgProDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgProDoc construction/destruction

CImgProDoc::CImgProDoc()
{
	// TODO: add one-time construction code here

}

CImgProDoc::~CImgProDoc()
{
}

BOOL CImgProDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CImgProDoc serialization

void CImgProDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CImgProDoc diagnostics

#ifdef _DEBUG
void CImgProDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImgProDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImgProDoc commands
