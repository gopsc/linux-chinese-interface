#pragma once
#include "../../type.h"
#include <iostream>
#include <exception>

namespace qing{

    class JsonInterface: public Interface{

    public:
	    class JsonParseError: public Error{
		public:
	            JsonParseError(const std::string& msg): Error(std::string("解析Json错误：") + msg){}
	    };
    
    };

}
