#include "Area.h"

/**/
static std::mutex lock;

namespace qing{

    Area::Area(size_t w, size_t h) {

        this->w = w;
        this->h = h;

	//this->buf = (void*)new uint32_t[w * h];
	this->buf = malloc(w * h * sizeof(uint32_t));
	if(!this->buf)//失败
            throw std::runtime_error(std::string("创建堆内存失败。") + strerror(errno));
    }//构造函数
    
    Area::Area(std::string path){
        /* 根据图像地址创建CImg类 */
        cimg_library::CImg<unsigned char> inImage(path.c_str());
        //取长
        this->w = inImage.width();
        this->h	= inImage.height();
	/* 很据宽和高创建缓冲区 */
	//this->buf = (void*)new uint32_t[this->w * this->h];
	this->buf = malloc(w * h * sizeof(uint32_t));
	if(!this->buf)//失败
            throw std::runtime_error(std::string("创建堆内存失败。") + strerror(errno));
	/*将图像复制到区域中*/
	for(int w = 0; w < this->w; ++w){
	    for(int h = 0; h < this->h; ++h){
	        //uint32_t color = 255 << 24; //没有效果
		uint32_t color = 0;
		color += inImage(w,h,0) << 16;
	        color += inImage(w,h,1) << 8;
	        color += inImage(w,h,2) << 0;
	        uint32_t* ptr = (uint32_t*)this->buf;
	        ptr[h*this->w + w] = color;
            }/*遍历高*/
        }/*遍历宽*/
    }//构造函数
    
    Area::~Area() {
        /* 用原始的方式释放分配的内存 */
        //if(this->buf != NULL) delete[] (uint32_t*)buf;
	if(this->buf) free(this->buf);
    } //析构函数
    
    
    size_t Area::getw() {
        return this->w;
    }//访问器
	
    size_t Area::geth() {
        return this->h;
    } //访问器
	
    void* Area::getbuf() {
        return this->buf;
    } //访问器
	
	
    void Area::paint(uint32_t color) {
        //将整个区域设置成一种颜色
		
        //获取缓冲区的指针，并转换为像素类型
        uint32_t *p1 = (uint32_t*)buf;
            
        //执行绘制
        for (size_t y=0; y<h; y++) {
            for (size_t x=0; x<w; x++) {
                *p1++ = color;
            }//h
        }//w
	    
	    
    } //paint
    
        
    void Area::pa(Area *a1, size_t x, size_t y, size_t fw, uint32_t color) {
        // 将窗体完整地画到画布上，需要输入一个窗体类型的指针，
        // 以及窗体的位置信息、框体颜色
	//
	// fw: frame_width。边框的宽度

        //取此区域的宽和高
        size_t w1 = this->w;
        size_t h1 = this->h;

        //获取这个窗体的长和宽
        size_t w3 = a1->getw();
        size_t h3 = a1->geth();
	    
        //获取本画布的缓冲区指针以及输入窗体的缓冲区指针
        uint32_t *buf1 = (uint32_t*)this->buf;
        uint32_t *buf2 = (uint32_t*)a1->getbuf();
	    
        //执行窗口绘制
        //遍历这个窗口的位置
        for (size_t j=0; j<h3; j++) {
            //遍历区域的高度
            for (size_t i=0; i<w3; i++) {
                //遍历区域的宽度
                        
                //计算像素位置
                size_t pos1 = (j+y)*w1+(i+x);
                size_t pos2 = j*w3+i;
                        
                //执行复制
                buf1[pos1] = buf2[pos2];
                            
                if (( i < fw || i > w3 - 1 - fw ) || ( j < fw || j > h3 - 1 - fw )) {
                    buf1[pos1] = color;
                } //绘制窗体的边框，向内部延申
                        

            }//w
        }//h

	
    }//绘制
	
	
    void Area::print(char *p, size_t cw, size_t ch, uint32_t color) {
        //在窗体上打印字符
	//
	//@qing:使其返回下一个字符的位置

        lock.lock();


        //将缓冲区指针转换为像素格式
        uint32_t *p1 = (uint32_t*)this->buf;
        //盒子，用于储存将要打印的字符
        //在这里限定中文字符（一个字符2字节）
        //应该选择utf8最大的字符数量
        unsigned char box[3] = {0};

        //开始的列数
        size_t x_start = 0;
        //开始的行数
        size_t y_start = 0;
        //位置的漂移量
        size_t drifting = 0;
	//位置的修正量
        size_t correction = 0;

        //---------------------------------------------

        //此字所在行
        size_t row = y_start;
        //此字所在列
        size_t col = x_start;

		
        size_t l1 = strlen(p);
        for(size_t k = 0; k < l1;) {
            //遍历每个字
			
            //向盒子中装填此字符
            box[0] = p[k];
            box[1] = p[k + 1];
            box[2] = '\0';

            //判断此字符是否非单字节
            //这里应该使用utf8工具类判断一下这个字符的长度
            //然后遍历计数器向后累加
            if ((unsigned char)box[0]>128) {
                k += 2;
            } else {
                box[1] = '\0';
                k++;
            }

            //检测到换行符，列数归零，行数加一
            if (box[0] == '\n') {
                row++;
                col = x_start;
                continue;
            }

            //清理印刷字体的缓冲区
            blank_img();
            //向文字专用缓冲区中画一个字
            drawvfont(0, 0, *(unsigned short*)box, cw, ch);

            //将文字从文专缓冲区复制到窗口缓冲区
            for (size_t j = 0; j < cw; ++j) {
                for (size_t i = 0; i < ch; ++i) {
                    size_t pos = (col * cw + row * ch * w) + ((j+drifting+correction) * w + i +drifting);
                    if (image[i][j] == '*') p1[pos] = color;
                }//ch
            }//cw
            col++;
        }


	lock.unlock();

        }//打印

    }//qing
