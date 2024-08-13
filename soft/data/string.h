//字符串的实现

//==========================
#ifndef __H__soft_data_string
#define __H__soft_data_string
//==========================
//--------------------------
#include <string.h>
#include "../../type.h"

//--------------------------
#ifdef __cplusplus
#include <sstream>
#include <iostream>
#include <iomanip>

//==========================
#ifdef __linux__
#include <iconv.h>
#endif //__linux__
//--------------------------
#ifdef _WIN32
#include <Windows.h>
#endif //_WIN32
//==========================

namespace qing {
	

    class string: Unit{
        //用一块连续的内存空间来存储一个字符串
        public:
            //----------------------------------------------------------------
            //基本字符串处理

            static long long count(const char* p){
		/*输入一个C风格字符串，返回它的有效长度*/
                long long l = 0;  
                while (*p != '\0') {
                    l++;  
                    p++;
                }
                return l;    
            }//count

	    static size_t count_utf8(const char *s) {
	        //计算字符串在UTF-8编码下的字符数
                
		size_t len = 0;
		mbstate_t state = {0};
		//这个对象可以保存转换后的位置信息

		while(*s){
		    //确定多字节字符序列的长度
		    size_t ret = mbrlen(s, MB_CUR_MAX, &state);
		    //处理错误
		    if(ret != (size_t)-2 && ret !=(size_t)-1){
		        len++;
			s += ret;
		    }else{
		        break;
		    }
		}
		return len;
	    }//count_utf8

            static size_t count_utf8_char(char byte){
		//手动计算utf8字符串的字符数量，存在一些边界情况和复杂性
		
	        if ((byte & 0x80) == 0) {  //ASCII char: 0xxxxxxx
		    return 1;
		} else if ((byte & 0xE0) == 0xC0) { // 2 byte char: 110xxxxx
		    return 2;
		} else if ((byte & 0xF0) == 0xE0) { // 3 byte char: 1110xxxx
		    return 3;
		} else if ((byte & 0xF8) == 0xF0) { // 4 byte char: 11110xxx
		    return 4;
		} else throw std::runtime_error(std::string("string::count_utf8_char: illegal character."));
	    }

            static char* copy(char* p, const char* src){
		//将一个C风格字符串的有效内容，复制到另一块连续的内存空间。
                char* tmp = p;
                while (*src != '\0') {
                    *p = *src;  
                    p++;  
                    src++;
                }
                *p = '\0';
                return tmp; 
            }

            static char* cat(char* p, const char* src){
		//将一个C风格的字符串，拼接到另一个的后面
                char* cur = p;
                while (*cur) {  
                    cur++;
                }
                while (*src) {  
                    *cur = *src;  
                    cur++;  
                    src++;  
                }
                *cur = '\0';
                return p; 
            }//concatenation

            static int compare(const char *p1, const char *p2){
		//比较两个C风格字符串是不是相等的
                while (*p1 && (*p1 == *p2)) {
                    p1++;  
                    p2++;  
                }
                return *(const unsigned char*)p1 - *(const unsigned char*)p2;  
            }

            static const char* strstr(const char* m, const char* s){
                return strstr(m, s);
            }

	
            //----------------------------------------------------------------
            //C风格字符串相关

            static char* get_copy(char* p){
		//获取一个存放在堆上的C风格字符串的副本，用完之后要释放。
                long long l = string::count(p);
                char* tmp = new char[l + 1];
                tmp[0] = '\0';
                string::copy(tmp, p);
                return tmp;
            }

            static char* get_cat(char* p1, char* p2){
		//获取一个存放在堆上的字符串拼接结果的副本，用完之后要释放。
                long long l = string::count(p1) + string::count(p2);
                char* news = new char[l + 1];
                news[0] = '\0';
                string::copy(news,p1);
                string::cat(news,p2);
                return news;
            }

	    //将utf8字符串转换为ansi编码，返回一个副本，用完后需要释放
            static char* utf8_to_ansi(char* utf8_string);
	    //将utf8字符串转换为wchar格式，返回一个副本，用完后需要释放
	    static wchar_t* utf8_to_wchar(const char*utf8_str);

            //----------------------------------------------------------------
            //C++风格字符串相关
        
            static std::string to_string(double value, int num){
		//将双精度小数转换为字符串，并且保留一定数量的小数位数
                std::ostringstream stream;
                stream << std::fixed << std::setprecision(num) << value;
                return stream.str();
            }

            static int count_lines(const std::string &s) {
                //计算字符串中的换行符数量
                int i = 0;
                for (char c : s) {
                    if (c == '\n') {
                        ++i;
                    }
                }
                return i;
            }


            //----------------------------------------------------------------
            //qing字符串

            string();
            string(const char *p);
            string(char *p);
            string(const string&);
            ~string();

            char* get() const;

            bool operator==(const string& m) const;
            bool operator==(const char *p) const;
            bool operator!=(const string& m) const;
            bool operator!=(const string*p) const;
	    bool operator!=(const char* p1) const;
            string& operator=(const string& m);
            string& operator=(const char* p);
            string& operator=(char* p);
            string  operator+(const string& m) const;
            string to_ANSI() const;
        private:
            char *data = NULL;
    };


}

#endif //__cplusplus
#endif //!__H__soft_data_string
