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
	ON_COMMAND(ID_32775,&CImgProView::Hough)

	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgProView construction/destruction

CImgProView::CImgProView() {
	// TODO: add construction code here
	image = 0;
	outImg = 0;
	findex = 0;

	HoughImg = nullptr;

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
	memcpy(outImg, image, sizeof(BYTE) * w * h);
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
		memcpy(image, outImg, sizeof(BYTE) * w * h);
	}
}

void CImgProView::Binarize(BYTE* image, int w, int h, BYTE* outImg, int threshold) {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (image[i * width + j] > threshold) {
				outImg[i * width + j] == 255;
			}
			else {
				outImg[i * width + j] == 0;
			}
		}
	}
}

void CImgProView::Hough_Trans(BYTE* BMPBeforeTrans, int w, int h, BYTE* BMPAfterTrans) {
//	memcpy(BMPAfterTrans, BMPBeforeTrans, sizeof(BYTE)*w*h);
//	return;
//	char *OriginalBMP, ResultBMP;
//	unsigned char *BMPBeforeTrans, *BMPAfterTrans;

//	BITMAPFILEHEADER fh;
//	BITMAPINFOHEADER ih;
//	RGBQUAD Color[256];

	int Row, Col, pixel;
	//循环变量
	int i, j, k, v = 0;

	int iMaxDist;
	//变换域指针
	unsigned int* lpTransArea;

	maxValue maxValue1;
	maxValue maxValue2;


	//变换域的坐标
	int iDist;
	int iAngleNumber;
	int iMaxAngleNumber = 90;

//	FILE *f, *p;

	//打开文件失败
	/*
	if ((f = fopen(OriginalBMP, "rb")) == NULL) {
		printf("open %s error,please check\n");
		return;
	}

	//读取文件头
	fread(&fh, sizeof(BITMAPFILEHEADER), 1, f);
	if (fh.bfType != 'MB') {
		printf("This is not a BMP picture\n");
		return;
	}

	//读取文件信息头
	fread(&ih, sizeof(BITMAPINFOHEADER), 1, f);
*/
//	Row = ih.biHeight;
//	Col = ih.biWidth;
	//列数需为4的倍数
	Row = h;
	Col = w;
	Col = (Col + 3) / 4 * 4;
	iMaxDist = (int) sqrt(Row * Row + Col * Col);

	//为缓存图像以及变换域分配存储空间
	lpTransArea = (unsigned int*) calloc(iMaxAngleNumber * iMaxDist, sizeof(int));
	//BMPBeforeTrans = (unsigned char*) calloc(Row * Col, sizeof(unsigned char));
	//BMPAfterTrans = (unsigned char*) calloc(Row * Col, sizeof(unsigned char));

	//初始化变换域
	for (k = 0; k < iMaxAngleNumber * iMaxDist; k++) {
		lpTransArea[k] = 0;
	}

	//fseek(f, sizeof(RGBQUAD) * 256 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), 0);
	//fread(BMPBeforeTrans, sizeof(unsigned char), Row * Col, f);
	//Row - height
	//统计图像上的每一个黑点，将其量化数组储存在变换域
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			BMPBeforeTrans[i * Col + 315] = 255;
			pixel = BMPBeforeTrans[i * Col + j];

			//如果图像不是二值图，退出程序
			/*
			if (pixel != 0 && pixel != 255) {
				printf("this picture is not correct");
				return;
			}
*/
			if (pixel == 0) {
				for (iAngleNumber = 0; iAngleNumber < iMaxAngleNumber; iAngleNumber++) {
					iDist = (int) fabs(i * cos(iAngleNumber * 2 * pi / 180.0) + j * sin(iAngleNumber * 2 * pi / 180.0)); // p = xcosa+ysina
					lpTransArea[iDist * iMaxAngleNumber + iAngleNumber] = lpTransArea[iDist * iMaxAngleNumber + iAngleNumber] + 1; // lpTransArea ++;
				}
			}
		}
	}
	maxValue1.value = 0;
	maxValue2.value = 0;
	//查找出第一条直线
	//iDist p
	//iAngleNumber a
	for (iDist = 0; iDist < iMaxDist; iDist++) {
		for (iAngleNumber = 0; iAngleNumber < iMaxAngleNumber; iAngleNumber++) {
			if ((int) lpTransArea[iDist * iMaxAngleNumber + iAngleNumber] > maxValue1.value) {
				maxValue1.value = (int) lpTransArea[iDist * iMaxAngleNumber + iAngleNumber];
				maxValue1.Dist = iDist;
				maxValue1.AngleNumber = iAngleNumber;
			}
		}
	}
	/*
	printf(" maxValue1.value= %d\n", maxValue1.value);
	printf(" maxValue1.AngleNumber= %d\n", maxValue1.AngleNumber);
	printf(" maxValue1.Dist= %d \n", maxValue1.Dist);
*/

	//将缓冲图片底色置为黑色，将直线数据写入到缓冲图片
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			BMPAfterTrans[i * Col + j] = 0;
			iDist = (int) fabs(i * cos(maxValue1.AngleNumber * 2 * pi / 180.0) + j * sin(maxValue1.AngleNumber * 2 * pi / 180.0));
			if (iDist == maxValue1.Dist) {
				BMPAfterTrans[i * Col + j] = (unsigned char) 255;
			}
		}
	}

	//查找出第二条直线
	for (iDist = 0; iDist < iMaxDist; iDist++) {
		for (iAngleNumber = 0; iAngleNumber < iMaxAngleNumber; iAngleNumber++) {
			if ((int)*(lpTransArea + iDist * iMaxAngleNumber + iAngleNumber) > maxValue2.value) {
				if (iDist < maxValue1.Dist) {
					maxValue2.value = (int)*(lpTransArea + iDist * iMaxAngleNumber + iAngleNumber);
					maxValue2.Dist = iDist;
					maxValue2.AngleNumber = iAngleNumber;
				}
			}

		}
	}
	/*
	printf(" maxValue2.value= %d\n", maxValue2.value);
	printf(" maxValue2.AngleNumber= %d\n", maxValue2.AngleNumber);
	printf(" maxValue2.Dist= %d \n  ", maxValue2.Dist);
*/

	//将第二条直线输出到缓存图像
	/*
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			iDist = (int) fabs(i * cos(maxValue1.AngleNumber * 2 * pi / 180.0) + j * sin(maxValue1.AngleNumber * 2 * pi / 180.0));
			if (iDist == maxValue2.Dist)
				BMPAfterTrans[i * Col + j] = (unsigned char) 255;
		}
	}
*/

	//fclose(f);
	/*
	for (i = 0; i < 256; i++) {
		Color[i].rgbBlue = i;
		Color[i].rgbGreen = i;
		Color[i].rgbRed = i;
		Color[i].rgbReserved = 0;
	}
*/

	//写入缓存图像数据
	/*
	fwrite(&fh, sizeof(BITMAPFILEHEADER), 1, p);
	fwrite(&ih, sizeof(BITMAPINFOHEADER), 1, p);
	fwrite(Color, sizeof(RGBQUAD), 256, p);

	fwrite(BMPAfterTrans, sizeof(unsigned char), Row * Col, p);

	fclose(p);
*/
	//return;
}

void CImgProView::Sobel(BYTE* srcBmp, int width, int height,int type ,BYTE* outImg) {
	int i, j, l, x, y;
	// int px[6]={0};
	//int py[6]={0};
	int p1[4] = { 0 };
	//double result;
	byte* tempy;
	byte* tempx;
	tempx = (byte *) malloc(sizeof(byte) * width * width);
	if (tempx == NULL)
		exit(-1);
	tempy = (byte *) malloc(sizeof(byte) * width * width);
	if (tempy == NULL)
		exit(-1);
	for (i = 1; i < height - 1; i++) {
		for (j = 1; j < width - 1; j++)//由于使用3×3的模板，为防止越界，所以不处理最下边和最右边的两列像素
		{
			if (i == height - 1)
				x = 0, y = 0;
			else if (j == width - 1)
				x = 0, y = 0;
			else {
				p1[0] = (byte)*(srcBmp + i * width + j);
				p1[1] = (byte)*(srcBmp + i * width + j + 1);
				p1[2] = (byte)*(srcBmp + (i + 1) * width + j);
				p1[3] = (byte)*(srcBmp + (i + 1) * width + j + 1);

				x = abs(p1[0] - p1[2]); //sqrt(( p1[1] - p1[2] )*( p1[1] - p1[2] ));
				y = abs(p1[0] - p1[1]);
				if (x > 0)
					x = 255;
				else
					x = 0;
				if (y > 0)
					y = 255;
				else
					y = 0;
			}
			tempx[i * width + j] = (byte) x;
			tempy[i * width + j] = (byte) y;
		}
	}
	for (i = 0; i < width; i++) {
		tempx[i] = 0;
		tempy[i] = 0;
		tempx[width * (height - 1) + i] = 0;
		tempy[width * (height - 1) + i] = 0;
	}
	for (i = 0; i < height; i++) {
		tempx[i * width] = 0;
		tempy[i * width] = 0;
		tempx[i * width + width - 1] = 0;
		tempy[i * width + width - 1] = 0;
	}
	/*
	if (type == 0)
		memcpy(srcBmp, tempx, sizeof(byte) * width * height);
	if (type == 1)
		memcpy(srcBmp, tempy, sizeof(byte) * width * height);
	if (type == 2) {
		for (i = 0; i < height; i++)
			for (j = 0; j < width; j++) {
				l = tempx[i * width + j] + tempy[i * height + j];
				if (l > 255)
					l = 255;
				tempx[i * width + j] = l;
			}
		//memcpy(srcBmp, tempx, sizeof(byte) * width * height);
	}
*/
	outImg = tempy;
	/*
	free(tempx);
	free(tempy);
	tempx = NULL;
	tempy = NULL;
*/

}

void CImgProView::Dilation(BYTE* image, int width, int height, int type, int num) {
	int dwWidth = width;
	int dwHeight = height;

	int i = 0;
	int j = 0;
	//int n=0;

	BYTE g = 0;

	//double avg=0;
	BYTE* temp;
	int k = 0;


	temp = (BYTE*) malloc(dwHeight * dwWidth * sizeof(BYTE));
	memcpy(temp, image, dwHeight * dwWidth * sizeof(byte));


	memset(temp, 0, dwWidth * dwHeight * sizeof(BYTE));


	if (type == 0) {
		//水平方向
		for (i = 0; i < dwHeight; i++) {
			for (j = (num - 1) / 2; j < dwWidth - (num - 1) / 2; j++) {
				for (k = -(num - 1) / 2; k <= (num - 1) / 2; k++) {
					g = *(image + dwWidth * i + j + k);
					if (g == 255) {
						*(temp + dwWidth * i + j) = 255;
						break;
					}
				}
			}
		}
	}
	else {
		//垂直方向
		for (i = (num - 1) / 2; i < dwHeight - (num - 1) / 2; i++) {
			for (j = 0; j < dwWidth; j++) {
				for (k = -(num - 1) / 2; k <= (num - 1) / 2; k++) {
					g = *(image + dwWidth * (i + k) + j);
					if (g == 255) {
						*(temp + dwWidth * i + j) = 255;
						break;
					}
				}
			}
		}
	}
	memcpy(image, temp, sizeof(byte) * width * height);
	free(temp);
	temp = NULL;
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
	if (flag_hough) {
		BYTE gray;
		if (HoughImg) {
			for (i = 0; i < hough_height; i++) {
				for (j = 0; j < hough_width; j++) {
					gray = HoughImg[i * hough_width + j];
					pDC->SetPixel(j + hough_width, i + height, RGB(gray, gray, gray));
				}
			}

		}

	}
	if (flag_kuang) {
		CPen NewPen(PS_SOLID, 1, RGB(255, 0, 0));
		pDC->SelectObject(&NewPen);
		BYTE r, g, b;
		for (i = y1; i < y2; i++)
			for (j = 3 * x1; j < 3 * x2; j = j + 3) {
				b = rgbimg[i * 3 * width + j];
				g = rgbimg[i * 3 * width + j + 1];
				r = rgbimg[i * 3 * width + j + 2];
				pDC->SetPixelV(j / 3 - x1, i - y1 + height, RGB(r, g, b));
			}
		pDC->MoveTo(x1, y1);
		pDC->LineTo(x2, y1);
		pDC->LineTo(x2, y2);
		pDC->LineTo(x1, y2);
		pDC->LineTo(x1, y1);
		x1 += width;
		x2 += width;
		pDC->MoveTo(x1, y1);
		pDC->LineTo(x2, y1);
		pDC->LineTo(x2, y2);
		pDC->LineTo(x1, y2);
		pDC->LineTo(x1, y1);
		x1 -= width;
		x2 -= width;
		if (0) {
			CString s2;
			s2.Format("x1: %d,y1: %d\nx2: %d,y2: %d", x1, y1, x2, y2);
			AfxMessageBox(s2, MB_OK, 0);
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
	flag_kuang = 0;
	flag_hough = 0;

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
	// TODO: Color() 
	BYTE r, g, b;
	int i, j;
	double h = 0, s, v;
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
				s = 0;
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
	HsvImg = new BYTE[width * height];
	memcpy(HsvImg, huiimg, width * height);
	Erosion(huiimg, width, height, outImg);
	Dilation(huiimg, width, height, outImg2);
	Erosion(huiimg, width, height, outImg);
	Dilation(huiimg, width, height, outImg2);
	Erosion(huiimg, width, height, outImg);
	Dilation(huiimg, width, height, outImg2);
	flag = 1;
	OnInitialUpdate();
}

void CImgProView::Extract() {
	int margin = 3;
	int l_length = 15; // level_length (for y)
	int v_length = 8; // vertical_length (for x)
	int i, j;
	int flagb1;
	int x1_max = width, y1_max = height, x2_max = 0, y2_max = 0;
	//255 White 0 Black
	flag_kuang = 1;
	if (!flag) {
		Color();
	}
	if (outImg2) {
		for (i = 0; i < height; i++) {
			flagb1 = 0;
			for (j = 1; j < width; j++) {
				if (outImg2[i * width + j - l_length] == 0 && outImg2[i * width + j - (l_length - 1)] == 255 && outImg2[i * width + j] == 255) {
					y1 = i - margin;
					flagb1 = 1;
					j = width;
				}
			}
			if (flagb1 != 0) {
				//i = height;
				//break;
				if (y1 < y1_max) {
					y1_max = y1;
				}
			}
		}
		for (i = height; i >= 0; i--) {
			flagb1 = 0;
			for (j = 1; j < width; j++) {
				if (outImg2[i * width + j - l_length] == 255 && outImg2[i * width + j] == 255 && outImg2[i * width + j + 1] == 0) {
					y2 = i + margin;
					flagb1 = 1;
					//j = width;
					break;
				}
			}
			if (flagb1 != 0) {
				//i = -1;
				if (y2 > y2_max) {
					y2_max = y2;
				}
			}
		}
		for (j = 1; j < width; j++) {
			flagb1 = 0;
			for (i = 0; i < height; i++) {
				if (outImg2[(i - v_length) * width + j] == 0 && outImg2[(i - v_length + 1) * width + j] == 255 && outImg2[i * width + j] == 255) {
					x1 = j - margin;// 3 为余量
					flagb1 = 1;
					i = height;
				}
			}
			if (flagb1 != 0) {
				//j = width;
				if (x1 < x1_max) {
					x1_max = x1;
				}
			}
		}
		for (j = width - 2; j >= 0; j--) {
			flagb1 = 0;
			for (i = 0; i < height; i++) {
				if (outImg2[(i - v_length) * width + j] == 255 && outImg2[(i - 1) * width + j] == 255 && outImg2[(i * width + j)] == 0) {
					x2 = j + margin;
					flagb1 = 1;
					i = height;
				}
			}
			if (flagb1 != 0) {
				//j = -1;
				if (x2 > x2_max) {
					x2_max = x2;
				}
			}
		}
	}
	x2 = x2_max;
	x1 = x1_max;
	y1 = y1_max;
	y2 = y2_max;
	hough_width = x2 - x1 + 1;
	hough_height = y2 - y1 + 1;
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

void CImgProView::Hough() {
	if (!flag_kuang) {
		Extract();
	}
	HoughImg = new BYTE[hough_width * hough_height];
	BYTE* temp = new BYTE[hough_width * hough_height];
	for (int i = 0; i < hough_height; i++) {
		for (int j = 0; j < hough_width; j++) {
			HoughImg[i * hough_width + j] = HsvImg[(i + y1) * width + j + x1];
		}
	}
	//Hough_Trans(HoughImg, hough_width, hough_height, temp);
	//Dilation(HoughImg, hough_width, hough_height, temp);
	Dilation(HoughImg, hough_width, hough_height, 1, 5);
	Sobel(HoughImg, hough_width, hough_height, 0, temp);
	memcpy(HoughImg, temp, sizeof(BYTE)*hough_width*hough_height);


	flag_hough = 1;
	OnInitialUpdate();
}
