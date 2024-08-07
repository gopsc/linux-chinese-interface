
#include "HideCursor.h"

namespace qing{
	
    HideCursor::HideCursor() {
        HideCursor::set();
    }
    
    HideCursor::~HideCursor() {
        HideCursor::back();
    }
        
    void HideCursor::set() {
        std::cout << "\033[?25l" << std::endl;  
    }
    
    void HideCursor::back() {
        std::cout << "\033[?25h" << std::endl;  
    }
}
