// ImgProDoc.h : interface of the CImgProDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMGPRODOC_H__A01A0049_AB88_47FF_A338_D5F31317F706__INCLUDED_)
#define AFX_IMGPRODOC_H__A01A0049_AB88_47FF_A338_D5F31317F706__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CImgProDoc : public CDocument
{
protected: // create from serialization only
	CImgProDoc();
	DECLARE_DYNCREATE(CImgProDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImgProDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImgProDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CImgProDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMGPRODOC_H__A01A0049_AB88_47FF_A338_D5F31317F706__INCLUDED_)
