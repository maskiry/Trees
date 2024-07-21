#include "Tree.h"


Tree::Tree(): size(0){
    head=nullptr;
    first=head;
    iter = new MyIter(first);
}

Tree::Tree(const Tree& other){
    head=nullptr;
    first=head;
    iter = new MyIter(first);

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

Tree::Tree(Tree&& other){
    head = other.head;
    other.head = nullptr;
    first = other.first;
    other.first = nullptr;
    size = other.size;
    iter = other.iter;
    other.iter = nullptr;
}

Tree::~Tree(){
    Node *rem=first, *next;
    while (rem){
        next = rem->next;
        delete rem;
        rem=next;
    }
    delete iter;
}

Information& Tree::find(string& key){
    Information* ans=nullptr;
    Node* cur=head;
    while (cur){
        if (cur->key==key){
            ans = cur->info;
            break;
        }
        else if (cur->key<key)
            cur = cur->right;
        else cur = cur->left;
    }
    if (!ans) throw std::out_of_range("No such element");
    return *ans;
}

Tree Tree::slice(string& key1, string& key2){
    Tree slice_tree;
    if (!first) return slice_tree;
    Node* cur = first;
    while (cur->key<key1){
        if (cur->next) cur = cur->next;
        else return slice_tree;
    }
    while (cur->key>=key1 && cur->key<key2){
        string key=cur->key, inf1 = cur->info->info1, inf2 = cur->info->info2;
        slice_tree.add(key, inf1, inf2);
        if (cur->next) cur = cur->next;
        else return slice_tree;
    }
    return slice_tree;
}

int Tree::add(string key, string info1, string info2){
    Node *new_node = new Node(key, info1, info2, nullptr, nullptr, nullptr);
    int code = add(new_node);
    if (code) delete new_node;
    else size++;
    return code;
}

int Tree::add(Node* new_node){
    Node *cur=head, *next=head, *p=nullptr, *n=nullptr;
    int branch=1;
    while (next){
        cur=next;
        if (cur->key==new_node->key){
            return 1; //not unique element
        }
        else if (cur->key>new_node->key){
            next = cur->left;
            branch=1;
        }
        else{
            next = cur->right;
            branch=2;
        }
    }

    new_node->parent=cur;

    if (branch==1){
        n=cur;
        p=cur;
        if (cur==first) {
            first=new_node;
            p=nullptr;
        }
        while (p){
            if (p->parent && p->parent->right==p){
                p=p->parent;
                break;
            }
            else if (!p->parent) break;
            p=p->parent;
        }
        if (cur) cur->left=new_node;
    }
    else if (branch==2){
        p=cur;
        n=cur;
        while (n){
            if (n->parent && n->parent->left==n){
                n=n->parent;
                break;
            }
            n=n->parent;
        }
        cur->right=new_node;
    }

    new_node->next=n;
    new_node->prev=p;
    if (n) n->prev=new_node;
    if (p) p->next=new_node;

    if (!head) head=new_node;

    return 0;
}

int Tree::del(string key){
    Node *rem=head, *next=head, *p, *n;
    int branch=0;
    while (next){
        rem=next;
        if (rem->key==key){
            break;
        }
        else if (rem->key<key){
            next = rem->right;
            branch=2;
        }
        else{
            next = rem->left;
            branch=1;
        }
    }
    if (!next){
        return 1; //no such key
    }

    if (rem==first){
        first=rem->next;
    }

    if (!rem->left && !rem->right){
        if (branch==0) head=nullptr;
        if (branch==1) rem->parent->left=nullptr;
        else if (branch==2) rem->parent->right=nullptr;

        if (rem->prev) rem->prev->next=rem->next;
        if (rem->next) rem->next->prev=rem->prev;
    }
    else if (rem->left && rem->right){
        Node* replace_node = rem->next;
        if (branch==0) head=replace_node;
        else if (branch==1) rem->parent->left=replace_node;
        else if (branch==2) rem->parent->right=replace_node;
        if (replace_node!=rem->right){
            if (replace_node->parent->left==replace_node) replace_node->parent->left=replace_node->right;
            else if (replace_node->parent->right==replace_node) replace_node->parent->right=replace_node->right;
            if (replace_node->right) {
                replace_node->right->parent=replace_node->parent;
            }
            replace_node->right=rem->right;
            rem->right->parent=replace_node;
        }
        replace_node->parent = rem->parent;
        replace_node->left=rem->left;
        rem->left->parent=replace_node;

        rem->prev->next = replace_node;
        rem->next->prev=rem->prev;
    }
    else {
        Node* replace_node;
        if (rem->left) {
            replace_node = rem->left;//==rem->prev
        }
        else if (rem->right) {
            replace_node = rem->right;//==rem->next
        }
        if (branch==0) head=replace_node;
        else if (branch==1) rem->parent->left=replace_node;
        else if (branch==2) rem->parent->right=replace_node;
        replace_node->parent=rem->parent;

        if (rem->prev) rem->prev->next = rem->next;
        if (rem->next) rem->next->prev = rem->prev;
    }


    delete rem;
    size--;

    return 0;
}

MyIter& Tree::get_iter(){
    *iter=first;
    return *iter;
}

void Tree::output_tree(){
    Node* cur = head;
    int next_branch = 1, depth=0;
    int* seps = new int[size];
    for (int i=0; i<size; i++){
        seps[i]=1;
    }

    while (cur){
        if (next_branch==0 || (!cur->right && !cur->left) || (next_branch==2 && !cur->right)){
            if (cur->parent==nullptr){
                break;
            }
            if (next_branch==1){
                for (int i=0; i<depth-1; i++){
                    if (seps[i]) std::cout << "|";
                    else std::cout << " ";
                    std::cout << "  ";
                }
                if (depth>0) std::cout << "|->" << cur->key << ": " << cur->info->info1 << "; " << cur->info->info2 << "\n";
                else std::cout << cur->key << ": " << cur->info->info1 << "; " << cur->info->info2 << "\n";
            }
            if (cur->parent->left==cur) next_branch=2;
            else if (cur->parent->right==cur) next_branch=0;
            cur = cur->parent;
            depth--;
        }
        else{
            if (next_branch==1){
                for (int i=0; i<depth-1; i++){
                    if (seps[i]) std::cout << "|";
                    else std::cout << " ";
                    std::cout << "  ";
                }
                if (depth>0) std::cout << "|->" << cur->key << ": " << cur->info->info1 << "; " << cur->info->info2 << "\n";
                else std::cout << cur->key << ": " << cur->info->info1 << "; " << cur->info->info2 << "\n";

                if (cur->left){
                    next_branch=1;
                    cur=cur->left;
                }
                else if (cur->right){
                    next_branch=1;
                    cur=cur->right;
                }
                seps[depth]=1;
            }
            else if (next_branch==2){
                if (cur->right){
                    next_branch=1;
                    cur=cur->right;
                }
                seps[depth]=0;
            }
            depth++;
        }


    }
    
}

std::ostream& operator << (std::ostream& out, Information& inf){
    out << "info1 = " << inf.info1 << "; info2 = " << inf.info2 << std::endl;
    return out;
}

std::ostream& operator << (std::ostream& out, Tree& tr){
    MyIter it=tr.get_iter();
    while (!it.is_finish()){
        out << it.get_key() << ": " << it.get_info().info1 << 
                               "; " << it.get_info().info2 << "\n";
        it.next();
    }

    return out;
};

