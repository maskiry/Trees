#include "BTree.h"
#include <iomanip>
#include <fstream>
#include <ctime> 
using std::string;

bool checkWordEnd(char c){
    int n_seps=7;
    char seps[n_seps]{' ', ',', '.', '!', '?', '\0', '\n'};
    for (int j=0;j<n_seps;j++){
        if (c==seps[j]){
            return true;
        }
    }
    return false;
}

void wordSearching(string adrs, BTree& tr){
    std::ifstream in(adrs);
    int x=0,y=0;
    float num;
    int i=0;
    string word="", s, info="";
    bool is_word=false;
    while (!in.eof()){
        is_word=false;
        std::getline(in, s);
        i=0;
        x=0;
        while (i<s.size()){
            if (!checkWordEnd(s[i])){
                if (!is_word){
                    x=i;
                    is_word=true;
                }
                word+=s[i];
            }
            else {
                if (is_word){
                    is_word=false;
                    info = std::to_string(x) + ";" + std::to_string(y);
                    tr.add(word, info);
                    word="";
                }
            }
            i++;
        }
        if (is_word){
            is_word=false;
            info = std::to_string(x) + ";" + std::to_string(y);
            tr.add(word, info);
            word="";
        }
        y++;
    }
    in.close();
}


int fill_tree(string adrs, BTree& tr){
    std::ifstream in(adrs);
    float num;
    int i=0;
    while (!in.eof()){
        string key, info1="", info2="", s;
        std::getline(in, key);
        std::getline(in, s);
        tr.add(key, s);
        i++;
    }
    in.close();
    return 0;
};

void checkTime(BTree& tr){
    float add_time, del_time;
    int N;
    std::cout << "intput N: ";
    std::cin >> N;
    string s = "1000000", inf="abc";

    add_time=clock();
    for (int i =0; i<N; i++){
        tr.add(s+std::to_string(i), inf);
    }
    add_time=(clock()-add_time)/N;

    tr.printTree();

    del_time=clock();
    for (int i =0; i<N; i++){
        tr.del(s+std::to_string(i));
    }
    del_time=(clock()-del_time)/N;

    std::cout << "add time = " << add_time << "\ndel time = " << del_time << "\n";
}


void menu(BTree& tr){
    int com, code;
    float num;
    string s, key, gr, info1, a, b;
    gr="input command: 0 - out, 1 - add, 2 - del, 3 - find, 4 - inverse slice,\n\
5 - get next, 6 - tree output, 7 - check time, 8 - count words in file: \n";
    std::cout << gr;
    std::cin >> com;
    while (com){
        if (com==1){
            std::cout << "input key: ";
            std::cin >> key;
            std::cout << "input info1: ";
            std::cin >> info1;
            tr.add(key, info1);
        }
        else if (com==2){
            std::cout << "input key: ";
            std::cin >> key;
            try{
                tr.del(key);
                
            }catch(const std::exception& e){
                std::cerr << e.what() << '\n';
            }
            
        }
        else if (com==3){
            std::cout << "input key: ";
            std::cin >> key;
            try{
                std::vector<string*> f = tr[key];
                for (int i = 0; i<f.size(); i++) std:: cout << *f[i] << " ";
                std::cout << "\n";
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
            try{
                tr.printInverseSlice(a,b);
            }
            catch(const std::exception& e){
                std::cerr << e.what() << '\n';
            }
        }
        else if (com==5){
            std::cout << "input key: ";
            std::cin >> key;
            try{
                std::vector<string*> f = tr.getGreater(key);
                for (int i = 0; i<f.size(); i++) std:: cout << *f[i] << " ";
                std::cout << "\n";
            }
            catch(const std::exception& e){
                std::cerr << e.what() << '\n';
            }
            
        }
        else if (com==6){
            tr.printTree();
        }
        else if (com==7){
            checkTime(tr);
        }
        else if (com==8){
            wordSearching("file.txt", tr);
        }
        else std::cout << gr;
        std::cout << "\ninput command: ";
        std::cin >> com;
    }
}


int main(){
    BTree tr;
    fill_tree("input.txt", tr);
    tr.printTree();
    menu(tr);



    return 0;
}