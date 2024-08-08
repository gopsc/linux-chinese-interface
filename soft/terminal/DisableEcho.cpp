
#include "DisableEcho.h"
namespace qing{
	
        DisableEcho::DisableEcho() {
            DisableEcho::set();
        }
	
        DisableEcho::~DisableEcho() {
            DisableEcho::back();
        }
	
        void DisableEcho::set() {  
            system("stty -icanon -echo");
        }
        
        void DisableEcho::back() {  
            system("stty icanon echo");  
        }  
}
