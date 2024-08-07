
#include "PrimaryTerminal.h"

namespace qing{
	
    PrimaryTerminal::PrimaryTerminal() {
        this->old = PrimaryTerminal::set_and_return(STDIN_FILENO);
    }
    
    PrimaryTerminal::~PrimaryTerminal() {
        PrimaryTerminal::back_from_configure(STDIN_FILENO, this->old);
    }

        
    struct termios PrimaryTerminal::set_and_return(int identifier) {  
        struct termios oldc, newc;  
        tcgetattr(identifier, &oldc);
        newc = oldc; 
        newc.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(identifier, TCSANOW, &newc);
        return oldc;
    } 
        
    void PrimaryTerminal::back_from_configure(int identifier, struct termios &oldc) {  
        tcsetattr(identifier, TCSANOW, &oldc);
    }
}
