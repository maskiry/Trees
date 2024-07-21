#include "BTree.h"

std::ostream& operator << (std::ostream& out, InfoField& inf){
    out << inf.key << ": ";
    Information* n=inf.info;
    while (n){
        out << n->info << " ";
        n=n->next;
    }
    out << "\n";
    return out;
};


Node::Node(){
    inform[0]=nullptr;
    inform[1]=nullptr;
    childs[0]=nullptr;childs[1]=nullptr;childs[2]=nullptr;
    parent=nullptr;
}

Node::~Node(){
    delete inform[0];
    delete inform[1];
}

InfoField* Node::chooseMedian(InfoField* inf_new){
    InfoField *med = nullptr;
    if (*inf_new<*inform[0]){
        med = inform[0];
        inform[0]=inf_new;
    }
    else if (*inf_new<*inform[1]){
        med = inf_new;
    }
    else {
        med = inform[1];
        inform[1] = inf_new;
    }
    return med;
}

Node* Node::splitNode(int mode=2){
    if (mode==-1) throw std::invalid_argument("func Node.splitNode(int): mode must be 0, 1 or 2\n");
    Node* split = new Node();
    swap(split->inform[0], inform[1]);
    swap(split->childs[(int) (mode!=2)], childs[2]);
    if (split->childs[(int) (mode!=2)]) split->childs[(int) (mode!=2)]->parent=split;
    if (mode==0) {
        swap(split->childs[0], childs[1]);
        if (split->childs[0]) split->childs[0]->parent=split;
    }
    return split;
}

InfoField* Node::addToLeaf(InfoField* new_info){
    if (inform[1]==nullptr){
        if (*new_info>*inform[0]){
            inform[1]=new_info;
        }
        else{
            inform[1]=inform[0];
            inform[0]=new_info;
        }
        return nullptr;
    }
    else{
        return chooseMedian(new_info);
    }
    throw std::runtime_error("func node.addToleaf: reached unreachable moment(\n");
}

InfoField* Node::add(string key, string new_info){
    if (inform[0] && *(inform[0])==key){
        inform[0]->add(new_info);
        return nullptr;
    }
    if (inform[1] && *(inform[1])==key){
        inform[1]->add(new_info);
        return nullptr;
    }
    if (childs[0]){
        throw std::runtime_error("func Node.add(string,string): add with new key must be called only on leaf\n");
    }

    InfoField *new_elem = new InfoField(key, new_info);
    return addToLeaf(new_elem);
}

void Node::printNode(){
    if (inform[1])
    std::cout << "addres: " << this << "\n   key1 = " << inform[0]->key << "\n   key2 = " << inform[1]->key 
        << "\n   left branch: " << childs[0] << "\n   mid branch: " << childs[1] << "\n   right branch: " << childs[2]
        << "\n   parent: " << parent << "\n";
    else
    std::cout << "addres: " << this << "\n   key1 = " << inform[0]->key 
        << "\n   left branch: " << childs[0] << "\n   mid branch: " << childs[1]
        << "\n   parent: " << parent << "\n";
};

Node* Node::redistributeChilds(int branch){
    if (!childs[0]) throw std::runtime_error("func Node.redistributeChilds: can't be called on leafs");
    if (branch==0){
        swap(inform[0], childs[0]->inform[0]);
        if (!childs[2] && !childs[1]->inform[1]) return this;
        swap(inform[0], childs[1]->inform[0]);
        if (childs[1]->inform[1]) {
            swap(childs[1]->inform[0], childs[1]->inform[1]);
            return nullptr;
        }
        swap(childs[1]->inform[0], inform[1]);
        if (childs[2]->inform[1]){
            swap(inform[1], childs[2]->inform[0]);
            swap(childs[2]->inform[0], childs[2]->inform[1]);
            return nullptr;
        }
        swap(childs[2]->inform[0], childs[1]->inform[1]);
        delete childs[2];
        childs[2]=nullptr;
        return nullptr;
    }
    if (branch==1){
        if (childs[0]->inform[1]){
            swap(inform[0], childs[1]->inform[0]);
            swap(inform[0], childs[0]->inform[1]);
            return nullptr;
        }
        if (childs[2]){
            swap(childs[1]->inform[0], inform[1]);
            if (childs[2]->inform[1]){
                swap(inform[1], childs[2]->inform[0]);
                swap(childs[2]->inform[0], childs[2]->inform[1]);
                return nullptr;
            }
            swap(childs[2]->inform[0], childs[1]->inform[1]);
            delete childs[2];
            childs[2]=nullptr;
            return nullptr;
        }
        swap(inform[0], childs[1]->inform[0]);
        return this;
    }
    if (branch==2){
        if (childs[1]->inform[1]){
            swap(inform[1], childs[2]->inform[0]);
            swap(inform[1], childs[1]->inform[1]);
            return nullptr;
        }
        if (childs[0]->inform[1]){
            swap(inform[1], childs[2]->inform[0]);
            swap(inform[1], childs[1]->inform[0]);
            swap(inform[0], childs[1]->inform[0]);
            swap(inform[0], childs[0]->inform[1]);
            return nullptr;
        }
        swap(inform[1], childs[1]->inform[1]);
        delete childs[2];
        childs[2]=nullptr;
        return nullptr;
    }
    string error_msg = "func Node.redistributeChilds: expected branch=0,1 or 2; got: \"" + std::to_string( branch) + "\"\n";
    throw std::invalid_argument(error_msg);
}

void Node::del(int idx){
            if (childs[0]) throw std::runtime_error("func Node.del(string): can be called only on leafs");
            delete inform[idx];
            inform[idx]=nullptr;
            if (!inform[0] && inform[1]) swap(inform[0], inform[1]);
        }


BTree::BTree(): size(0), head(nullptr){};

BTree::~BTree(){
    auto remover = [](Node* nd, int next_br, int is_leaf){
        if (next_br==2){
            delete nd->childs[0];
            nd->childs[0]=nullptr;
        }
        else if (next_br==3){
            delete nd->childs[1];
            nd->childs[1]=nullptr;
        }
        else if (next_br==-1) {
            delete nd->childs[2];
            nd->childs[2]=nullptr;
        }
    };

    treeTraversal(remover);
    delete head;
}

Node* BTree::getNext(Node* cur, int &idx){
    if (!cur->childs[0]){
        if (idx==0 && cur->inform[1]){
            idx=1;
            return cur;
        }
        else if (cur->parent && (cur->parent->childs[0]==cur || (cur->parent->childs[1]==cur && cur->parent->inform[1]))) {
            idx=cur->parent->childs[1]==cur;
            return cur->parent;
        }
        else if (!cur->parent){
            return nullptr;
        } 
        else {
            Node* n = cur->parent;
            while (n){
                if (*(n->inform[0])>*(cur->inform[idx])){
                    idx=0;
                    return n;
                }
                else if (n->inform[1] && *(n->inform[1])>*(cur->inform[idx])){
                    idx=1;
                    return n;
                }
                n=n->parent;
            }
            return n;
        }
    }
    Node* n = cur->childs[idx+1];
    while (n->childs[0]){
        n = n->childs[0];
    }
    idx=0;
    return n;
}

Node* BTree::getPrev(Node* cur, int &idx){
    if (!cur->childs[0]){
        if (idx==1){
            idx=0;
            return cur;
        }
        else if (cur->parent && (cur->parent->childs[1]==cur || (cur->parent->childs[2]==cur && cur->parent->inform[1]))) {
            idx=cur->parent->childs[2]==cur;
            return cur->parent;
        }
        else if (!cur->parent){
            return nullptr;
        } 
        else {
            Node* n = cur->parent;
            while (n){
                if (n->inform[1] && *(n->inform[1])<*(cur->inform[idx])){
                    idx=1;
                    return n;
                }
                else if (*(n->inform[0])<*(cur->inform[idx])){
                    idx=0;
                    return n;
                }
                n=n->parent;
            }
            return n;
        }
    }
    Node* n = cur->childs[idx];
    while (n->childs[0]){
        if (n->childs[2]) n=n->childs[2];
        else n = n->childs[1];
    }
    if (n->inform[1]) idx=1;
    else idx=0;
    return n;
}

Node* BTree::find(string key){
    Node* cur = head;
    if (!cur) return nullptr;
    while (cur->childs[0]){
        if (*(cur->inform[0])==key || (cur->inform[1] && *(cur->inform[1])==key)) return cur;
        if (*(cur->inform[0])>key) cur = cur->childs[0];
        else if (!cur->inform[1] || *(cur->inform[1])>key) cur=cur->childs[1];
        else  cur = cur->childs[2];
    }
    return cur;
}

void BTree::add(string key, string info){
    size++;
    if (!head){
        head = new Node();
        head->inform[0] = new InfoField(key, info);
        return;
    }
    Node* insert_node = find(key);
    InfoField* median_key=insert_node->add(key, info);
    Node* right_extra_node;
    int mode=-1;
    if (median_key) {right_extra_node = insert_node->splitNode();
        }
    while (median_key){
        if (!insert_node->parent){
            head = new Node();
            head->inform[0]=median_key;
            head->childs[0]=insert_node;
            head->childs[1]=right_extra_node;
            insert_node->parent=head;
            right_extra_node->parent=head;
            return;
        }
        else if (!insert_node->parent->inform[1]){
            insert_node->parent->inform[1]=median_key;
            insert_node->parent->childs[2]=right_extra_node;
            right_extra_node->parent=insert_node->parent;
            if (*(insert_node->parent->inform[1])<*(insert_node->parent->inform[0])){
                swap(insert_node->parent->inform[1], insert_node->parent->inform[0]);
                swap(insert_node->parent->childs[1], insert_node->parent->childs[2]);
            }
            return;
        }
        if (insert_node->parent->childs[0]==insert_node) mode=0;
        else if (insert_node->parent->childs[1]==insert_node) mode=1;
        else if (insert_node->parent->childs[2]==insert_node) mode=2; 
        else { 
            string error_msg = "func BTree.add(string, string): insert_node with key[0] " + 
                (insert_node->inform[0]->key) + "don't match with parents childs\n";
            throw std::runtime_error(error_msg);
        }
        insert_node = insert_node->parent;
        median_key = insert_node->chooseMedian(median_key);
        Node* temp = insert_node->splitNode(mode);
        if (!right_extra_node) std::cout << "ATTENTION2\n";
        if (mode==0) {
            insert_node->childs[1]=right_extra_node;
            right_extra_node->parent=insert_node;
        }
        else if (mode==1) {
            temp->childs[0]=right_extra_node;
            right_extra_node->parent=temp;
        }
        else {
            temp->childs[1]=right_extra_node;
            right_extra_node->parent=temp;
        }
        right_extra_node=temp;
    }
}

Node* BTree::swapWithEquiv(Node* cur, int idx){
    if (!cur) return cur;
    if (!cur->childs[0]) return cur;
    Node *equiv = cur->childs[idx];
    while (equiv->childs[0]){
        if (equiv->childs[2]) equiv=equiv->childs[2];
        else equiv = equiv->childs[1];
    }
    if (equiv->inform[1]) swap(cur->inform[idx], equiv->inform[1]);
    else swap(cur->inform[idx], equiv->inform[0]);
    return equiv;
}

Node* BTree::mergeNodes(Node* empty, int branch){
    Node* extra_node = empty->childs[0];
    if (empty->childs[1]){
        swap(extra_node->inform[1], empty->childs[1]->inform[0]);
        // delete empty->childs[1];
        empty->childs[1]=nullptr;
    }
    if (branch==-1){
        head = extra_node;
        // delete empty;
        head->parent=nullptr;
        return nullptr;
    }
    Node* parent = empty->parent;
    if (branch==0){
        Node *mid_brother = parent->childs[1];
        swap(empty->inform[0], parent->inform[0]);
        if (mid_brother->inform[1]){
            swap(parent->inform[0], mid_brother->inform[0]);
            swap(mid_brother->inform[0], mid_brother->inform[1]);
            swap(empty->childs[1], mid_brother->childs[0]);
            empty->childs[1]->parent=empty;
            swap(mid_brother->childs[0], mid_brother->childs[1]);
            swap(mid_brother->childs[1], mid_brother->childs[2]);
            return nullptr;
        }
        else{
            swap(empty->inform[1], mid_brother->inform[0]);
            swap(mid_brother->childs[0], empty->childs[1]);
            empty->childs[1]->parent=empty;
            swap(mid_brother->childs[1], empty->childs[2]);
            empty->childs[2]->parent=empty;
            parent->childs[1]=nullptr;
            // delete mid_brother;
            if (parent->inform[1]){
                swap(parent->inform[1], parent->inform[0]);
                swap(parent->childs[1], parent->childs[2]);
                return nullptr;
            }
            else {
                return parent;
            }
        }
    }
    else if (branch==1){
        Node* little_brother = parent->childs[0];
        if (little_brother->inform[1]){
            swap(empty->inform[0], parent->inform[0]);
            swap(little_brother->inform[1], parent->inform[0]);
            swap(empty->childs[0], empty->childs[1]);
            swap(empty->childs[0], little_brother->childs[2]);
            empty->childs[0]->parent=empty;
            return nullptr;
        }
        else {
            swap(parent->inform[0], little_brother->inform[1]),
            swap(empty->childs[0], little_brother->childs[2]);
            little_brother->childs[2]->parent=little_brother;
            parent->childs[1]=nullptr;
            if (parent->inform[1]){
                swap(parent->inform[1], parent->inform[0]);
                swap(parent->childs[1], parent->childs[2]);
                return nullptr;
            }
            else{
                return parent;
            }
        }
    }
    else if (branch==2){
        Node *mid_brother = parent->childs[1];
        if (mid_brother->inform[1]){
            swap(parent->inform[1], empty->inform[0]);
            swap(parent->inform[1], mid_brother->inform[1]);
            swap(empty->childs[0], empty->childs[1]);
            swap(mid_brother->childs[2], empty->childs[0]);
            empty->childs[0]->parent=empty;
            return nullptr;
        }
        else {
            swap(parent->inform[1], mid_brother->inform[1]);
            swap(empty->childs[0], mid_brother->childs[2]);
            mid_brother->childs[2]->parent=mid_brother;
            delete empty;
            parent->childs[2]=nullptr;
            return nullptr;
        }
    }
    throw std::runtime_error("func BTree:mergeNodes: something went wrong");
}

void BTree::del(string key){
    Node* rem = find(key);
    if (!rem) {
        std::cout << ("Tree already empty\n");
        return;
    }
    int idx;
    if (*(rem->inform[0])==key) idx=0;
    else if ((rem->inform[1]) && *(rem->inform[1])==key) idx=1;
    else throw std::invalid_argument("func Node.del(string): key \"" + key + "\" not found\n");

    while (rem->childs[0]){
        rem=swapWithEquiv(rem, idx);
        if (rem->inform[1]) idx=1;
        else idx=0;
    }

    rem->del(idx);
    Node* par = rem->parent;
    if (!rem->parent && !rem->inform[0]) {
        delete head;
        head = nullptr;
        return;
    }
    else if (rem->inform[0]) return;
    else {
        int branch=-1;
        if (par->childs[0]==rem) branch=0;
        else if (par->childs[1]==rem) branch=1;
        else if (par->childs[2]==rem) branch=2;
        par = par->redistributeChilds(branch);
        while (par){
            if (!par->parent) branch=-1;
            else if (par->parent->childs[0]==par) branch=0;
            else if (par->parent->childs[1]==par) branch=1;
            else if (par->parent->childs[2]==par) branch=2;
            par = mergeNodes(par, branch);
        }
    }

}

void BTree::printTree(){
    auto printer = [](Node* nd, int next_br, int is_leaf){
        int len=-1;
        Node* temp=nd;
        while (temp){
            temp=temp->parent;
            len++;
        }

        if (next_br==1 || is_leaf) {
            for (int i=0; i<len-1; i++){
                std::cout << "|  ";
            }
            if (len>0) std::cout << "|->";
            std::cout << nd->inform[0]->key;
            if (nd->inform[1]) std::cout << "; " << nd->inform[1]->key;
            std::cout << "\n";
            if (next_br>0) len++;
        }
        else if (next_br<0) len--;
    };
    treeTraversal(printer);
}

void BTree::printInverseSlice(string a, string b){
    string error_msg = "func BTree:printInverseSlice: expected b>a; got a = " +a+ "; b= " + b + "\n";
    if (b<=a) throw std::invalid_argument(error_msg); 
    Node* cur = find(b);
    int idx=-1;
    if (cur->inform[1] && *(cur->inform[1])<b && *(cur->inform[1])>a){
        idx=1;
        std::cout << *(cur->inform[1]);
    }
    else if (*(cur->inform[0])<b && *(cur->inform[0])>a){
        idx=0;
        std::cout << *(cur->inform[0]);
    }
    else return;
    cur = getPrev(cur, idx);
    while (cur && *(cur->inform[idx])>a){
        std::cout << *(cur->inform[idx]);
        cur = getPrev(cur, idx);
    }
}

std::vector<string*> BTree::getGreater(string key){
    Node* n = find(key);
    std::vector<string*> ans{};
    int idx=-1;
    if (n->childs[0]){
        if (*(n->inform[0])==key) idx=0;
        if (n->inform[1] && *(n->inform[1])==key) idx=1;
        n=getNext(n,idx);
    }
    else {
        if (*(n->inform[0])>key) idx=0;
        else if (n->inform[1] && *(n->inform[1])>key) idx=1;
        else {
            if (n->inform[1]) idx=1;
            else idx=0;
            n=getNext(n, idx);
        }
    }
    if (!n) return ans;
    ans.push_back(&(n->inform[idx]->key));
    Information *next = n->inform[idx]->info;
    while (next){
        ans.push_back(&(next->info));
        next=next->next;
    }
    return ans;
}

void BTree::treeTraversal(void func(Node*, int, int)){
    Node* cur = head, next;
    if (!cur) {
        std::cout << "Tree is empty\n";
        return;
    }
    int leaf=(!cur->childs[0]), next_branch = (-1)*leaf+(1-leaf);
    while (next_branch){
        func(cur, next_branch, leaf);
        if (next_branch<0){
            if (cur->parent) {
                if (cur->parent->childs[0]==cur) next_branch=2;
                else if (cur->parent->childs[1]==cur && cur->parent->childs[2]) next_branch=3;
                else next_branch=-1;
                cur = cur->parent;
                leaf=(!cur->childs[0]);
            }
            else next_branch=0;
        }
        else{
            if (cur->childs[next_branch-1]) {
                cur = cur->childs[next_branch-1];
                leaf=(!cur->childs[0]);
                if (leaf) next_branch=-1;
                else next_branch=1;
            }
            else throw std::runtime_error("func BTree.treeTraversal: something went wrong\n");
        }
    }
    

}