#ifndef __H__soft_data_Flow
#define __H__soft_data_Flow

#include"../../type.h"
#include"../system-call/Io.h"
#include"../system-call/FilesystemOp.h"
#include"string.h"
#include"CNode.h"

#ifdef __cplusplus
#include<iostream>

namespace qing{

    class Flow :public Unit{
        public:
            static const int FROM_ARRAY = 0, FROM_INPUT = 1, FROM_FILE = 2;
            //Attribute, used to identify the source from the constructor

            Flow(const char* p, const int mode);
            Flow(Flow&) = delete;
            ~Flow();
            char* to_array() const;
            void to_file(char *path) const;
            string to_string() const;
        private:
            CNode* LinkedList = NULL;
    };//FLow

}//qing

#endif //__cplusplus
#endif //!__H__soft_data_FLow
