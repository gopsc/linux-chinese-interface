#pragma once
#ifdef __cplusplus
#include <iostream>
#include <exception>
namespace qing {
    class Unit {};
    class Declare {};
    class Interface {};
    class Error: public std::exception {
        public:
	    Error(): m_message("LCI代码部分出了问题。"){}
	    Error(const std::string& message): m_message(message) {}

	    const char* what() const noexcept override {
	        return m_message.c_str();
	    }
	private:
	    std::string m_message;
    };
}
#endif //__cplusplus
