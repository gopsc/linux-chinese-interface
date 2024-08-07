#ifndef __H__soft_data_CNode
#define __H__soft_data_CNode

#include"../../type.h"
#define CNODE_DATA_LENGTH 2

#ifdef __cplusplus
#include<stdexcept>

namespace qing{

    class CNode: public Unit{
        //A node under Flow
        //Save string as linked list
        private:
            //data field
            char data[CNODE_DATA_LENGTH] = {0};
            int length = 0;
            //pointer field
            CNode* next = NULL;
            CNode* prev = NULL;

        public:
            ~CNode();
            //---------------------------------
            //accessor

            CNode* get_prev() const{
                return this->prev;
            }

            CNode* get_next() const{
                return this->next;
            }

            char get_data(int idx) const{
                return this->data[idx];
            }

            int get_length() const{
                return this->length;
            }


            CNode* set_prev(CNode* m){
                return this->prev = m;
            }

            CNode* set_next(CNode* m){
                return this->next = m;
            }

            char insert_data(const char 字){
                return this->data[this->length++] = 字;
            }

            const CNode* get_tail() const;
	    
            //---------------------------------
            CNode* push();
            //push a blank node
            //is't static to compatible with null pointers

            CNode* insert(char c);
            //Insert a char to node

    };//CNode
    
}//qing

#endif //__cplusplus
#endif //!__H__soft_data_CNode
