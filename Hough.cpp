# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <malloc.h>
# include <ctype.h>
# include <process.h>
# include <windows.h>
# include <math.h>

typedef struct {
	int value;
	int Dist;
	int AngleNumber;
} maxValue;

#define pi 3.1415926

int HoughBMP(char* OriginalBMP, char* ResultBMP);

int main() {
	HoughBMP("E:\\xljFile\\pictureProcessing\\test.bmp", "E:\\xljFile\\pictureProcessing\\line3.bmp");
	return 0;
}

int HoughBMP(char* OriginalBMP, char* ResultBMP) {
	//缓存图像和原始图像
	unsigned char *BMPBeforeTrans, *BMPAfterTrans;

	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	RGBQUAD Color[256];

	int Row, Col, pixel;
	//循环变量
	int i, j, k, v = 0;

	int iMaxDist;
	//变换域指针
	int* lpTransArea;

	maxValue maxValue1;
	maxValue maxValue2;


	//变换域的坐标
	int iDist;
	int iAngleNumber;
	int iMaxAngleNumber = 90;

	FILE *f, *p;

	//打开文件失败
	if ((f = fopen(OriginalBMP, "rb")) == NULL) {
		printf("open %s error,please check\n");
		return 0;
	}

	//读取文件头
	fread(&fh, sizeof(BITMAPFILEHEADER), 1, f);
	if (fh.bfType != 'MB') {
		printf("This is not a BMP picture\n");
		return 0;
	}

	//读取文件信息头
	fread(&ih, sizeof(BITMAPINFOHEADER), 1, f);
	Row = ih.biHeight;
	Col = ih.biWidth;
	//列数需为4的倍数
	Col = (Col + 3) / 4 * 4;
	iMaxDist = (int) sqrt(Row * Row + Col * Col);

	//为缓存图像以及变换域分配存储空间
	lpTransArea = (unsigned int*) calloc(iMaxAngleNumber * iMaxDist, sizeof(int));
	BMPBeforeTrans = (unsigned char*) calloc(Row * Col, sizeof(unsigned char));
	BMPAfterTrans = (unsigned char*) calloc(Row * Col, sizeof(unsigned char));

	//初始化变换域
	for (k = 0; k < iMaxAngleNumber * iMaxDist; k++) {
		lpTransArea[k] = 0;
	}

	fseek(f, sizeof(RGBQUAD) * 256 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), 0);
	fread(BMPBeforeTrans, sizeof(unsigned char), Row * Col, f);

	//统计图像上的每一个黑点，将其量化数组储存在变换域
	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++) {
			BMPBeforeTrans[i * Col + 315] = 255;
			pixel = BMPBeforeTrans[i * Col + j];

			//如果图像不是二值图，退出程序
			if (pixel != 0 && pixel != 255) {
				printf("this picture is not correct");
				return 0;
			}
			if (pixel == 0) {

				for (iAngleNumber = 0; iAngleNumber < iMaxAngleNumber; iAngleNumber++) {
					iDist = (int) fabs(i * cos(iAngleNumber * 2 * pi / 180.0) + j * sin(iAngleNumber * 2 * pi / 180.0));
					lpTransArea[iDist * iMaxAngleNumber + iAngleNumber] = lpTransArea[iDist * iMaxAngleNumber + iAngleNumber] + 1;

				}
			}
		}
	maxValue1.value = 0;
	maxValue2.value = 0;
	//查找出第一条直线
	for (iDist = 0; iDist < iMaxDist; iDist++) {
		for (iAngleNumber = 0; iAngleNumber < iMaxAngleNumber; iAngleNumber++) {
			if ((int) lpTransArea[iDist * iMaxAngleNumber + iAngleNumber] > maxValue1.value) {
				maxValue1.value = (int) lpTransArea[iDist * iMaxAngleNumber + iAngleNumber];
				maxValue1.Dist = iDist;
				maxValue1.AngleNumber = iAngleNumber;
			}

		}
	}
	printf(" maxValue1.value= %d\n", maxValue1.value);
	printf(" maxValue1.AngleNumber= %d\n", maxValue1.AngleNumber);
	printf(" maxValue1.Dist= %d \n", maxValue1.Dist);

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
	printf(" maxValue2.value= %d\n", maxValue2.value);
	printf(" maxValue2.AngleNumber= %d\n", maxValue2.AngleNumber);
	printf(" maxValue2.Dist= %d \n  ", maxValue2.Dist);

	//将第二条直线输出到缓存图像
	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			iDist = (int) fabs(i * cos(maxValue1.AngleNumber * 2 * pi / 180.0) + j * sin(maxValue1.AngleNumber * 2 * pi / 180.0));
			if (iDist == maxValue2.Dist)
				BMPAfterTrans[i * Col + j] = (unsigned char) 255;
		}
	}

	fclose(f);
	if ((p = fopen(ResultBMP, "wb")) == NULL) {
		printf("open savefile error\n");
		return 0;
	}
	for (i = 0; i < 256; i++) {
		Color[i].rgbBlue = i;
		Color[i].rgbGreen = i;
		Color[i].rgbRed = i;
		Color[i].rgbReserved = 0;
	}

	//写入缓存图像数据
	fwrite(&fh, sizeof(BITMAPFILEHEADER), 1, p);
	fwrite(&ih, sizeof(BITMAPINFOHEADER), 1, p);
	fwrite(Color, sizeof(RGBQUAD), 256, p);

	fwrite(BMPAfterTrans, sizeof(unsigned char), Row * Col, p);

	fclose(p);
	return 0;
}
