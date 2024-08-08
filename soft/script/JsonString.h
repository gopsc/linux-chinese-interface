#pragma once
#include<mutex>
#include <stdexcept>
#include"../../cJSON-master/cJSON.h"
#include"../../type.h"
namespace qing{
    class JsonString: public Unit{
    public:
        JsonString(){
            this->root = cJSON_CreateObject();
        }
        JsonString(std::string s){
            this->root = cJSON_Parse(s.c_str());
            if (!this->root) throw std::runtime_error(std::string("解析JSON字符串。"));
        }
        ~JsonString(){
            if(this->root) cJSON_Delete(this->root);
            //if(this->root) free(this->root);
        }
        void on_lk(){
            lk.lock();
        }
        void unlk(){
            lk.unlock();
        }
        cJSON *get(){
            return root;
        }
        std::string print(){
            char *json_str = cJSON_Print(this->root);
            std::string s = std::string(json_str);
            free(json_str);            
            return s;
        }
    private:
        std::mutex lk;
        cJSON *root;
    };
}
