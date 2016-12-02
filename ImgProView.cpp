// ImgProView.cpp : implementation of the CImgProView class
//
//Created by Chingyi Chang 20161203

#include "stdafx.h"
#include "ImgPro.h"

#include "ImgProDoc.h"
#include "ImgProView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImgProView

IMPLEMENT_DYNCREATE(CImgProView, CView)

BEGIN_MESSAGE_MAP(CImgProView, CView)
	//{{AFX_MSG_MAP(CImgProView)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_32771, &CImgProView::Color)
	ON_COMMAND(ID_32772,&CImgProView::Extract)
	ON_COMMAND(ID_32773,&CImgProView::Split)
	ON_COMMAND(ID_32774,&CImgProView::Recognize)

	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgProView construction/destruction

CImgProView::CImgProView() {
	// TODO: add construction code here
	image = 0;
	outImg = 0;
	findex = 0;

}

CImgProView::~CImgProView() {
	if (image)
		delete image;
	if (outImg)
		delete outImg;
}

BOOL CImgProView::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CImgProView drawing

void CImgProView::Erosion(BYTE* image, int w, int h, BYTE* outImg) {
	int rept;
	memcpy(outImg, image, sizeof(BYTE) * width * height);
	int i, j;
	int m, n;
	BYTE flag;
	for (rept = 0; rept < 1; rept++) {
		for (i = 1; i < h - 1; i++) {
			for (j = 1; j < w - 1; j++) {
				if (image[i * w + j] == 255) {
					flag = 0;
					for (m = -1; m < 2; m++) {
						for (n = -1; n < 2; n++) {
							if (image[(i + m) * w + j + n] == 0) {
								flag++;
								break;
							}
						}
					}

					if (flag > 1)
						outImg[i * w + j] = 0;
				}

			}
		}
		memcpy(image, outImg, sizeof(BYTE) * width * height);
	}
}

void CImgProView::Dilation(BYTE* image, int w, int h, BYTE* outImg) {
	int rept;
	memcpy(outImg, image, sizeof(BYTE) * width * height);

	int i, j;
	int m, n;
	BYTE flag;
	for (rept = 0; rept < 100; rept++) {
		for (i = 1; i < h - 1; i++) {
			for (j = 1; j < w - 1; j++) {
				if (image[i * w + j] == 0) {
					flag = 0;
					for (m = -1; m < 2; m++) {
						for (n = -1; n < 2; n++) {
							if (image[(i + m) * w + j + n] == 255) {
								flag++;

							}
						}


					}
					if (flag > 3)
						outImg[i * w + j] = 255;
				}
			}


		}
		memcpy(image, outImg, sizeof(BYTE) * width * height);
	}

}

void CImgProView::OnDraw(CDC* pDC) {
	CImgProDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	if (image == 0)
		return;

	int i, j;
	if (rawflag == 1) {
		BYTE gray;
		for (i = 0; i < height; i++) {
			for (j = 0; j < width; j++) {
				gray = image[i * width + j];
				pDC->SetPixel(j, i, RGB(gray, gray, gray));
			}
		}
	}
	//////   show outImg here //////////////////////

	if (bmpflag == 1) {
		BYTE r, g, b;
		for (i = 0; i < height; i++)
			for (j = 0; j < 3 * width; j = j + 3) {
				b = rgbimg[i * 3 * width + j];
				g = rgbimg[i * 3 * width + j + 1];
				r = rgbimg[i * 3 * width + j + 2];
				pDC->SetPixelV(j / 3, i, RGB(r, g, b));
			}
	}
	if (flag == 1) {
		BYTE gray;
		if (huiimg) {
			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					gray = huiimg[i * width + j];
					pDC->SetPixel(j + width, i, RGB(gray, gray, gray));
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CImgProView printing

BOOL CImgProView::OnPreparePrinting(CPrintInfo* pInfo) {
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CImgProView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
	// TODO: add extra initialization before printing
}

void CImgProView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CImgProView diagnostics

#ifdef _DEBUG
void CImgProView::AssertValid() const {
	CView::AssertValid();
}

void CImgProView::Dump(CDumpContext& dc) const {
	CView::Dump(dc);
}

CImgProDoc* CImgProView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImgProDoc)));
	return (CImgProDoc*) m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImgProView message handlers

void CImgProView::OnFileOpen() {
	// TODO: Add your command handler code here
	CFileDialog MyFDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, NULL, NULL);

	MyFDlg.m_ofn.lpstrFilter = "(*.lst;*.raw;*.bmp)\0*.lst;*.raw;*.bmp\0";
	MyFDlg.m_ofn.lpstrInitialDir = "C:\\";

	flag = 0;
	bmpflag = 0;
	rawflag = 0;

	if (MyFDlg.DoModal() == IDOK) {
		CString SFName;

		SFName = MyFDlg.GetPathName(); //full name with path

		char* fnstr;
		fnstr = SFName.GetBuffer(4); //read the name from string 

		FILE* fpLst;
		int n;

		int len;
		len = strlen(fnstr);
		if (!strcmp(fnstr + len - 3, "raw")) //single raw image
		{
			rawflag = 1;
			fnum = 0;
			char* ptr;
			ptr = fnstr + len - 3;
			while (*ptr != '\\')
				ptr--;
			*ptr = 0;
			strcpy(directory, fnstr);//Directory

			fnames = new char[500];
			strcpy(fnames, ptr + 1); //image name
		}
		if (!strcmp(fnstr + len - 3, "lst")) //list file
		{
			bmpflag = 1;
			fpLst = fopen(fnstr, "rb");
			fscanf(fpLst, "%3d", &fnum);
			fscanf(fpLst, "%s", directory);//directory

			fnames = new char[fnum * 100];
			for (n = 0; n < fnum; n++)
				fscanf(fpLst, "%s", fnames + n * 100);// image names

			fclose(fpLst);
		}
		if (!strcmp(fnstr + len - 3, "bmp")) {
			bmpflag = 1;
			fnum = 0;
			char* ptr;
			ptr = fnstr + len - 3;
			while (*ptr != '\\')
				ptr--;
			*ptr = 0;
			strcpy(directory, fnstr);//Directory

			fnames = new char[500];
			strcpy(fnames, ptr + 1); //image name

			findex = 0;
		}
		readImg(findex);
	}
}

void CImgProView::readImg(int findex) {

	char fullName[120];
	sprintf(fullName, "%s\\%s", directory, fnames + findex * 100);

	FILE* fpImg;
	fpImg = fopen(fullName, "rb");
	if (fpImg == 0) {
		AfxMessageBox("Cannot open the list file", MB_OK, 0);
		//TODO: 20161130 AfxMessageBox
		return;
	}

	CString sFTitle;
	sFTitle.Format("%s", fnames + findex * 100);

	CImgProDoc* pDoc;
	pDoc = GetDocument();
	pDoc->SetTitle(sFTitle);

	int bicount;
	BITMAPFILEHEADER bmpFHeader;
	BITMAPINFOHEADER bmiHeader;

	fread(&bmpFHeader, sizeof(BITMAPFILEHEADER), 1, fpImg);
	fread(&bmiHeader, sizeof(BITMAPINFOHEADER), 1, fpImg);

	width = bmiHeader.biWidth;
	height = bmiHeader.biHeight;
	bicount = bmiHeader.biBitCount;


	BYTE* rowBuff;
	int rowLen;

	rowLen = ((width * bicount) / 8 + 3) / 4 * 4;
	rowBuff = new BYTE[rowLen];

	if (bicount == 8) {
		RGBQUAD bmiColors[256];
		fread(bmiColors, sizeof(RGBQUAD), 256, fpImg);
		image = new BYTE[width * height];
		rgbimg = new BYTE[3 * width * height];
		for (int i = height - 1; i >= 0; i--) {
			fread(rowBuff, 1, rowLen, fpImg);
			memcpy(image + i * width, rowBuff, width);
		}
		for (int i = 0; i < height; i++)
			for (int j = 0; j < 3 * width; j = j + 3) {
				rgbimg[i * 3 * width + j] = bmiColors[image[i * width + j / 3]].rgbBlue;
				rgbimg[i * 3 * width + j + 1] = bmiColors[image[i * width + j / 3]].rgbGreen;
				rgbimg[i * 3 * width + j + 2] = bmiColors[image[i * width + j / 3]].rgbRed;
			}
	}
	if (bicount == 24) {
		image = new BYTE[width * height];
		rgbimg = new BYTE[rowLen * height];
		for (int i = height - 1; i >= 0; i--) {
			fread(rowBuff, 1, rowLen, fpImg);
			memcpy(rgbimg + i * 3 * width, rowBuff, 3 * width);
		}
	}
	delete rowBuff;

	fclose(fpImg);

	////////////////////////////////////////////////////////////

	OnInitialUpdate();
}

void CImgProView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	// TODO: Add your message handler code here and/or call default
	if (nChar == VK_NEXT) {
		if (findex < fnum) {
			findex++;
			readImg(findex);
			if (huiimg != NULL) {
				huiimg = NULL;
			}
		}
	}
	if (nChar == VK_PRIOR) {
		if (findex > 0) {
			findex--;
			readImg(findex);
			if (huiimg != NULL) {
				huiimg = NULL;
			}
		}
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CImgProView::OnFileSave() {
	// TODO: Add your command handler code here
	char str[40], *ptr;
	sprintf(str, fnames + findex * 40);
	ptr = str;
	while (*ptr != '.')
		ptr++;
	*ptr = 0;

	char fname[80];
	sprintf(fname, "D:\\out%s.raw", str);

	FILE* fpOut;
	fpOut = fopen(fname, "wb");

	fwrite(&width, 4, 1, fpOut);
	fwrite(&height, 4, 1, fpOut);
	fwrite(outImg, 1, width * height, fpOut);

	fclose(fpOut);

}


void CImgProView::Color() {
	// TODO: �ڴ���������������
	BYTE r, g, b;
	int i, j;
	double h, s, v;
	BYTE max, min, delta;
	int RGB_BYTEs = 3 * width;
	huiimg = new BYTE[width * height];
	if (image == 0) {
		AfxMessageBox("No Images Opened\nOpen a Image First", MB_OK, 0);
		return;
	}
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j = j++) {
			b = rgbimg[i * 3 * width + j * 3];
			g = rgbimg[i * 3 * width + j * 3 + 1];
			r = rgbimg[i * 3 * width + j * 3 + 2];
			min = min(min(b, g), r);
			max = max(max(b, g), r);
			delta = max - min;
			if (max == min) {
				h = 0;
			}
			if (max == r) {
				if (g >= b) {
					h = (double) 60 * (g - b) / delta;
				}
				else {
					h = (double) 60 * (g - b) / delta + 360;
				}
			}
			else if (max == g) {
				h = (double) 60 * (b - r) / delta + 120;
			}
			else if (max == b) {
				h = (double) 60 * (r - g) / delta + 240;
			}
			v = max;
			if (v == 0) {
				s == 0;
			}
			else {
				s = (double) delta / max;
			}
			if (h >= 190 && h <= 245 && s >= 0.55) {
				huiimg[i * width + j] = 255;
			}
			else {
				huiimg[i * width + j] = 0;
			}
		}
	outImg = new BYTE[width * height];
	outImg2 = new BYTE[width * height];
	Erosion(huiimg, width, height, outImg);
	Dilation(huiimg, width, height, outImg2);

	flag = 1;
	OnInitialUpdate();
}

void CImgProView::Extract() {
	AfxMessageBox("Hello World!!!", MB_OK, 0);
	OnInitialUpdate();
}

void CImgProView::Split() {
	AfxMessageBox("Split!!!", MB_OK, 0);
	OnInitialUpdate();
}

void CImgProView::Recognize() {
	AfxMessageBox("Recognize!!!", MB_OK, 0);
	OnInitialUpdate();
}
