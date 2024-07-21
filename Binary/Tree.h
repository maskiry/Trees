#include "vector"
#include "string"
#include <stdio.h>
#include <iostream>


#ifndef Tree_h
#define Tree_h 1

using std::string;

class Tree;
class MyIter;

struct Information{
    string info1;
    string info2;
    Information(): info1(""), info2(""){};
    Information(string& info1, string& info2): info1(info1), info2(info2) {};
};

std::ostream& operator << (std::ostream& out, Information& inf);

class Node{
    friend Tree;
    friend MyIter;
    private:
    // public:
        string key;
        Information *info;
        Node* parent;
        Node* left;
        Node* right;
        Node* next;
        Node* prev;
        void* operator new(std::size_t size) {return ::operator new(size);}
        void operator delete(void* ptr) {::operator delete(ptr);}
        void* operator new[](std::size_t size) {return ::operator new[](size);}
        void operator delete[](void* ptr) {::operator delete[](ptr);}
        Node(): key(""), info(nullptr), parent(nullptr), left(nullptr), right(nullptr),
                next(nullptr), prev(nullptr){};
        Node(string key, string& info1, string& info2,
             Node* par, Node* n, Node* p): 
             key(key), 
             left(nullptr), 
             right(nullptr), 
             parent(par), 
             next(n), 
             prev(p){
                info = new Information(info1, info2);
        };
        ~Node(){
            delete info;
        }
};

class MyIter{
    private:
        Node* nd;
        bool finish;
    public:
        MyIter(): nd(nullptr), finish(true){};
        MyIter(Node* nd): nd(nd){
            if (nd) finish=false;
            else finish=true;
        };
        MyIter(const MyIter& other){
            nd=other.nd;
            finish=other.finish;
        }
        MyIter(MyIter&& other){
            nd=other.nd;
            finish=other.finish;
        }
        void next(){
            if (nd) nd=nd->next;
            if (!nd) finish=true;
        };
        bool is_finish(){
            return finish;
        };
        string get_key(){
            return nd->key;
        }
        Information& get_info(){
            return *nd->info;
        }

        MyIter& operator = (const MyIter& other){
            if (this!=&other){
                nd=other.nd;
                finish=other.finish;
            }
            return *this;
        }
        MyIter& operator = (MyIter&& other){
            if (this!=&other){
                nd=other.nd;
                other.nd=nullptr;
                finish=other.finish;
                other.finish=true;
            }
            return *this;
        }

        MyIter& operator = (Node* other){
            nd=other;
            if (nd) finish=false;
            else finish=true;
            return *this;
        }
};


class Tree{
    private:
        Node* head;
        Node* first;
        int size;
        MyIter *iter;
    public:
        Tree();
        Tree(const Tree& other);
        Tree(Tree&& other);
        ~Tree();
        int add(string key, string info1, string info2);
        int add(Node* nd);
        int del(string key);
        Information& find(string& key);
        Tree slice(string& key1, string& key2);
        MyIter& get_iter();
        void output_tree();

        Tree& operator = (const Tree& other){
            if (this!=&other){
                Node *rem=first, *next;
                while (rem){
                    next = rem->next;
                    delete rem;
                    rem=next;
                }
                iter=nullptr;

                MyIter* it = other.iter;
                string key=head->key, inf1 = head->info->info1, inf2 = head->info->info2;
                add(key, inf1, inf2);
                while (!it->is_finish()){
                    key=it->get_key();
                    inf1 = it->get_info().info1;
                    inf2 = it->get_info().info2;
                    add(key, inf1, inf2);
                    it->next();
                }
            }
            return *this;
        }

        Tree& operator = (Tree&& other){
            if (this!=&other){
                Node *rem=first, *next;
                while (rem){
                    next = rem->next;
                    delete rem;
                    rem=next;
                }
                delete iter;

                head = other.head;
                other.head = nullptr;
                first = other.first;
                other.first = nullptr;
                size = other.size;
                iter = other.iter;
                other.iter = nullptr;
            }
            return *this;
        }
};

std::ostream& operator << (std::ostream& out, Tree& tr);



#endif