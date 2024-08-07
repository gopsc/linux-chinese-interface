#pragma once

#include<stdio.h>
#include"../../type.h"
namespace qing{
	
    class Io:public Unit{

        public:
            static char getc(){
                return getchar();
            }//getc
		    
            static int format(const char* fmt, char* p){
                return printf(fmt, p);
            }//format

    };//Io
	
}//qing
