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
        //Use a continuous block of memory space to store a string.
        public:
            //----------------------------------------------------------------
            //Basic string processing

            static long long count(const char* p){
                long long l = 0;  
                while (*p != '\0') {
                    l++;  
                    p++;
                }
                return l;    
            }//count

	    static size_t count_utf8(const char *s) {
	        //Calculate the length of the string in UTF-8 characters
                
		size_t len = 0;
		mbstate_t state = {0};
		//This object can save the converted location information

		while(*s){
		    //Determine the length of the multibyte character sequence
		    size_t ret = mbrlen(s, MB_CUR_MAX, &state);
		    if(ret != (size_t)-2 && ret !=(size_t)-1){
		        len++;
			s += ret;
		    }else{
		        break;
		    }//handle error
		}
		return len;
	    }//count_utf8

            static size_t count_utf8_char(char byte){
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
            //C-Array string about

            static char* get_copy(char* p){
                long long l = string::count(p);
                char* tmp = new char[l + 1];
                tmp[0] = '\0';
                string::copy(tmp, p);
                return tmp;
            }

            static char* get_cat(char* p1, char* p2){
                long long l = string::count(p1) + string::count(p2);
                char* news = new char[l + 1];
                news[0] = '\0';
                string::copy(news,p1);
                string::cat(news,p2);
                return news;
            }

            static char* utf8_to_ansi(char* utf8_string);
	    static wchar_t* utf8_to_wchar(const char*utf8_str);

            //----------------------------------------------------------------
            //C++ style string about
        
            static std::string to_string(double value, int num){
		// Keep a certain number of decimal places
                std::ostringstream stream;
                stream << std::fixed << std::setprecision(num) << value;
                return stream.str();
            }

            static int count_lines(const std::string &s) {
                //Count the newline symbols in s string
                int i = 0;
                for (char c : s) {
                    if (c == '\n') {
                        ++i;
                    }
                }
                return i;
            }


            //----------------------------------------------------------------
            //qing string

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
