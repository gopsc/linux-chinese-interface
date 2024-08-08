// showzhi.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 作者：陈必红
// 修改：qing

#include <iostream>
#include "font.h"
extern unsigned char ttu[];
extern int ttunumber;
extern unsigned char zzhi[];
extern int zzhinumber;
// 将ku500中的四个变量改名在下面
unsigned short* chss = (unsigned short*)zzhi;
unsigned char* vec = ttu;
int zn = zzhinumber;
int vennum = ttunumber;

char image[FTX][FTY];
void pixel(int ix, int iy) {
    if (ix < 0 || ix >= FTX || iy < 0 || iy >= FTY)return;
    image[ix][iy] = '*';
}
void line(double x1, double y1, double x2, double y2) {
	double t, dt;
	double i, j;
	i = x1;
	j = y1;
	if (abs(x2 - x1) > abs(y2 - y1))
		dt = 1 / (abs(x2 - x1) * 2.0);
	else
		dt = 1 / (abs(y2 - y1) * 2.0);
	for (t = 0; t <= 1.0; t += dt)
	{
		pixel(int(i+0.5), int(j+0.5));
		i += dt * (x2 - x1);
		j += dt * (y2 - y1);
	}
}
unsigned char* drawvect(int x, int y, unsigned char* vect, int width, int high)
// x,y是左上角坐标,vect指向矢量图数据,width和high分别代表要绘制的图的宽与高
{
	if (!vect)return NULL;
	int i = 0, j = 1;// 指向第一对数字
	unsigned char ix, iy;// 从数组取出的一对坐标放在这儿
	double x1, y1, x2, y2;// 绘制直线的实数的坐标
	if (high == 0)high = width;// 调节高宽
	int ii1, jj1, ii2, jj2;// 最后算出的绘制直线的坐标
	bool isend = false;//是否到达曲线尾
	bool isallend = false;//是否结束整个图像
	do
	{
		ix = vect[i];
		iy = vect[j];
		isend = ix & 1; // 奇数代表曲线尾
		isallend = iy & 1; // 奇数整个图像结束
		if (isallend) {
			i += 2;
			break; // 整个图像结束返回
		}
		if (isend) {// 如果这里曲线结束什么也不画进到下一条曲线
			i += 2;
			j += 2;
			continue;
		}
		ix /= 2;
		iy /= 2;
		x1 = ix;
		y1 = iy;
		x1 = width * x1 / 128.0;
		y1 = high * y1 / 128.0;
		ii1 = (int)(x1 + x);
		jj1 = (int)(y1 + y);
		int k;
		for (k = 0; k < 100000; k++) {
			x2 = x1;
			y2 = y1;
			ii2 = ii1;
			jj2 = jj1;
			i += 2;
			j += 2;
			ix = vect[i];
			iy = vect[j];
			isend = ix & 1; // 奇数代表曲线尾
			isallend = iy & 1; // 奇数整个图像结束
			ix /= 2;
			iy /= 2;
			x1 = ix;
			y1 = iy;
			x1 = width * x1 / 128.0;
			y1 = high * y1 / 128.0;
			ii1 = (int)(x1 + x);
			jj1 = (int)(y1 + y);
			line(ii1, jj1, ii2, jj2);
			if (isend || isallend)
				break;
		}
		i += 2;// 指向下一条曲线
		j += 2;
	} while (!isallend);
	return vect + i;
}
void drawvfont(int x, int y, unsigned short z, int xsize, int ysize)
{
	int i, j, k, ii;
	j = -1;
	for (i = 0; i < zn; i++) {
		if (chss[i] == z) {
			j = i;
			break;
		}
	}
	if (j < 0)return;
	k = 0;
	for (i = 0; i < vennum; i++) {
		if (k == j)
			break;
		for (ii = i; ii < vennum; ii++) {
			if (vec[2 * ii + 1] & 1) {
				k++;
				i = ii;
				break;
			}
		}
	}
	if (k < j)return;
	drawvect(x, y, vec + 2 * i, xsize, ysize);
}

void blank_img()
{
    for (int i = 0; i < FTX; i++)for (int j = 0; j < FTY; j++)
        image[i][j] = ' ';
}
/*
int main()
{
    int i, j;
    for (i = 0; i < TX; i++)for (j = 0; j < TY; j++)
        image[i][j] = ' ';
	//line(2, 5, 10, 13);
	char a[] = "一";
	drawvfont(0, 0, *(unsigned short*)a, TX, TY);
    for (j = 0; j < TX; j++) {
        for (i = 0; i < TY; i++)
            printf("%c", image[i][j]);
        printf("\n");
    }
    return 0;
}
*/
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
