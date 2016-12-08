// ImgProView.h : interface of the CImgProView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMGPROVIEW_H__CA52376A_801B_4E87_AA91_71B205388883__INCLUDED_)
#define AFX_IMGPROVIEW_H__CA52376A_801B_4E87_AA91_71B205388883__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "math.h"
typedef unsigned char BYTE;

struct HSV {
	float H;
	float S;
	int V;
};

struct RGB {
	byte bitb;
	byte bitg;
	byte bitr;
	byte re;
};

struct Bmp1 {
	DWORD width;
	DWORD height;
	byte strc[67][800];
	BYTE* image;
	int p1[15];//xl
	int p2[15];//xr
	int p3[15];//yu
	int p4[15];//yd
	int up;
	int down;
	byte strr[7][2500];
	byte string[7];
	float ang;//��б�Ƕ�
	int x1, x2, y1, y2;
};

struct BMP_img {
	WORD bfType;
	DWORD bfSize;
	DWORD bfReserved;
	DWORD bfOffBits;
	DWORD biSize;
	DWORD biWidth;
	DWORD biHeight;
	WORD biPlanes;
	WORD biBitCount; /* 8bit 24bit; */
	DWORD biCompression;
	DWORD biSizeImage;
	DWORD biXPelsPerMeter;
	DWORD biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
	BYTE* image;
	byte* header_info;
	DWORD lineBytes;
	int x1;
	int x2;
	int y1;
	int y2;
};

class CImgProView : public CView {
protected: // create from serialization only
	CImgProView();
	DECLARE_DYNCREATE(CImgProView)

	// Attributes
public:
	CImgProDoc* GetDocument();

	// Operations
public:

	BYTE* image;
	BYTE* outImg;
	BYTE* outImg2;
	BYTE* rgbimg;
	BYTE* huiimg;
	BYTE* HsvImg;
	BYTE* HoughImg;

	int width, height;
	int hough_width, hough_height;

	void readImg(int);
	int findex;
	int fnum;
	char directory[120];
	char* fnames;
	int bmpflag, rawflag;
	int flag;

	int flag_kuang;
	int flag_hough;

	//int x1, x2, y1, y2;
	BMP_img bmp_img;
	Bmp1 bimg1;

	void Erosion(BYTE* image, int w, int h, BYTE* outImg);
	void Dilation(BYTE* image, int w, int h, BYTE* outImg);
	void Binarize(BYTE* image, int w, int h, BYTE* outImg, int threshold);
	void Hough_Trans(BYTE* image, int w, int h, BYTE* outImg);
	void Sobel(BYTE* image, int w, int h, int type, BYTE* outImg);

	void Dilation(BYTE* image, int width, int height, int type, int num);
	void sob(byte* temp1, int width, int height, int type);
	void Hough1(Bmp1* img);
	int hough(byte* srcBmp, int width, int height);
	void Rotate(Bmp1* img1);
	byte* Rotate_RGB(byte* image, float iRotateAngle, int width, int height, int* lwidth, int* lheight);
	void hsvzation(BYTE* image, HSV* hsv, int width, int height);
	byte* myMalloc(int num, const byte* bmp, int type);
	void edgesob8(byte* image, int width, int height);
	void location(byte* image, int width, int height, int yuzhi, int* HL, int* HH, int* VL, int* VH);
	void CutBmp(BMP_img img, Bmp1* img1, int HL, int HH, int VL, int VH);
	void CutBmp1(Bmp1* img1, int HL, int HH, int VL, int VH);
	void read_img(FILE* infile,BMP_img* img);
	void display_img(Bmp1& img1,CDC* pDC);
	void display_img(BMP_img& img, CDC* pDC);
	void location(byte* outImg2, int width, int height, int& x1, int& x2, int &y1, int& y);



	// add image processing function 


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImgProView)
public:
	virtual void OnDraw(CDC* pDC); // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CImgProView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CImgProView)
	afx_msg void OnFileOpen();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFileSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void Color();
	afx_msg void Extract();
	afx_msg void Split();
	afx_msg void Recognize();
	afx_msg void Hough();
};

typedef struct {
	int value;
	int Dist;
	int AngleNumber;
} maxValue;

#define pi 3.1415926
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef float FLOAT;
typedef unsigned char byte;
#define RADIAN(angle) ((angle)*3.14/180.0)


#ifndef _DEBUG  // debug version in ImgProView.cpp
inline CImgProDoc* CImgProView::GetDocument()
   { return (CImgProDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMGPROVIEW_H__CA52376A_801B_4E87_AA91_71B205388883__INCLUDED_)
