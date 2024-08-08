#pragma once

#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <unistd.h>
#include <iostream>
#include <mutex>
#include "../../type.h"
#include "Area/Area.h"
#include "../data/string.h"
#include "../Printer/StandardPrinter.h"
#include "../script/DScript.h"

/*
@qing：此为程序的屏幕控制器，其他模块通过读写linux系统的帧缓冲区在显示器上面显示图形。
*/

// c cache
namespace qing {


    /*colors*/	
    const uint32_t BLACK32 = 0x000000;
    const uint32_t WHITE32 = 0xffffff;
    const uint32_t GREEN32 = 0x00ff00;
    const uint32_t RED32 = 0xff0000;
    const uint32_t BLUE32 = 0x0000ff;
    const uint32_t YELLOW32 = 0xffff00;
    const uint32_t CYAN32 = 0x00ffff;

    const uint16_t BLACK16 = 0x0000;
    const uint16_t WHITE16 = 0xffff;
    const uint16_t GREEN16 = 0x0f00;
    const uint16_t RED16 = 0xf000;
    const uint16_t BLUE16 = 0x00ff;
    const uint16_t YELLOW16 = 0xff00;
    const uint16_t CYAN16 = 0x0fff;


    //前向声明
    class StandardPrinter;

    class CmdPainter: public Unit {
        /*命令行画家类，负责显示器调度和区域创建
        可以使用DScript定义每个区域的参数

        将该类定义为线程，并持续对输入栏进行刷新*/

        public:
            //构造函数
            CmdPainter(StandardPrinter *printer, DScript *script, std::string imgpath);        
            // 该类型暂不支持复制构造函数
            CmdPainter(CmdPainter&) = delete;
            //析构函数
            ~CmdPainter();
            //将这些缓冲区绘制到屏幕缓冲区，随后刷新
        //----------------------------------------------------------
            
            std::mutex lk;
            Area *a0 = NULL; //整个屏幕区域
            /*双缓冲区*/
            //Area *buf = NULL;
            
            int char_w, char_h; //命令行用
            int char_w1, char_h1; //状态栏用
            int char_w2, char_h2; //for title
            int border_w0, border_w1, border_w2;

            int w4, h4, x4, y4; //输入栏
            Area *a4 = NULL; 

            int w2, h2, x2, y2; //状态栏
            Area *a2 = NULL; //状态栏

            int w2_1, h2_1, x2_1, y2_1; //title
            Area *a2_1 = NULL;

            int w3, h3, x3, y3; //命令行界面        
            Area *a3 = NULL;

            int w3_1, h3_1, x3_1, y3_1; //显示文字的区域
            Area *a3_1 = NULL;
            
        //----------------------------------------------------------
            void Paint();
            
            void flush();
            
        private:
            //BasicLinuxScreen *screen = NULL;
            int w, h; //主要缓冲区用
            bool rand;//旋转
            
            int fd;
            struct fb_var_screeninfo var_info;
            struct fb_fix_screeninfo fix_info;
            void* frameBuf = NULL;//帧缓冲的内存地址，设备映射到这里
        
    }; //命令行绘图机

} //qing
