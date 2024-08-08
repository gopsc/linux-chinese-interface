#include "CNode.h"

namespace qing{

    CNode::~CNode(){
        if(this->next) delete(this->next);
    }//free


    CNode* CNode::push(){
        //join a blank node
        //it static to be compatible with null pointers
        CNode* m = new CNode;
        if(!m) throw std::runtime_error("Failed to create new momery space.");
        m->set_next(this);
        this->set_prev(m);

        return m;
    }

    CNode* CNode::insert(char c){
        //insert a char to node
        //push a new node if it's full
        CNode* m = (this->length == CNODE_DATA_LENGTH)
                   ? this->push()
                   : this;
        m->insert_data(c);
        return m;
    }

    const CNode* CNode::get_tail() const{
        const CNode* tmp = this;
        while(tmp->next){
            tmp = tmp->get_next();
        }
        return tmp;
    }

}//qing
