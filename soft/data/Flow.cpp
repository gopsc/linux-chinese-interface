#include "Flow.h"

namespace qing {

    Flow::Flow(const char* p, const int mode) {
        //Obtain data from different source
        //
        //“@qing，20240527：Plan B, Derive different types

        this->LinkedList = new CNode;

        if(mode == Flow::FROM_ARRAY){
            // From array
            CNode* m = NULL;

            while('\0' != *p && (m = LinkedList->insert(*(p++))) != NULL)
                this->LinkedList = m;
            if (*p != '\0') {
                delete (this->LinkedList);
                throw std::runtime_error("Failed to insert to Flow.");
            }

        } else if (mode == Flow::FROM_INPUT){
            // From input
            //
            //@qing，20240526：Line break using backslash

            CNode* m = (CNode*)this;
            char c;
            bool newline = false;

            while(!('\n' == (c = Io::getc()) && newline == false)){

                if (!(('\n' == c && newline == true) || (newline = ('\\' == c))) && (m = LinkedList->insert(c)))
                    this->LinkedList = m;
                if (m == NULL) break;

            }

            if ('\n' != c) {
                delete this->LinkedList;
                throw std::runtime_error("Failed to insert Flow.");
            }

        } else if (mode == Flow::FROM_FILE){
            //From file

            FILE* f = FilesystemOp::Openfile((char*)p, "r");
            if(!f) throw std::runtime_error("Failed to open file.");

            CNode* m;
            char c;

            while((c = FilesystemOp::Readchar(f)) != (char)EOF && (m = LinkedList->insert(c)))
                this->LinkedList = m;

            FilesystemOp::Closefile(f);

            if(c != (char)EOF){
                delete this->LinkedList;
                throw std::runtime_error("Failed to insert Flow.");
            }

        } //Different mode

    }//constructor

    Flow::~Flow(){
        delete this->LinkedList;
    }//Destructor

        

    char* Flow::to_array() const{

        char *array = new char[1];
        *array = '\0';

        const CNode* m = LinkedList->get_tail();
        while(m){
            long long l = string::count(array);
            long long nl = m->get_length();
            char *tmp;

            try{
		//Free memory space when err
                tmp = new char[l + 1 + nl];
            } catch (const std::bad_alloc& e) {
                delete[] array;
                throw e;
            }

            int i;

            for(i=0; i<l; ++i){
                tmp[i] = array[i];
            }//Copy copied

            for(i=0; i<nl; ++i){
                tmp[l+i] = m->get_data(i);
            }//Copy new node data

            tmp[l+i] = '\0';

            char *tmp1 = array;
            array = tmp;
            delete tmp1;
            //exchange an release

            m = m->get_prev();
        }//traverse and write

        return array;
    }//to_array


    void Flow::to_file(char* path) const{

        FILE *f = FilesystemOp::Openfile(path, "w");
        if(!f) throw std::runtime_error("Failed to open file.");

        const CNode* tmp = LinkedList->get_tail();
        while(tmp){
            long long l = tmp->get_length();
            for(int i=0; i<l; ++i){
                FilesystemOp::Writechar(tmp->get_data(i), f);
            }//traverse and write

            tmp = tmp->get_prev();
        }//traverse in reverse order

        FilesystemOp::Closefile(f);

    }//to_file


    string Flow::to_string() const{
        
        char *s = this->to_array();
        return string(s);
        //Just save the pointer
    }

}
