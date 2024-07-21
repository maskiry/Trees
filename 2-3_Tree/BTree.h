#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#include <stdexcept>
using std::string;


#ifndef BTree_h
#define BTree_h 1

class BTree;
class MyIter;

template<class T>
void swap(T (&a), T (&b)){
    T tmp;
    tmp=a;
    a=b;
    b=tmp;
}


struct Information{
    string info;
    Information* next;
    Information(string info): info(info), next(nullptr){};
};

struct InfoField{
    string key;
    Information* info;
    InfoField(string key, string new_info): key(key){
        info = new Information(new_info);
    };
    ~InfoField(){
        Information *next = info, *cur;
        while (next){
            cur=next;
            next=cur->next;
            delete cur;
        }
    }
    bool operator == (const InfoField& rh){
        return key==rh.key;
    }
    bool operator > (const InfoField& rh){
        return key>rh.key;
    }
    bool operator < (const InfoField& rh){
        return key<rh.key;
    }
    bool operator == (const string& rh){
        return key==rh;
    }
    bool operator > (const string& rh){
        return key>rh;
    }
    bool operator >= (const string& rh){
        return key>=rh;
    }
    bool operator < (const string& rh){
        return key<rh;
    }
    bool operator <= (const string& rh){
        return key<=rh;
    }
    void add(string information){
        Information* new_info = new Information(information);
        new_info->next = info;
        info = new_info;
    }
};

std::ostream& operator << (std::ostream& out, InfoField& inf);


class Node{
    friend BTree;
    friend MyIter;
    private:
        InfoField* inform[2];
        Node* childs[3];
        Node* parent;
        Node();
        ~Node();
        InfoField* chooseMedian(InfoField* inf_new);
        InfoField* addToLeaf(InfoField* new_info);
        InfoField* add(string key, string new_info);
        Node* splitNode(int mode);

        void printNode();
        Node* redistributeChilds(int branch);
        void del(int idx);
};

class BTree{
    private:
        Node* head;
        int size;
        Node* find(string key);
        Node* swapWithEquiv(Node* cur, int idx);
        Node* getNext(Node* cur, int &idx);
        Node* getPrev(Node* cur, int &idx);
        Node* mergeNodes(Node* empty, int branch);
    public:
        BTree();
        ~BTree();
        void add(string key, string info);
        void del(string key);
        void treeTraversal(void (Node*, int, int));
        void printTree();
        void printInverseSlice(string a, string b);
        std::vector<string*> getGreater(string key);

        std::vector<string*> operator [] (string key){
            Node* n = find(key);
            std::vector<string*> ans;
            if (*(n->inform[0])==key){
                Information *next = n->inform[0]->info;
                while (next){
                    ans.push_back(&(next->info));
                    next=next->next;
                }
                return ans;
            }
            if (n->inform[1] && *(n->inform[1])==key){
                Information *next = n->inform[1]->info;
                while (next){
                    ans.push_back(&(next->info));
                    next=next->next;
                }
                return ans;
            }
            string error_msg = "no such key: \"" + key + "\"";
            throw std::out_of_range(error_msg);
        }

};



#endif