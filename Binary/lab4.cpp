#include "Tree.h"
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <fstream>


int fill_tree(string adrs, Tree& tr){
    std::ifstream in(adrs);
    float num;

    while (!in.eof()){
        string key, info1="", info2="", s;
        std::getline(in, key);
        std::getline(in, s);
        int i=0, info_number=1;
        while (i<s.size()){
            if (info_number==1) info1+=s[i++];
            else if (info_number==2) info2+=s[i++];
            if (s[i]==','){
                info_number=2;
                i++;
            }
        }
        tr.add(key, info1, info2);
    }
    in.close();
    return 0;
};


void menu(Tree& tr){
    int com, code;
    float num;
    string s, key, gr, info1, info2, a, b;
    gr="input command: 0 - out, 1 - add, 2 - del, 3 - find, 4 - slice, 5 - std::output, 6 - tree output: \n";
    std::cout << gr;
    std::cin >> com;
    while (com){
        if (com==1){
            std::cout << "input key: ";
            std::cin >> key;
            std::cout << "input info1: ";
            std::cin >> info1;
            std::cout << "input info2: ";
            std::cin >> info2;
            code=tr.add(key, info1, info2);
            if (code) std::cout << "can't add: not unique key\n";
        }
        else if (com==2){
            std::cout << "input key: ";
            std::cin >> key;
            code = tr.del(key);
            if (code == 1 ) std::cout << "no such key\n";
        }
        else if (com==3){
            std::cout << "input key: ";
            std::cin >> key;
            try{
                std:: cout << tr.find(key);
            }
            catch(const std::exception& e){
                std::cerr << e.what() << '\n';
            }
        }
        else if (com==4){
            std::cout << "input first edge: ";
            std::cin >> a;
            std::cout << "input second edge: ";
            std::cin >> b;
            Tree tr2(tr.slice(a,b));
            std::cout << tr2;
        }
        else if (com==5){
            std::cout << tr;
        }
        else if (com==6){
            tr.output_tree();
        }
        else std::cout << gr;
        std::cout << "\ninput command: ";
        std::cin >> com;
    }
    
}


int main(){
    Tree tr;
    // fill_tree("input.txt", tr);
    // menu(tr);
    




    return 0;
}