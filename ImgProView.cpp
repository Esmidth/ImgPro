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
	ON_COMMAND(ID_32772, &CImgProView::Extract)
	ON_COMMAND(ID_32773, &CImgProView::Split)
	ON_COMMAND(ID_32774, &CImgProView::Recognize)
	ON_COMMAND(ID_32775, &CImgProView::Hough)

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
	int width = w;
	int height = h;
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
				outImg[i * width + j] = 255;
			}
			else {
				outImg[i * width + j] = 0;
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

void CImgProView::Sobel(BYTE* srcBmp, int width, int height, int type, BYTE* outImg) {
	int i, j, l, x, y;
	// int px[6]={0};
	//int py[6]={0};
	int p1[4] = {0};
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
				x = 0 , y = 0;
			else if (j == width - 1)
				x = 0 , y = 0;
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

void CImgProView::sob(byte* srcBmp, int width, int height, int type) {
	int i, j, l, x, y;
	// int px[6]={0};
	//int py[6]={0};
	int p1[4] = {0};
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
				x = 0 , y = 0;
			else if (j == width - 1)
				x = 0 , y = 0;
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
		memcpy(srcBmp, tempx, sizeof(byte) * width * height);
	}
	free(tempx);
	free(tempy);
	tempx = NULL;
	tempy = NULL;
}

void CImgProView::Hough1(Bmp1* img) {
	int i, j, n;
	float ang;
	byte* temp1;
	//int temp[2000]={0};
	HSV* hsv = (HSV *) malloc(sizeof(struct HSV) * img->width * img->height);
	hsvzation(img->image, hsv, img->width, img->height);
	temp1 = (byte *) malloc(sizeof(byte) * img->height * img->width);


	for (i = 0; i < img->height; i++) {
		for (j = 0 , n = 0; j < img->width; n += 3 , j++) {
			if ((hsv[i * img->width + j].H < 220.0) && (hsv[i * img->width + j].H > 180.0) && (hsv[i * img->width + j].V < 250) && (hsv[i * img->width + j].S > 0.6)) {
				temp1[i * img->width + j] = 255;
			}
			else {
				temp1[i * img->width + j] = 0;
			}
		}
	}
	Dilation(temp1, img->width, img->height, 1, 5);
	sob(temp1, img->width, img->height, 0);
	ang = hough(temp1, img->width, img->height);
	if ((ang <= 8) && (ang >= 1))
		ang = ang - 1;
	if (ang > 90) {
		ang = ang - 180;
	}
	img->ang = ang;
}

int CImgProView::hough(byte* srcBmp, int width, int height) {
	int kmax = 0;
	int pmax = 0;
	int yuzhi = 0;
	int i, j, k, n, p = (int) (height * cos(pi) + width * sin(pi));
	int mp = (int) (sqrt(width * width + height * height) + 1);
	int ma = 180;//180
	//int ap;
	int npp[180][1000];
	for (i = 0; i < 180; i++)
		for (j = 0; j < 1000; j++)
			npp[i][j] = 0;
	/*ap=90*(p+mp);
	int *npp=(int *)malloc(sizeof(int)*ap);//myMalloc(ap,0,0);
	memset(npp,0x00,sizeof(int)*ap);*/
	for (i = 1; i < height; i++)
		for (j = 1; j < width; j++) {
			if (srcBmp[i * width + j] == 255) {
				for (k = 1; k < ma; k++) {
					p = (int) (i * cos(pi * k / 180) + j * sin(pi * k / 180));
					p = (int) (p / 2 + mp / 2);
					npp[k][p] = npp[k][p]++;
					//npp[k*p+k]=npp[k*p+k]++;
				}
			}
		}
	kmax = 0;
	pmax = 0;
	n = 0;
	for (i = 1; i < ma; i++)
		for (j = 1; j < mp; j++) {
			if (npp[i][j] > yuzhi)
			//	if(npp[i*j+i]>yuzhi)
			{
				yuzhi = npp[i][j];
				//  yuzhi=npp[i*j+i];
				kmax = i;
				pmax = j;
			}
		}

	return kmax;
}

void CImgProView::Rotate(Bmp1* img1) {
	int lwidth = 0, lheight = 0;
	//int nwidth=0,nheight=0;
	//float m;

	byte* p;
	p = Rotate_RGB(img1->image, img1->ang, img1->width, img1->height, &lwidth, &lheight);
	img1->width = lwidth;
	img1->height = lheight;
	free(img1->image);
	img1->image = NULL;
	img1->image = p;

}

byte* CImgProView::Rotate_RGB(byte* image, float iRotateAngle, int width, int height, int* lwidth, int* lheight) {
	byte* temp;
	// 循环变量
	int i, j, m, n, lNewWidth, lNewHeight, i0, j0;
	// 旋转后图像的宽度和高度

	// 象素在源DIB中的坐标

	// 旋转角度（弧度）
	float fRotateAngle;
	// 旋转角度的正弦和余弦
	float fSina, fCosa;
	// 源图四个角的坐标（以图像中心为坐标系原点）
	float fSrcX1, fSrcY1, fSrcX2, fSrcY2, fSrcX3, fSrcY3, fSrcX4, fSrcY4;
	// 旋转后四个角的坐标（以图像中心为坐标系原点）
	float fDstX1, fDstY1, fDstX2, fDstY2, fDstX3, fDstY3, fDstX4, fDstY4;
	// 两个中间常量
	float f1, f2;


	// 将旋转角度从度转换到弧度
	fRotateAngle = (float) RADIAN(iRotateAngle);
	// 计算旋转角度的正弦
	fSina = (float) sin((double) fRotateAngle);
	// 计算旋转角度的余弦
	fCosa = (float) cos((double) fRotateAngle);
	// 计算原图的四个角的坐标（以图像中心为坐标系原点）
	fSrcX1 = (float) (-(width - 1) / 2);
	fSrcY1 = (float) ((height - 1) / 2);
	fSrcX2 = (float) ((width - 1) / 2);
	fSrcY2 = (float) ((height - 1) / 2);
	fSrcX3 = (float) (-(width - 1) / 2);
	fSrcY3 = (float) (-(height - 1) / 2);
	fSrcX4 = (float) ((width - 1) / 2);
	fSrcY4 = (float) (-(height - 1) / 2);

	// 计算新图四个角的坐标（以图像中心为坐标系原点）
	fDstX1 = fCosa * fSrcX1 + fSina * fSrcY1;
	fDstY1 = -fSina * fSrcX1 + fCosa * fSrcY1;
	fDstX2 = fCosa * fSrcX2 + fSina * fSrcY2;
	fDstY2 = -fSina * fSrcX2 + fCosa * fSrcY2;
	fDstX3 = fCosa * fSrcX3 + fSina * fSrcY3;
	fDstY3 = -fSina * fSrcX3 + fCosa * fSrcY3;
	fDstX4 = fCosa * fSrcX4 + fSina * fSrcY4;
	fDstY4 = -fSina * fSrcX4 + fCosa * fSrcY4;
	// 计算旋转后的图像实际宽度
	lNewWidth = (int) (max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);

	// 计算旋转后的图像高度
	lNewHeight = (int) (max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);

	temp = myMalloc(lNewHeight * lNewWidth * 3, 0, 0);
	// 两个常数，这样不用以后每次都计算了
	f1 = (float) (-0.5 * (lNewWidth - 1) * fCosa - 0.5 * (lNewHeight - 1) * fSina
		+ 0.5 * (width - 1));
	f2 = (float) (0.5 * (lNewWidth - 1) * fSina - 0.5 * (lNewHeight - 1) * fCosa
		+ 0.5 * (height - 1));


	for (i = 0; i < lNewHeight; i++) {
		// 针对图像每列进行操作
		for (m = 0 , j = 0; j < lNewWidth; m += 3 , j++) {

			// 计算该象素在源DIB中的坐标
			i0 = (int) (-((float) j) * fSina + ((float) i) * fCosa + f2 + 0.5);
			j0 = (int) (((float) j) * fCosa + ((float) i) * fSina + f1 + 0.5);

			// 判断是否在源图范围内
			if ((j0 >= 0) && (j0 < width) && (i0 >= 0) && (i0 < height)) {
				n = i0 * width * 3 + j0 * 3;
				//*(temp + lNewWidth * (lNewHeight - 1 - i) + j)=*(image + width * (height - 1 - i0) + j0);
				*(temp + lNewWidth * i * 3 + m + 1) = *(image + n + 1);
				*(temp + lNewWidth * i * 3 + m + 2) = *(image + n + 2);
				*(temp + lNewWidth * i * 3 + m) = *(image + n);
				//*(temp1 + n)=0;
			}
			else {
				// 对于源图中没有的象素，直接赋值为255
				*(temp + lNewWidth * i * 3 + m + 1) = 0;
				*(temp + lNewWidth * i * 3 + m + 2) = 0;
				*(temp + lNewWidth * i * 3 + m) = 0;
				//*(temp + lNewWidth * (lNewHeight - 1 - i) + j);
			}
		}
	}
	*lwidth = lNewWidth;
	*lheight = lNewHeight;
	return temp;
}

void CImgProView::hsvzation(BYTE* image, HSV* hsv, int width, int height) {
	int i, j, k;
	float min, max, delta, tmp;//h,s,v,m,n;
	//int lineBytes=(width*24+31)/32*4;
	byte r, g, b;
	for (i = 0; i < height; i++) {
		for (k = 0 , j = 0; j < width * 3; k++ , j += 3) {
			g = image[i * width * 3 + j + 1];
			b = image[i * width * 3 + j];
			r = image[i * width * 3 + j + 2];
			tmp = min(r, g);
			min = min(tmp, b);
			tmp = max(r, g);
			max = max(tmp, b);
			hsv[i * width + k].V = (int) max;
			delta = max - min;
			if (delta == 0) {
				hsv[i * width + k].H = 0;
				continue;
				//value=0;
				//return value;
			}
			if (max != 0) {
				//tmp=delta/max;
				hsv[i * width + k].S = delta / max;//tmp;
			}
			else {
				hsv[i * width + k].S = 0;
				hsv[i * width + k].H = 0;
				continue;
			}
			if (r == max) {
				// tmp=(g-b)/delta;
				hsv[i * width + k].H = (g - b) / delta;//tmp;
			}
			else if (g == max)
				hsv[i * width + k].H = 2 + (b - r);
			else {
				//tmp=4+(r-g)/delta;
				hsv[i * width + k].H = 4 + (r - g) / delta;//tmp;
			}
			hsv[i * width + k].H *= 60;
			if (hsv[i * width + k].H < 0)
				hsv[i * width + k].H += 360;
			//value=(int)(h+0.5);
		}
	}
}

void CImgProView::hsvzation(BYTE* image, int width, int height, BYTE* outImg) {
	BYTE r, g, b;
	int i, j;
	double h = 0, s, v;
	BYTE max, min, delta;
	int RGB_BYTEs = 3 * width;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j = j++) {
			b = image[i * 3 * width + j * 3];
			g = image[i * 3 * width + j * 3 + 1];
			r = image[i * 3 * width + j * 3 + 2];
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
				outImg[i * width + j] = 255;
			}
			else {
				outImg[i * width + j] = 0;
			}
		}
	}
	BYTE* temp1 = new BYTE[width * height];
	BYTE* temp2 = new BYTE[width * height];
	Erosion(outImg, width, height, temp1);
	Dilation(outImg, width, height, temp2);
	Erosion(outImg, width, height, temp1);
	Dilation(outImg, width, height, temp2);
	Erosion(outImg, width, height, temp1);
	Dilation(outImg, width, height, temp2);
}

byte* CImgProView::myMalloc(int num, const byte* bmp, int type) {
	byte* p = (byte*) malloc(sizeof(byte) * num);
	if (p == NULL) {
		exit(-1);
	}
	if (type == 0)
		memset(p, 0x00, sizeof(byte) * num);
	if (type == 1)
		memset(p, 255, sizeof(byte) * num);
	if (type == 2)
		memcpy(p, bmp, sizeof(byte) * num);
	return p;
}

void CImgProView::edgesob8(byte* image, int width, int height) {
	//int sum1,sum2,sum;double gray;
	int i, j, logNum;
	//int p[8];
	BYTE* temp;
	temp = (byte*) malloc(sizeof(byte) * width * height);
	memset(temp, 0x00, sizeof(byte) * width * height);
	for (i = 3; i < height - 2; i++)
		for (j = 3; j < width - 2; j++) {
			logNum = 16 * image[i * width + j] - image[(i - 2) * width + j] - image[(i - 1) * width + j - 1] - 2 * image[(i - 1) * width + j] - image[(i - 1) * width + j + 1] - image[i * width + j - 2] - 2 * image[i * width + j - 1] - 2 * image[i * width + j + 1] - image[i * width + j + 2] - image[(i + 1) * width + j - 1] - 2 * image[(i + 1) * width + j] - image[(i + 1) * width + j + 1] - image[(i + 2) * width + j];
			if (logNum > 0)
				temp[i * width + j] = 255;
			else
				temp[i * width + j] = 0;
		}
	memcpy(image, temp, width * height);
	free(temp);
	temp = NULL;
}

void CImgProView::location(byte* image, int width, int height, int yuzhi, int* HL, int* HH, int* VL, int* VH) {
	int i, j, n, maxnum, flag = 0;
	struct HSV* hsv;
	int temp[2000] = {0};
	byte* temp1;
	hsv = (struct HSV *)malloc(sizeof(struct HSV) * width * height);
	hsvzation(image, hsv, width, height);
	temp1 = (byte *) malloc(sizeof(byte) * height * width);
	//int lineBytes=(width*24+31)/32*4;
	for (i = 0; i < height; i++) {
		for (j = 0 , n = 0; j < width; n += 3 , j++) {
			if ((hsv[i * width + j].H < 220.0) && (hsv[i * width + j].H > 180.0) && (hsv[i * width + j].V < 250) && (hsv[i * width + j].S > 0.6)) {
				temp1[i * width + j] = 255;
				//putpixel(j,i,RGB(255,255,255));
			}
			else {
				temp1[i * width + j] = 0;
				//putpixel(j,i,RGB(0,0,0));
			}
		}
	}
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (temp1[i * width + j] == 255) {
				temp[i]++;
			}
		}
	}
	edgesob8(temp1, width, height);
	maxnum = temp[0];
	for (i = 1; i < height; i++) {

		if (temp[i] > maxnum)
			maxnum = temp[i];
	}
	maxnum = maxnum / 3;
	for (i = 0; i < height; i++) {
		if (flag == 0) {
			if (temp[i] > maxnum) {
				*HL = i - yuzhi;
				flag = 1;
			}
		}
		if (flag == 1) {
			if (temp[i] == 0) {
				*HH = i + yuzhi;
				break;
			}
		}
	}
	memset(temp, 0x00, sizeof(int) * width);
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			if (temp1[j * width + i] == 255) {
				temp[i]++;
			}
		}
	}
	flag = 0;
	maxnum = temp[0];
	for (i = 1; i < width; i++) {

		if (temp[i] > maxnum)
			maxnum = temp[i];
	}
	maxnum = maxnum / 3;
	for (i = 0; i < width; i++) {
		if (temp[i] > maxnum) {
			*VL = i - yuzhi;
			break;
		}
	}
	for (i = width; i > *VL; i--) {
		if (temp[i] > maxnum) {
			*VH = i + yuzhi;
			break;
		}
	}
	free(temp1);
	temp1 = NULL;
	free(hsv);
}

void CImgProView::CutBmp(BMP_img img, Bmp1* img1, int HL, int HH, int VL, int VH) {
	int i, j, n;
	//float gray;
	int x = 0, y = 0;
	img1->width = VH - VL;
	img1->height = HH - HL;
	img1->image = myMalloc(img1->height * img1->width * 3, 0, 0);//(byte *)malloc(sizeof(byte)*img1->height*img1->width*3);
	for (x = 0 , i = HL; i < HH; x++ , i++) {
		for (y = 0 , n = 0 , j = VL * 3; j < VH * 3; n++ , j += 3 , y += 3) {
			img1->image[x * img1->width * 3 + y] = img.image[i * img.biWidth * 3 + j];
			img1->image[x * img1->width * 3 + y + 1] = img.image[i * img.biWidth * 3 + j + 1];
			img1->image[x * img1->width * 3 + y + 2] = img.image[i * img.biWidth * 3 + j + 2];
		}
	}
}

void CImgProView::CutBmp1(Bmp1* img1, int HL, int HH, int VL, int VH) {
	int i, j, n;
	//float gray;
	int x = 0, y = 0;
	int width, height;
	width = img1->width;
	height = img1->height;

	//myMalloc(img1->width*img1->height*3,p,0);

	img1->width = VH - VL;
	img1->height = HH - HL;

	for (x = 0 , i = HL; i < HH; x++ , i++) {
		for (y = 0 , n = 0 , j = VL * 3; j < VH * 3; n++ , j += 3 , y += 3) {
			img1->image[x * img1->width * 3 + y] = img1->image[i * width * 3 + j];
			img1->image[x * img1->width * 3 + y + 1] = img1->image[i * width * 3 + j + 1];
			img1->image[x * img1->width * 3 + y + 2] = img1->image[i * width * 3 + j + 2];
		}
	}
}

void CImgProView::read_img(FILE* infile, BMP_img* img) {
	DWORD i, j, l, bitcolor;
	DWORD line24;
	DWORD line8;
	struct RGB* bitmap;
	fread(&img->bfType, sizeof(WORD), 1, infile);//printf("\n打开的图为 %d",img->bfType);
	fread(&img->bfSize, sizeof(DWORD), 1, infile); //        printf("\nBMP size             :%l",img->size);
	fread(&img->bfReserved, sizeof(DWORD), 1, infile);//printf("\n保留位:");
	fread(&img->bfOffBits, sizeof(DWORD), 1, infile); //printf("\nheader length    :%l",img->header_length);
	fread(&img->biSize, sizeof(DWORD), 1, infile);
	fread(&img->biWidth, sizeof(DWORD), 1, infile);
	fread(&img->biHeight, sizeof(DWORD), 1, infile); //printf( "\nwidth   :%l\n  height  :%l ", img->width, img->height);
	fread(&img->biPlanes, sizeof(WORD), 1, infile);
	fread(&img->biBitCount, sizeof(WORD), 1, infile); // printf("\nBMP Tpye             :%l ", img->bmp_type);
	fread(&img->biCompression, sizeof(DWORD), 1, infile); //if(img->compres==0) {printf("\nbmp图片为非压缩!");}printf(" ");
	fread(&img->biSizeImage, sizeof(DWORD), 1, infile);//printf("\nBMP Data Size        :%l ",img->datasize);
	fread(&img->biXPelsPerMeter, sizeof(DWORD), 1, infile);
	fread(&img->biYPelsPerMeter, sizeof(DWORD), 1, infile);
	fread(&img->biClrUsed, sizeof(DWORD), 1, infile); //printf("\n实际使用颜色数=%d ",img->clrused);printf(" ");
	fread(&img->biClrImportant, sizeof(DWORD), 1, infile);

	img->lineBytes = (img->biWidth * img->biBitCount + 31) / 32 * 4;
	//printf("\nLineBytes            :%l\n",img->lineBytes);

	line24 = (img->biWidth * 24 + 31) / 32 * 4;

	line8 = (img->biWidth * 8 + 31) / 32 * 4;
	if (img->biBitCount == 1) {
		bitcolor = 2;
		printf("不能读取退出");
		exit(-1);
	}
	if (img->biBitCount == 4) {
		bitcolor = 16;
		printf("不能读取退出");
		exit(-1);
	}
	if (img->biBitCount == 8) {
		byte* temp = (BYTE*) malloc(img->biHeight * line8 * sizeof(BYTE));
		memset(temp, 0x00, img->biHeight * img->lineBytes * sizeof(BYTE));

		bitcolor = 256;
		bitmap = (struct RGB *)calloc(bitcolor, sizeof(struct RGB));
		img->image = (unsigned char *) malloc(sizeof(unsigned char) * (line8 * img->biHeight));
		memset(img->image, 0x00, sizeof(byte) * line8 * img->biHeight);

		if (img->image == NULL) { fprintf(stderr, "\n Allocation error for temp in read_bmp() \n"); }

		fseek(infile, 0x36, SEEK_SET);
		fread(bitmap, sizeof(struct RGB), bitcolor, infile);
		fseek(infile, img->bfOffBits, SEEK_SET);
		//fread(temp, sizeof(unsigned char),lineBytes*img->height, infile);
		fread(temp, img->lineBytes * img->biHeight, 1, infile);
		if (temp == NULL)printf("\n读取失败\n");

		for (i = 0; i < img->biHeight; i++) {
			for (j = 0; j < img->biWidth; j++) {
				img->image[i * img->biWidth + j] = (byte) (0.299 * bitmap[temp[i * line8 + j]].bitb + 0.578 * bitmap[temp[i * line8 + j]].bitg + 0.114 * bitmap[temp[i * line8 + j]].bitr);
				//	putpixel(j,img->height-i,RGB(img->image[i*img->width+j],img->image[i*img->width+j],img->image[i*img->width+j]));

			}
		}
		free(temp);
		temp = NULL;
	}
	if (img->biBitCount == 24) {
		byte* temp = (byte *) malloc(sizeof(byte) * img->biHeight * img->lineBytes);
		if (temp == NULL)
			exit(-1);
		img->image = (unsigned char *) malloc(sizeof(unsigned char) * ((line24) * img->biHeight));
		if (img->image == NULL) fprintf(stderr, "\n Allocation error for temp in read_bmp() \n");
		fseek(infile, img->bfOffBits, SEEK_SET);
		fread(temp, sizeof(unsigned char), (img->lineBytes) * img->biHeight, infile);
		// byte *temp=(byte *)malloc(sizeof(byte)*img->lineBytes*img->height)

		for (i = 0; i < img->biHeight; i++) {
			l = 0;
			for (j = 0; j < img->biWidth * 3; j += 3) {
				//l=(img->height-i-1)*img->lineBytes+j;
				l = (img->biHeight - i - 1) * img->biWidth * 3 + j;
				img->image[l + 2] = *(temp + i * img->lineBytes + j + 2);
				img->image[l + 1] = *(temp + i * img->lineBytes + j + 1);
				img->image[l] = *(temp + i * img->lineBytes + j);
			}
		}

		free(temp);
		temp = NULL;
	}
}

void CImgProView::display_img(Bmp1& bmp_img1, CDC* pdc) {
	int w = bmp_img1.width;
	int h = bmp_img1.height;
	BYTE r, g, b;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w * 3; j += 3) {
			b = bmp_img1.image[i * w * 3 + j];
			g = bmp_img1.image[i * w * 3 + j + 1];
			r = bmp_img1.image[i * w * 3 + j + 2];
			pdc->SetPixelV(j / 3 + width, i + height, RGB(r, g, b));
		}
	}
}

void CImgProView::display_img(BMP_img& img, CDC* pDC) {
	int w = img.biWidth;
	int h = img.biHeight;
	BYTE r, g, b;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w * 3; j += 3) {
			b = img.image[i * w * 3 + j];
			g = img.image[i * w * 3 + j + 1];
			r = img.image[i * w * 3 + j + 2];
			pDC->SetPixelV(j / 3, i, RGB(r, g, b));
		}
	}
}

void CImgProView::display_img(BYTE* in, int width, int height, int posx, int posy, CDC* pdc) {
	int i, j;
	BYTE gray;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			gray = in[i * width + j];
			pdc->SetPixel(j + posx, i + posy, RGB(gray, gray, gray));
		}
	}
}

void CImgProView::huidu(Bmp1 img, BYTE* srcBmp, BYTE* dstBmp) {
	int i, j, n;
	float gray;
	srcBmp = img.image;
	for (i = 0; i < img.height; i++) {
		for (j = 0 , n = 0; j < img.width; n += 3 , j++) {
			gray = (*(srcBmp + i * img.width * 3 + n + 2) * 0.299) + (*(srcBmp + i * img.width * 3 + n + 1) * 0.578) + (*(srcBmp + i * img.width * 3 + n) * 0.114);
			dstBmp[i * img.width + j] = (byte) (gray + 0.5);
		}
	}
}

void CImgProView::displaytwo(Bmp1 img, BYTE* srcBmp, BYTE* dstBmp, int yuzhi) {
	//int i,j,T,n,k,t,l;
	//int m;
	//!!! Otsu Binarization
	int totalPixels = img.width * img.height;
	int bestT = 0;
	int i, j;
	int histogramArray[256] = {0};
	double densityArray[256] = {0};
	double u0 = 0;
	double u1 = 0;
	double w0 = 0;
	double w1 = 0;
	double bestDeviation = 0;
	for (i = 0; i < img.height; i++) {
		for (j = 0; j < img.width; j++) {
			histogramArray[srcBmp[i * img.width + j]]++;
		}
	}
	for (i = 0; i < 256; i++) {
		densityArray[i] = histogramArray[i] * 1.0 / totalPixels;
	}
	for (i = 0; i < 256; i++) {
		w0 = 0;
		w1 = 0;
		for (j = 0; j <= i; j++) {
			w0 += densityArray[j];
		}
		for (j = i + 1; j < 256; j++) {
			w1 += densityArray[j];
		}
		u0 = 0;
		u1 = 0;
		for (j = 0; j <= i; j++) {
			u0 += j * densityArray[j];
		}
		for (j = i + 1; j < 256; j++) {
			u1 += j * densityArray[j];
		}
		u0 = u0 / w0;
		u1 = u1 / w1;
		if (w0 * w1 * (u0 - u1) * (u0 - u1) > bestDeviation) {
			bestT = i;
			bestDeviation = w0 * w1 * (u0 - u1) * (u0 - u1);
		}
	}
	for (i = 0; i < 256; i++) {
		histogramArray[i] = 0;
	}
	bestT = bestT + yuzhi;
	if (bestT < 0)
		bestT = 0;
	if (bestT > 255)
		bestT = 255;
	for (i = 0; i < img.height; i++) {
		for (j = 0; j < img.width; j++) {
			if (srcBmp[i * img.width + j] > bestT) {
				dstBmp[i * img.width + j] = 255;
			}
			else {
				dstBmp[i * img.width + j] = 0;
			}
		}
	}
}

void CImgProView::delpoint(BYTE* dst, int width, int height, int yuzhi) {
	int i, j, num = 0, num1;
	byte* src = (byte *) malloc(sizeof(byte) * width * height);
	memset(src, 0x00, sizeof(byte) * width * height);
	for (i = 1; i < height - 1; i++)//消除孤立噪点
	{
		for (j = 1; j < width - 1; j++) {
			if (dst[i * width + j] > 200) {
				num1 = dst[i * width + j - 1] + dst[i * width + j + 1] + dst[(i - 1) * width + j - 1] + dst[(i - 1) * width + j] + dst[(i - 1) * width + j + 1] + dst[(i + 1) * width + j - 1] + dst[(i + 1) * width + j] + dst[(i + 1) * width + j + 1] + 255;
				num = num1 / 255;
				if (num >= yuzhi)
				//if((dst[i*width+j-1]==0)&&(dst[i*width+j-1]==0)&&(dst[(i-1)*width+j+1]==0)&&(dst[(i-1)*width+j]==0)&&(dst[(i-1)*width+j+1]==0)&&(dst[(i+1)*width+j-1]==0)&&(dst[(i+1)*width+j]==0)&&(dst[(i+1)*width+j+1]==0))
					src[i * width + j] = 255;
				else src[i * width + j] = 0;
			}
		}
	}
	memcpy(dst, src, sizeof(byte) * width * height);
}

void CImgProView::shuipingtouying(Bmp1* img, BYTE* dst) {
	byte temp;
	int i, j;
	//int *p=(int *)malloc(sizeof(int)*img.height);
	int p[500] = {0};
	memset(p, 0, img->height * sizeof(int));

	// 消除边缘的干扰 上下两条像素
	for (i = 0; i < img->width; i++) {
		if ((dst[i] >= 200) || (dst[img->width + i] >= 200))
			for (j = 0; j < img->height; j++) {
				if (dst[j * img->width + i] >= 200)
					dst[j * img->width + i] = 0;
				else break;
			}
	}
	for (i = 0; i < img->width; i++) {
		if ((dst[img->width * (img->height - 1) + i] >= 200) || (dst[img->width * (img->height - 2) + i] >= 200))
			for (j = img->height - 1; j > 0; j--) {
				if (dst[j * img->width + i] >= 200)
					dst[j * img->width + i] = 0;
				else break;
			}
	}


	for (i = 2; i < img->height - 2; i++) {
		for (j = 0; j < img->width; j++) {
			if (dst[i * img->width + j] >= 200)
				p[i]++; // 像素为白 则计数++
		}
	}


	temp = 0;
	for (i = 0; i < img->height; i++) {
		if (p[i] > temp) {
			temp = p[i];
		}
	}
	//temp --- max

	j = temp / 5;
	for (i = img->height; i > img->up; i--) {
		if (p[i] > j) {
			img->down = i;
			break;
		}
		if (i < img->height / 2)
			exit(-1);
	}
	//下半截 求出img.down 最远离中线的值

	j = temp / 3;
	for (i = 0; i < img->down; i++) {
		if (p[i] > j) {
			img->up = i;
			break;
		}
		if (i > img->height / 2)
			exit(-1);
	}
	//上半截 img.up最远离中线的值

	for (i = 0; i < img->up - 1; i++) {
		for (j = 0; j < img->width; j++) {
			dst[i * img->width + j] = 0;
		}

	}
	for (i = img->height; i > img->down + 1; i--) {
		for (j = 0; j < img->width; j++) {
			dst[i * img->width + j] = 0;
		}
	}
}

void CImgProView::cuizhitouying(Bmp1* img, BYTE* temp) {
	int p3[15] = {0};
	int i, j, k = 0, m, num, flag;
	//img.p1[15]={0};
	//img.p2[15]={0};
	int up = 0;
	int down = 0;
	int p[500] = {0};
	for (i = 0; i < img->width; i++) {
		for (j = 0; j < img->height; j++) {
			if (temp[j * img->width + i] == 255) {
				p[i]++;//计数
			}
		}
	}

	for (i = 1; i < img->width; i++) {//除燥
		if (p[i] > 0) {
			if ((p[i - 1] == 0) && (p[i + 1] == 0))
				if (p[i] < 5)
					p[i] = 0;
		}
	}
	flag = 0 , m = 0;
	for (i = 0; i < img->width; i++) {
		if (p[i] > 0) {
			flag = 1;
			m++;
		}
		if ((p[i] == 0) && (flag == 1))
			break;
	}

	while (1) {
		num = 0;
		flag = 0;

		for (i = 0; i < img->width; i++) {
			if (p[i] > 1) {
				if (flag == 0) {
					img->p1[num] = i;
					flag = 1;
				}

			}
			else if (p[i] == 0) {
				if (flag == 1) {
					img->p2[num] = i - 1;
					flag = 0;
					j = img->p2[num] - img->p1[num];
					if (j < m / 3) {
						if (p[i - 1] < (img->down - img->up) / 2)
							continue;
					}
					num++;
				}
			}
			if ((flag == 1) && (i == img->width - 1)) {
				img->p2[num] = i;
				num++;
				break;
			}

		}
		if (num == 7)
			break;
		else if (num == 8) {
			j = 100;
			for (i = 0; i < num; i++) {
				p3[i] = img->p2[i] - img->p1[i];
				if (p3[i] < j) {
					j = p3[i];
					k = i;
				}
			}
			for (i = k; i <= num - k; i++) {
				img->p1[i] = img->p1[i + 1];
				img->p2[i] = img->p2[i + 1];
			}
			num--;
			break;
		}
		else //(num<7)
		{
			for (i = 0; i < img->width; i++) {
				if (p[i] > 0)
					p[i]--;
			}
		}
	}
	flag = 0;
	up = 0;
	down = 0;
	up = img->up - 3;
	if (up < 0)
		up = img->up;
	down = img->down + 4;
	if (down > img->height)
		down = img->down;

	for (k = 0; k < 7; k++) {
		for (i = up; i <= down; i++) {
			for (j = img->p1[k]; j <= img->p2[k]; j++) {
				if (temp[i * img->width + j] == 255) {
					img->p3[k] = i;
					flag = 1;
					break;
				}
			}
			if (flag == 1) {
				flag = 0;
				break;
			}
		}
		for (i = down; i >= up; i--) {
			for (j = img->p1[k]; j <= img->p2[k]; j++) {
				if (temp[i * img->width + j] == 255) {
					img->p4[k] = i;
					flag = 1;
					break;
				}
			}
			if (flag == 1) {
				flag = 0;
				break;
			}
		}
	}
}

void CImgProView::strBmp(Bmp1* img, BYTE* temp) {
	int i, j, k, m, n, o;
	//int width=20,height=20;
	for (i = 0; i < 7; i++) {
		o = img->p2[i] - img->p1[i] + 1;
		for (m = 0, j = img->p3[i]; j <= img->p4[i]; j++, m++) {

			for (n = 0, k = img->p1[i]; k <= img->p2[i]; k++, n++) {
				img->strr[i][m * o + n] = temp[j * img->width + k];
			}
		}
	}
}

void CImgProView::guiyi(Bmp1* img) {
	int xxx;
	int i;
	//xxx=img->p2[0]-img->p1[0]+1;
	int yyy;
	//
	byte* temp = (byte *) malloc(sizeof(byte) * 800);
	//strBmp(&img1,temp1);
	for (i = 0; i < 7; i++) {
		xxx = img->p2[i] - img->p1[i] + 1;
		yyy = img->p4[i] - img->p3[i] + 1;
		changeGray(img->strr[i], temp, xxx, yyy, 20, 40);
		memcpy(img->strr[i], temp, sizeof(byte) * 800);
	}
}

void CImgProView::readmoban(char* path, Bmp1* img2) {
	FILE* f[72];
	int i;
	char str2[] = ".bmp";
	char str[80];
	char str1[10];
	for (i = 0; i <= 66; i++) {
		//char str[80];
		strcpy(str, path);
		//char str1[10];
		myitoa(i, str1, 10);
		strcat(str1, str2);
		strcat(str, str1);
		f[i] = fopen(str, "rb");
		if (f[i] == NULL)
			exit(-1);
		readstr(f[i], img2->strc[i]);
		fclose(f[i]);

	}


}

void CImgProView::strout(Bmp1* img) {
	int i, j, k = 0, m = 800, n;
	//int p[5]={0};
	for (i = 36; i < 67; i++) {
		// 36 First Province Hanji
		// 66 Last Province Hanji
		k = cmpstr(img->strr[0], img->strc[i]);
		if (k == 0) {
			n = i;
			break;
		}
		else {
			if (k < m) {
				m = k;
				n = i;
			}
		}
	}
	img->string[0] = n;
	for (i = 1; i < 7; i++) {
		n = 0;
		k = 0;
		m = 800;
		for (j = 0; j < 36; j++) {
			// 35 last Italian Character Z
			if ((img->p2[i] - img->p1[i]) < 4) {
				n = 1;
				break;
			}
			k = cmpstr(img->strr[i], img->strc[j]);
			if (k == 0) {
				n = j;
				break;
			}
			else {
				if (k < m) {
					m = k;
					n = j;
				}
			}
		}
		if (n == 27)//p&r
		{
			k = cmpstr(img->strc[27], img->strr[i]);
			j = cmpstr(img->strr[i], img->strc[25]);
			if (k < j)
				n = 27;
			else n = 25;
		}
		if (n == 0)//0
		{
			j = cmpstr(img->strc[0], img->strr[i]);
			k = cmpstr(img->strr[i], img->strc[12]);
			if (j > k)
				n = 12;
			else n = 0;

		}
		if (n == 26) {
			k = cmpstr(img->strc[26], img->strr[i]);
			j = cmpstr(img->strr[i], img->strc[0]);
			if (k < j)
				n = 26;
			else n = 0;
		}
		if (n == 19) {
			k = cmpstr(img->strc[0], img->strr[i]);
			j = cmpstr(img->strr[19], img->strc[i]);
			if (k < j)
				n = 0;
			else n = 19;
		}
		if (n == 13) {
			k = cmpstr(img->strc[13], img->strr[i]);
			j = cmpstr(img->strr[i], img->strc[0]);
			if (k < j)
				n = 13;
			else n = 0;
		}

		/*if((n==6)||(n==3))
		{
		k=cmpstr(img->strc[6],img->strr[i]);
		j=cmpstr(img->strc[3],img->strr[i]);
		if(k<j)
		n=5;
		else n=3;
		}
		if(n==12)
		{
		k=cmpstr(img->strc[0],img->strr[i]);
		j=cmpstr(img->strc[12],img->strr[i]);
		if(k<j)
		n=0;
		else n=12;
		}*/
		if (n == 14)//E F
		{
			k = cmpstr(img->strc[14], img->strr[i]);
			j = cmpstr(img->strr[i], img->strc[15]);
			if (k < j)
				n = 14;
			else n = 15;
		}
		if (n == 8) {
			k = cmpstr(img->strc[8], img->strr[i]);
			j = cmpstr(img->strr[i], img->strc[3]);
			if (k < j)
				n = 8;
			else n = 3;
		}
		img->string[i] = n;
		i = i;
	}
}

void CImgProView::outtext(Bmp1 img1, int x, int y) {
	char* table[] = { "0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F",
		"G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","京",
		"冀","津","晋","蒙","辽","吉","黑","沪","苏","浙","皖","闽","赣","鲁","豫","鄂","湘",
		"粤","桂","琼","渝","川","贵","云",
		"藏","陕","甘","青","宁","新" };
	int i;
	plate_String = "";
	printf("\n\n\n\n\n\n\n\n");
	printf("\n        The Car Id Is\n");
	printf("\n");
	//s2.Format("x1: %d,y1: %d\nx2: %d,y2: %d", x1, y1, x2, y2);
	//AfxMessageBox(s2, MB_OK, 0);
	for (i = 0; i < 7; i++) {
		printf("   ");
		//printf("%s", table[img1.string[i]]);
		//outtextxy(x+i*40,y,table[img1.string[i]]);
		plate_String += table[img1.string[i]];
	}
	//AfxMessageBox(plate_String, MB_OK, 0);
	flag_plate = 1;
}

void CImgProView::location(BYTE* outImg2, int width, int height, int& x1, int& x2, int& y1, int& y2) {
	int margin = 15;
	int l_length = 15; // level_length (for y)
	int v_length = 8; // vertical_length (for x)
	int i, j;
	int flagb1;
	int x1_max = width, y1_max = height, x2_max = 0, y2_max = 0;
	//255 White 0 Black
	flag_kuang = 1;
	if (outImg2) {
		for (i = 0; i < height; i++) {
			flagb1 = 0;
			for (j = 1; j < width; j++) {
				if ((i * width + j - l_length) >= 0) {
					if (outImg2[i * width + j - l_length] == 0 && outImg2[i * width + j - (l_length - 1)] == 255 && outImg2[i * width + j] == 255) {
						y1 = i - margin;
						flagb1 = 1;
						j = width;
					}
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
				if (((i - v_length) * width + j) >= 0) {
					if (outImg2[(i - v_length) * width + j] == 0 && outImg2[(i - v_length + 1) * width + j] == 255 && outImg2[i * width + j] == 255) {
						x1 = j - margin;// 3 ÎªÓàÁ¿
						flagb1 = 1;
						i = height;
					}
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
				if (((i - v_length) * width + j) >= 0) {
					if (outImg2[(i - v_length) * width + j] == 255 && outImg2[(i - 1) * width + j] == 255 && outImg2[(i * width + j)] == 0) {
						x2 = j + margin;
						flagb1 = 1;
						i = height;
					}
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
	//	hough_width = x2 - x1 + 1;
	//	hough_height = y2 - y1 + 1;
}

void CImgProView::changeGray(BYTE* srcBmp, BYTE* dstBmp, int width, int height, int nWidth, int nHeight) {
	// Size Scale
	int i = 0, j = 0, i0, j0;
	float xx;
	float yy;
	xx = (float) nWidth / width;
	yy = (float) nHeight / height;
	//memset(srcBmp,0x00,nHeight*nWidth*sizeof(byte));
	memset(dstBmp, 0x00, nHeight * nWidth * sizeof(byte));
	for (i = 0; i < nHeight; i++) {
		for (j = 0; j < nWidth; j++) {
			//i0 = (int) ((float)i/yy+0.5);
			//j0 = (int) ((float)j/xx+0.5);
			i0 = (int) ((float) i / yy);
			j0 = (int) ((float) j / xx);
			if ((j0 >= 0) && (j0 < width) && (i0 >= 0) && (i0 < height))
				// {
				dstBmp[i * nWidth + j] = srcBmp[i0 * width + j0];
			//}
			//else
			//{
			//	dstBmp[i*nWidth+j]=255;
			//}
		}
	}
}

char* CImgProView::myitoa(int num, char* str, int radix) {
	char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";/* 索引表 */
	unsigned unum; /* 中间变量 */
	int i = 0, j, k;/* 确定unum的值 */

	if (radix == 10 && num < 0) /* 十进制负数 */ {
		unum = (unsigned) -num;
		str[i++] = '-';
	}
	else unum = (unsigned) num; /* 其他情况 */
								/* 逆序 */
	do {
		str[i++] = index[unum % (unsigned) radix];
		unum /= radix;
	} while (unum);
	str[i] = '\0';
	/* 转换 */
	if (str[0] == '-') k = 1; /* 十进制负数 */
	else k = 0;
	/* 将原来的“/2”改为“/2.0”，保证当num在16~255之间，radix等于16时，也能得到正确结果 */
	for (j = k; j < (i - 1) / 2.0 + k; j++) {
		num = str[j];
		str[j] = str[i - j - 1 + k];
		str[i - j - 1 + k] = num;
	}
	return str;
}

void CImgProView::readstr(FILE* infile, BYTE* srcBmp) {
	int width, height, headlength;
	int i, j, bitcolor, line8;
	byte* temp;
	byte temp1;
	struct RGB* bitmap;
	width = 20;
	height = 40;
	headlength = 1078;
	line8 = (width * 8 + 31) / 32 * 4;
	temp = (BYTE*) malloc(height * line8 * sizeof(BYTE));
	memset(temp, 0x00, height * line8 * sizeof(BYTE));
	bitcolor = 256;
	bitmap = (struct RGB *)malloc(sizeof(struct RGB) * bitcolor);
	fseek(infile, 0x36, SEEK_SET);
	fread(bitmap, sizeof(struct RGB), bitcolor, infile);
	fseek(infile, headlength, SEEK_SET);
	fread(temp, line8 * height, 1, infile);
	if (temp == NULL) {
		printf("\n读取失败\n");
		exit(-1);
	}
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {

			temp1 = temp[i * line8 + j];
			if (temp1 >= 150)
				temp1 = 255;
			else temp1 = 0;
			srcBmp[(height - i - 1) * width + j] = temp1;//(byte)(0.299*bitmap[temp[i*line8+j]].bitb+0.578*bitmap[temp[i*line8+j]].bitg+0.114*bitmap[temp[i*line8+j]].bitr);
		}
	}
	free(temp);
	temp = NULL;
}

int CImgProView::cmpstr(BYTE* src, BYTE* moban) {
	int i, j, k = 0;
	byte temp[800] = { 0 };
	memcpy(temp, src, 800);
	for (i = 0; i < 40; i++)
		for (j = 0; j < 20; j++) {
			if (temp[i * 20 + j] == 255) {
				if (moban[i * 20 + j] == 255)
					temp[i * 20 + j] = 0;
			}
		}
	delpoint(temp, 40, 20, 4);
	for (i = 0; i < 40; i++)
		for (j = 0; j < 20; j++) {
			if (temp[i * 20 + j] == 255) {
				k++;
			}
		}
	return k;
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
		/*
		BYTE r, g, b;
		for (i = 0; i < height; i++)
			for (j = 0; j < 3 * width; j = j + 3) {
				b = rgbimg[i * 3 * width + j];
				g = rgbimg[i * 3 * width + j + 1];
				r = rgbimg[i * 3 * width + j + 2];
				pDC->SetPixelV(j / 3, i, RGB(r, g, b));
			}
*/
		display_img(bmp_img, pDC);
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
		/*
		BYTE gray;
		if (HoughImg) {
			for (i = 0; i < hough_height; i++) {
				for (j = 0; j < hough_width; j++) {
					gray = HoughImg[i * hough_width + j];
					pDC->SetPixel(j + hough_width, i + height, RGB(gray, gray, gray));
				}
			}
		}
*/
		display_img(bimg1, pDC);
		//display_img(HsvImg, hough_width, hough_height, width, height, pDC);
		/*
		CString s2;
		s2.Format("x1: %d,y1: %d\nx2: %d,y2: %d", bimg1.x1, bimg1.y1, bimg1.x2, bimg1.y2);
		AfxMessageBox(s2, MB_OK, 0);
		int x1 = bimg1.x1;
		int y1 = bimg1.y1;
		int x2 = bimg1.x2;
		int y2 = bimg1.y2;
		x1 += width;
		x2 += width;
		CPen NewPen(PS_SOLID, 1, RGB(255, 0, 0));
		pDC->SelectObject(&NewPen);
		pDC->MoveTo(x1, y1);
		pDC->LineTo(x2, y1);
		pDC->LineTo(x2, y2);
		pDC->LineTo(x1, y2);
		pDC->LineTo(x1, y1);
*/
	}
	if (flag_split) {
		display_img(bimg1.huiimage, bimg1.width, bimg1.height, width + bimg1.width, height, pDC);
	}
	if (flag_kuang) {
		int x1, x2, y1, y2;
		x1 = bmp_img.x1;
		x2 = bmp_img.x2;
		y1 = bmp_img.y1;
		y2 = bmp_img.y2;
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
	if(flag_plate) {
		TextOut(pDC->GetSafeHdc(), width*2, 0, plate_String, plate_String.GetLength());
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
	flag_split = 0;
	flag_plate = 0;

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
	//TODO: readimg()
	bmp_img.bfType = bmpFHeader.bfType;
	bmp_img.bfSize = bmpFHeader.bfSize;
	//TODO: WORD + WORD = DWORD
	bmp_img.bfReserved = static_cast<DWORD>(bmpFHeader.bfReserved1);
	bmp_img.bfOffBits = bmpFHeader.bfOffBits;

	bmp_img.biSize = bmiHeader.biSize;
	bmp_img.biWidth = bmiHeader.biWidth;
	bmp_img.biHeight = bmiHeader.biHeight;
	bmp_img.biPlanes = bmiHeader.biPlanes;
	bmp_img.biBitCount = bmiHeader.biBitCount;
	bmp_img.biCompression = bmiHeader.biCompression;
	bmp_img.biSizeImage = bmiHeader.biSizeImage;
	bmp_img.biXPelsPerMeter = bmiHeader.biXPelsPerMeter;
	bmp_img.biYPelsPerMeter = bmiHeader.biYPelsPerMeter;
	bmp_img.biClrUsed = bmiHeader.biClrUsed;
	bmp_img.biClrImportant = bmiHeader.biClrImportant;

	bmp_img.lineBytes = (bmp_img.biWidth * bmp_img.biBitCount + 31) / 32 * 4;
	DWORD i, j, l, bitcolor;
	DWORD line24 = (bmp_img.biWidth * 24 + 31) / 32 * 4;
	DWORD line8 = (bmp_img.biWidth * 8 + 31) / 32 * 4;

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
				bmp_img.image = rgbimg;
			}
	}
	if (bicount == 24) {
		image = new BYTE[width * height];
		rgbimg = new BYTE[rowLen * height];
		for (int i = height - 1; i >= 0; i--) {
			fread(rowBuff, 1, rowLen, fpImg);
			memcpy(rgbimg + i * 3 * width, rowBuff, 3 * width);
		}
		bmp_img.image = rgbimg;
	}
	delete[] rowBuff;

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
	for (i = 0; i < height; i++) {
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
	if (!flag) {
		Color();
	}
	location(huiimg, bmp_img.biWidth, bmp_img.biHeight, bmp_img.x1, bmp_img.x2, bmp_img.y1, bmp_img.y2);
	OnInitialUpdate();
}

void CImgProView::Split() {
	if(!flag_hough) {
		Hough();
	}
	//AfxMessageBox("Split!!!", MB_OK, 0);
	temp1 = myMalloc(bmp_img.biHeight * bmp_img.biWidth * 4, temp, 0);
	temp = myMalloc(bmp_img.biHeight * bmp_img.biWidth * 4, temp, 0);

	huidu(bimg1, bimg1.image, temp);
	displaytwo(bimg1, temp, temp1, 30);

	delpoint(temp1, bimg1.width, bimg1.height, 3); // 消除噪点
	shuipingtouying(&bimg1, temp1);

	delpoint(temp1, bimg1.width, bimg1.height, 2); // 消除噪点
	cuizhitouying(&bimg1, temp1);


	//bimg1.image = temp1; //test Otsu
	//	shuipingtouying(&bimg1, temp1);
	bimg1.huiimg = temp1;
	memset(temp, 0, sizeof(char)*bimg1.width*bimg1.height);
	flag_split = 1;

	OnInitialUpdate();
}

void CImgProView::Recognize() {
	if(!flag_split) {
		Split();
	}
	char path1[80] = "test\\moban\\";
//	AfxMessageBox("Recognize!!!", MB_OK, 0);
	memset(temp, 0, sizeof(char)*bimg1.width*bimg1.height);
	strBmp(&bimg1, temp1);
	guiyi(&bimg1);
	readmoban(path1, &bimg1);
	strout(&bimg1);
	outtext(bimg1, 0, 600);

	
	OnInitialUpdate();
}

void CImgProView::Hough() {
	if (!flag_kuang) {
		Extract();
	}
	/*
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
	memcpy(HoughImg, temp, sizeof(BYTE) * hough_width * hough_height);
*/
	int HL = 0, HH = 0, VH = 0, VL = 0;

	//location(bmp_img.image, bmp_img.biWidth, bmp_img.biHeight, 15, &HL, &HH, &VL, &VH);
	CutBmp(bmp_img, &bimg1, bmp_img.y1, bmp_img.y2, bmp_img.x1, bmp_img.x2);

	Hough1(&bimg1);
	Rotate(&bimg1);

	location(bimg1.image, bimg1.width, bimg1.height, 0, &HL, &HH, &VL, &VH);
	CutBmp1(&bimg1, HL, HH, VL, VH);
	bimg1.x1 = HL;
	bimg1.x2 = HH;
	bimg1.y1 = VL;
	bimg1.y2 = VH;

	//bimg1.huiimage = new BYTE[width * height];
	//hsvzation(bimg1.image, bimg1.width, bimg1.height, bimg1.huiimage);
	//location(bimg1.huiimage, bimg1.width, bimg1.height, bimg1.x1, bimg1.x2, bimg1.y1, bimg1.y2);

	//CutBmp1(&bimg1, bimg1.x1, bimg1.x2, bimg1.y1, bimg1.y2);

	//AfxMessageBox("FUCK", MB_OK, 0);
	flag_hough = 1;
	OnInitialUpdate();
}
