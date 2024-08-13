
#include "CmdPainter.h"

/*main.cpp*/
extern bool isDebug;

namespace qing{
	
    CmdPainter::CmdPainter(StandardPrinter *printer, DScript *script, std::string imgpath) {
        // 构造函数
        
        /* 从脚本中打开有关帧缓冲设备的信息 */	    

	std::string devstr = script->Open("配置—屏幕—帧缓冲");

        std::string randstr = script->Open("配置—屏幕—旋转");
        this->rand = randstr == "真" ? true : randstr == "假" ? false : false;

        //字体大小
	std::string fontstr = script->Open("配置—屏幕—字号");

	//---------------------------------------------------------------------------------

	/* 打开帧缓冲设备 */
        this->fd = open(devstr.c_str(), O_RDWR);
        if (this->fd == -1) throw std::runtime_error("打开帧缓冲设备时发生了错误。");
        
        /* 读取可变参数 */
        if (ioctl(fd, FBIOGET_VSCREENINFO, &(this->var_info)) < 0) {
            ::close(this->fd);
            throw std::runtime_error("读取可变参数时发生了错误。");  
        }

	/* 读取固定参数 */
        if (ioctl(fd, FBIOGET_FSCREENINFO, &(this->fix_info)) < 0) {
            ::close(this->fd);
            throw std::runtime_error("读取固定参数时发生了错误。");
        }
            
        //检查帧缓冲设备的变量
	if (isDebug) {
            std::cout << "Linux screen just boot." << std::endl;
            std::cout << "Descartes:\t"     << var_info.xres            << "," << var_info.yres << std::endl;
            std::cout << "Virtual:\t"       << var_info.xres_virtual    << "," << var_info.yres_virtual << std::endl;
            std::cout << "Buffer length:\t" << fix_info.smem_len        << std::endl;
            std::cout << "Start point:\t"   << fix_info.smem_start      << std::endl;
            std::cout << "pixel size:\t"    << var_info.bits_per_pixel  << std::endl;
        }

	/* 目前仅支持32位像素比特的屏幕驱动 */
	if (var_info.bits_per_pixel != 32){
	    ::close(this->fd);
	    throw std::runtime_error("不支持该驱动的像素比特。");
	}

        /* 将帧缓冲设备映射到内存 */
        this->frameBuf = mmap(this->frameBuf, this->fix_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, this->fix_info.smem_start);
        if (this->frameBuf == MAP_FAILED) {
            ::close(this->fd);
            throw std::runtime_error("将帧缓冲设备映射到内存区时发生错误。" );
        }
//-------------------------------------------------------------
        
        /* 获取显示器的尺寸 */
        w = (this->rand == false) ? this->var_info.xres : this->var_info.yres;
        h = (this->rand == false) ? this->var_info.yres : this->var_info.xres;

	/* 利用显示器的尺寸初始化主区域 */
        a0 = new Area(w, h);


        /* 字体大小 */
        char_w = std::stoi(fontstr);
        char_h = char_w;

        /* 用于状态栏的字的大小 */              
        char_w1 = char_w * 0.8;
        char_h1 = char_h * 0.8;

        /* 用于标题栏的字体大小 */         
        char_w2 = char_w * 1.4;
        char_h2 = char_h * 1.4;
            

        /* 边框宽度 */
        border_w0 = 1;
        border_w1 = 0;
	border_w2 = 4;

            
//-------------------------------------------------------------
//-------------------------------------------------------------
	int 命令行文字部分_外边距 = border_w2 / 0.4;
	int 状态栏文字部分_外边距 = char_w2 * 1.6;
        //----------------------------------------
	/* 因为要根据图片的大小确定标题栏的高度，进而
	 * 确定命令行栏的高度，所以我们把标题区域的
	 * 初始化放到参数定义中 */
        a2_1 = new Area(imgpath);
        w2_1 = a2_1->getw();
        h2_1 = a2_1->geth();
        x2_1 = 状态栏文字部分_外边距;
        y2_1 = 0;
        //----------------------------------------
//-------------------------------------------------------------
//============================================================= 
        int 状态栏顶部间距 = 1 * char_h;
        int 状态栏底部间距 = 1 * char_h;
        int 状态栏左边距  = 1 * char_w;
        int 状态栏右边距  = 1 * char_w;
        int 状态栏高度    = h2_1;
        int 状态栏宽度    = w - 状态栏左边距 - 状态栏右边距;
            
        int 输入栏顶部间距 = 1 * char_h;
        int 输入栏底部间距 = 1 * char_w;
        int 输入栏左边距  = 1 * char_w;
        int 输入栏右边距  = 1 * char_w;
        int 输入栏高度    = 3 * char_h;
        int 输入栏宽度    = w - 输入栏左边距 - 输入栏右边距;
            
        int 命令行顶部间距 = -1;
        int 命令行底部间距 = -1;
        int 命令行左边距   = 1 * char_h;
        int 命令行右边距   = 1 * char_h;
        int 命令行高度    = h - (状态栏高度 + 状态栏顶部间距 + 状态栏底部间距) - (输入栏高度 + 输入栏底部间距 + 输入栏顶部间距);
        int 命令行宽度_期望 = (w - 命令行左边距 - 命令行右边距) * 0.5;
//        int 命令行宽度_阀值 = 15 * 字符宽;
//        int 命令行宽度    = 命令行宽度_期望 > 命令行宽度_阀值 ? 命令行宽度_期望 : 命令行宽度_阀值;
        int 命令行宽度     = w - 命令行左边距 - 命令行右边距;
	int 命令行文字部分_高度 = 命令行高度 - 2 * 命令行文字部分_外边距;
	int 命令行文字部分_宽度 = 命令行宽度 - 2 * 命令行文字部分_外边距;
        //----------------------------------------
                
//============================================================= 

        /* 初始化状态栏区域 */
        w2   = 状态栏宽度;
        h2   = 状态栏高度;
        a2   = new Area(w2, h2);
        a2->paint(BLACK32);
        x2   = 状态栏左边距;
        y2   = 状态栏顶部间距;
        
        /* 初始化命令行区域 */
        w3 = 命令行宽度;
        h3 = 命令行高度;
        a3 = new Area(w3, h3); 
        a3->paint(BLACK32);
        x3 = 命令行左边距;
        y3 = 状态栏高度 + 状态栏顶部间距 + 状态栏底部间距;

	/* 初始化命令行文字部分区域 */
	w3_1 = 命令行文字部分_宽度;
	h3_1 = 命令行文字部分_高度;
	a3_1 = new Area(w3_1, h3_1);
	a3_1->paint(BLACK32);
	x3_1 = 命令行文字部分_外边距;
        y3_1 = 命令行文字部分_外边距;

        /* 初始化输入栏区域 */
        w4 = 输入栏宽度;
        h4 = 输入栏高度;
        a4 = new Area(w4, h4);
        a4->paint(BLACK32);
        x4   = 输入栏左边距;
        y4   = 命令行高度 + (状态栏高度 + 状态栏顶部间距 + 状态栏底部间距) + 输入栏顶部间距 ;

//============================================================= 

        /* 除了标题栏的图像区域之外，全部区域初始化为空白 */
        a0->paint(BLACK32);
        a2->paint(BLACK32);
        //a2_1->paint(BLACK32);
        a3->paint(BLACK32);
        a3_1->paint(BLACK32);

        /* 启动成功，刷新一次屏幕 */
	this->Paint();
//============================================================= 
#ifndef DEBUG
        //设置输出宽度和高度
        printer->Set(w3_1/char_w, h3_1/char_h + 1);
#endif
    } //构造函数
        
    CmdPainter::~CmdPainter() {
        /* 析构函数 */

        /* 将所有的区域释放 */
        if (a4          != NULL) delete(a4);
        if (a2          != NULL) delete(a2);
        if (a2_1        != NULL) delete(a2_1);
        if (a3          != NULL) delete(a3);
	if (a3_1        != NULL) delete(a3_1);
        if (a0          != NULL) delete(a0);    
                
        //从内存区解除映射
        if (this->frameBuf != NULL && this->frameBuf != MAP_FAILED && munmap(this->frameBuf,this->fix_info.smem_len) != -1){
            this->frameBuf = NULL;
        }
        if (this->fd != -1) {
            close(this->fd);
            this->fd = -1;
        }

    } //析构函数

    void CmdPainter::Paint() {
        //可以把区域刷新封装到绘图器里面去
        
        a2->pa(a2_1, x2_1, y2_1, border_w1, WHITE32);
	a3->pa(a3_1, x3_1, y3_1, border_w1, WHITE32);
        
        a0->pa(a3, x3, y3, border_w1, WHITE32);
        a0->pa(a2, x2, y2, border_w1, WHITE32);
        a0->pa(a4, x4, y4, border_w1, WHITE32);
        
        this->flush();
    } //绘制

    void CmdPainter::flush() {
        //将主区域的内容刷新到帧缓冲设备
        //memcpy(this->frameBuf, buf, this->var_info.smem_len);

        uint32_t *p1 = (uint32_t*)this->frameBuf;
        uint32_t *p2 = (uint32_t*)this->a0->getbuf();

        int max_w  = this->var_info.xres;
        int max_h  = this->var_info.yres;
        int maxv_w = this->var_info.xres_virtual;
        int maxv_h = this->var_info.yres_virtual;
            
        for (int x = 0; x < max_w; x++) {
            for (int y = 0; y < max_h; y++) {

                //第一个缓冲区的位置对应
                //第二个缓冲区的位置
                size_t pos1, pos2;
                    
                if (this->rand == false) {

                    pos1 = x + y * maxv_w;
                    pos2 = x + y * max_w;
                        
                } else if (this->rand == true) {

                    pos1 = x + y * maxv_w;
                    //pos2 = y + (max_w - x - 1) * max_h;
		    pos2 = (max_h - y - 1) + x * max_h;
                }
                    
		//开始复制
                p1[pos1] = p2[pos2];
            }
        }
            
    } //刷写

}//qing
