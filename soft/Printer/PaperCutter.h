#pragma once

#include <cstddef>
#include <iostream>
#include "../../type.h"
#include "../data/string.h"

namespace qing {
	
    class PaperCutter: public Interface {
		
        public:
            /*在目标字符串中插入换行符，使得目标字符串符合一定的最大宽度。  
            仅适用于汉字的整理，且在这里汉字和英文字母是一样大小*/
            static std::string InsertLineBreak(std::string s, int w);
            /*从目标字符串尾部截取字符串，使得目标字符串符合一定的最大行数。*/
            static std::string CutLines(std::string s, int h);
		
    }; //裁纸机
	
}//qing
