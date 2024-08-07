#pragma once

#include <string.h>
#include <stdint.h>
#include <iostream>
#include <stdexcept>
#include "../../../CImg-3.4.0/CImg.h"
#include "../../../type.h"
#include "../font_library/font.h"

//最好使用锁机制进行限定，或者每次由函数生成新的区域
//在scr/font/ku500.cpp中声明的缓冲区
extern char image[FTX][FTY];
//在scr/font/ku500.cpp中声明的绘制字体的函数
extern void drawvfont(int x, int y, unsigned short z, int xsize, int ysize);
//在scr/font/ku500.cpp中声明的清空缓冲区的函数
extern void blank_img();

namespace qing {

    class Area: public Unit {
    //------------------------------
    /*
    方案二：你可以参考一下一般的设计：Painter画笔、
    PaintDevice画布。Painter提供drawLine、
    drawRect、drawPixmap、drawText等绘图方法。
    PaintDevice维护一个可绘制的缓冲区buffer，接受Painter
    的绘制。Pixmap继承PaintDevice用于保图案并提供
    浅拷贝、深拷贝、转置、operator[]寻址等方法。
    */
    //------------------------------

    public:
        //构造函数
        //仅仅只是一个区域，不需要传入设备指针
        Area(size_t w, size_t h);
        // open a picture with CImg
        Area(std::string path);
        //作为一个窗体应该可以拥有复制构造函数，但是这里我们先空出这个功能
        Area(Area&) = delete;
	//析构函数
        ~Area();
	//--------属性存取--------
	//获取窗口的宽度
        size_t getw();
	//获取窗口的高度
	size_t geth();
        //获取缓冲区指针
        void* getbuf();
        //----------------------
	//将整个区域设置成一种颜色-
        void paint(uint32_t color);
	// 将窗体完整地画到画布上，需要输入一个窗体类型的指针，
	// 以及窗体的位置信息、框体颜色
	void pa(Area *a1, size_t x, size_t y, size_t frame_width, uint32_t color);
        //在窗体上打印字符
	void print (char *p, size_t cw, size_t ch, uint32_t color);


    private:
        //根据模板类确定缓冲区的大小    
        void *buf;
        //矩形区域的参数
        size_t w, h;

    };//Area

}//qing

