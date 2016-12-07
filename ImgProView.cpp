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
			img1->image[x * img1->width * 3 + y] = img.image[i * img.width * 3 + j];
			img1->image[x * img1->width * 3 + y + 1] = img.image[i * img.width * 3 + j + 1];
			img1->image[x * img1->width * 3 + y + 2] = img.image[i * img.width * 3 + j + 2];
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
	fread(&img->size, sizeof(DWORD), 1, infile); //        printf("\nBMP size             :%l",img->size);
	fread(&img->reser, sizeof(DWORD), 1, infile);//printf("\n保留位:");
	fread(&img->header_length, sizeof(DWORD), 1, infile); //printf("\nheader length    :%l",img->header_length);
	fread(&img->infoheader_length, sizeof(DWORD), 1, infile);
	fread(&img->width, sizeof(DWORD), 1, infile);
	fread(&img->height, sizeof(DWORD), 1, infile); //printf( "\nwidth   :%l\n  height  :%l ", img->width, img->height);
	fread(&img->biplanes, sizeof(WORD), 1, infile);
	fread(&img->bmp_type, sizeof(WORD), 1, infile); // printf("\nBMP Tpye             :%l ", img->bmp_type);
	fread(&img->compres, sizeof(DWORD), 1, infile); //if(img->compres==0) {printf("\nbmp图片为非压缩!");}printf(" ");
	fread(&img->datasize, sizeof(DWORD), 1, infile);//printf("\nBMP Data Size        :%l ",img->datasize);
	fread(&img->bixpm, sizeof(DWORD), 1, infile);
	fread(&img->biypm, sizeof(DWORD), 1, infile);
	fread(&img->clrused, sizeof(DWORD), 1, infile); //printf("\n实际使用颜色数=%d ",img->clrused);printf(" ");
	fread(&img->relclrused, sizeof(DWORD), 1, infile);

	img->lineBytes = (img->width * img->bmp_type + 31) / 32 * 4;
	//printf("\nLineBytes            :%l\n",img->lineBytes);

	line24 = (img->width * 24 + 31) / 32 * 4;

	line8 = (img->width * 8 + 31) / 32 * 4;
	if (img->bmp_type == 1) {
		bitcolor = 2;
		printf("不能读取退出");
		exit(-1);
	}
	if (img->bmp_type == 4) {
		bitcolor = 16;
		printf("不能读取退出");
		exit(-1);
	}
	if (img->bmp_type == 8) {
		byte* temp = (BYTE*) malloc(img->height * line8 * sizeof(BYTE));
		memset(temp, 0x00, img->height * img->lineBytes * sizeof(BYTE));

		bitcolor = 256;
		bitmap = (struct RGB *)calloc(bitcolor, sizeof(struct RGB));
		img->image = (unsigned char *) malloc(sizeof(unsigned char) * (line8 * img->height));
		memset(img->image, 0x00, sizeof(byte) * line8 * img->height);

		if (img->image == NULL) { fprintf(stderr, "\n Allocation error for temp in read_bmp() \n"); }

		fseek(infile, 0x36, SEEK_SET);
		fread(bitmap, sizeof(struct RGB), bitcolor, infile);
		fseek(infile, img->header_length, SEEK_SET);
		//fread(temp, sizeof(unsigned char),lineBytes*img->height, infile);
		fread(temp, img->lineBytes * img->height, 1, infile);
		if (temp == NULL)printf("\n读取失败\n");

		for (i = 0; i < img->height; i++) {
			for (j = 0; j < img->width; j++) {
				img->image[i * img->width + j] = (byte) (0.299 * bitmap[temp[i * line8 + j]].bitb + 0.578 * bitmap[temp[i * line8 + j]].bitg + 0.114 * bitmap[temp[i * line8 + j]].bitr);
				//	putpixel(j,img->height-i,RGB(img->image[i*img->width+j],img->image[i*img->width+j],img->image[i*img->width+j]));

			}
		}
		free(temp);
		temp = NULL;
	}
	if (img->bmp_type == 24) {
		byte* temp = (byte *) malloc(sizeof(byte) * img->height * img->lineBytes);
		if (temp == NULL)
			exit(-1);
		img->image = (unsigned char *) malloc(sizeof(unsigned char) * ((line24) * img->height));
		if (img->image == NULL) fprintf(stderr, "\n Allocation error for temp in read_bmp() \n");
		fseek(infile, img->header_length, SEEK_SET);
		fread(temp, sizeof(unsigned char), (img->lineBytes) * img->height, infile);
		// byte *temp=(byte *)malloc(sizeof(byte)*img->lineBytes*img->height)

		for (i = 0; i < img->height; i++) {
			l = 0;
			for (j = 0; j < img->width * 3; j += 3) {
				//l=(img->height-i-1)*img->lineBytes+j;
				l = (img->height - i - 1) * img->width * 3 + j;
				img->image[l + 2] = *(temp + i * img->lineBytes + j + 2);
				img->image[l + 1] = *(temp + i * img->lineBytes + j + 1);
				img->image[l] = *(temp + i * img->lineBytes + j);
			}
		}

		free(temp);
		temp = NULL;
	}
}

void CImgProView::display_img(Bmp1* img1, CDC* pdc) {
	int w = img1->width;
	int h = img1->height;
	BYTE r, g, b;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w * 3; j += 3) {
			b = img1->image[i * w * 3 + j];
			g = img1->image[i * w * 3 + j + 1];
			r = img1->image[i * w * 3 + j + 2];
			pdc->SetPixelV(j / 3, i + height, RGB(r, g, b));
		}
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
		display_img(&img1, pDC);
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
	/*
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
	memcpy(HoughImg, temp, sizeof(BYTE) * hough_width * hough_height);
*/
	FILE* f;
	char path[80] = "test\\2.bmp";
	byte *temp, *temp1;
	int HL = 0, HH = 0, VH = 0, VL = 0;

	if ((f = fopen(path, "rb")) == NULL) {
		printf("\nCan not open the path: %s \n", path);
		exit(-1);
	}
	read_img(f, &img);
	fclose(f);
	temp = nullptr;
	temp = myMalloc(img.height * img.width * 4, temp, 0);//(byte *)malloc(sizeof(byte)*img.height*img.width*4);// byte *temp2=(byte *)malloc(sizeof(byte)*img.height*img.width);
	temp1 = myMalloc(img.height * img.width * 4, temp, 0);

	location(img.image, img.width, img.height, 15, &HL, &HH, &VL, &VH);
	CutBmp(img, &img1, HL, HH, VL, VH);

	Hough1(&img1);
	Rotate(&img1);

	//AfxMessageBox("FUCK", MB_OK, 0);
	flag_hough = 1;
	OnInitialUpdate();
}
