#include "index.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#define key first
#define value second
#define MIN_INT -2147483648

using namespace std;

const int M = 127;
const int N = 153;
const int CHUNK_SIZE = 4096;

//write file
inline void writeFile(const vector<int>& vec, FILE* f) {
    char buffer[10000];
    int current = 0;
    int i = 0;
    const int len = vec.size();

    while (i < len) {
        current += sprintf(&buffer[current], "%d\n", vec[i]);
        i++;

        if (current >= CHUNK_SIZE) {
            fwrite(buffer, CHUNK_SIZE, 1, f);
            current -= CHUNK_SIZE;
            memcpy(buffer, &buffer[CHUNK_SIZE], current);
        }
    }

    if (current > 0) fwrite(buffer, 1, current, f);
}

// ------------- BPlusTree -------------
BPTree::BPTree() {
}

BPTree::~BPTree() {
    if (!root) return;
    vector<void*> p = {root};
    for (int i = 0; i < level; i++) {
        vector<void*> q;
        for (auto a : p) {
            internalNode* inode = (internalNode*)a;
            q.insert(q.end(), (inode->children).begin(), (inode->children).end());
            delete inode;
        }
        p.swap(q);
    }
    for (auto a : p) {
        leafNode* lnode = (leafNode*)a;
        delete lnode;
    }
}

void BPTree::insert(int k, int v) {
    if (!root) {
        leafNode* l = new leafNode();
        l->prev = l->next = nullptr;
        l->entries.emplace_back(k, v);
        l->x = v;
        root = l;
        return;
    }

    // Find leaf node
    vector<internalNode*> layer;
    void* _n = root;
    for (int i = 0; i < level; i++) {
        internalNode* inode = (internalNode*)_n;
        layer.push_back(inode);
        _n = inode->find(k);
    }

    // Insert entry into leaf node
    leafNode* lnode = (leafNode*)_n;
    if (lnode->entries.size() + 1 <= N) {
        lnode->insert(k, v);
        return;
    }

    // Check splitting to top
    int kick;
    void* split = lnode->insertAndSplit(k, v, kick);
    for (int i = level - 1; i >= 0; i--) {
        internalNode* inode = layer[i];
        if (inode->keys.size() + 1 <= M - 1) {
            inode->insert(kick, split);
            return;
        }
        split = inode->insertAndSplit(kick, split, kick);
    }

    // Root is splitted
    if (split) {
        internalNode* newRoot = new internalNode();
        newRoot->keys.push_back(kick);
        newRoot->children.push_back(split);
        newRoot->children.push_back(root);
        root = newRoot;
        level++;
    }
}

int BPTree::query(const int k) const {
    if (!root) return -1;
    void* _n = root;
    for (int i = 0; i < level; i++) {
        _n = ((internalNode*)_n)->find(k);
    }
    return ((leafNode*)_n)->query(k);
}

//find the minimum value in the range(l,r)

int BPTree::query(const int l, const int r) const {
    
    if (!root) return -1;

    void* _n = root;
    void* _r = root;
    for (int i = 0; i < level; i++) {
        _n = ((internalNode*)_n)->find(l);
        _r = ((internalNode*)_r)->find(r);
    }

    int ret = MIN_INT;
    leafNode* lnode = (leafNode*)_n;
    
    // Find the position of the first entry in the node (lnode) whose key is not less than "l".
    auto lix = lower_bound((lnode->entries).begin(),(lnode->entries).end(), l, [](const entry& e, const int& n) {
        return e.key < n;
    });

    for (lix->key==l; lix != lnode->entries.end() && lix->key <= r; lix++) {
        ret = (ret == MIN_INT) ? lix->value : min(ret, lix->value);
    }

    // Check if lnode is the same as _r
    if (lnode == _r) 
        return (ret == MIN_INT) ? -1 : ret;

    lnode = lnode->next;

    // Iterate through the remaining nodes
    for (; lnode != _r; lnode = lnode->next) {
        auto lix = lnode->entries.begin();
        for (; lix != lnode->entries.end() && lix->key <= r; lix++) {
            ret = (ret == MIN_INT) ? lix->value : min(ret, lix->value);
        }
        // ret = min(ret, lnode->x);
    }
    //------additional-------
    lix = lnode->entries.begin();
    for (; lix != lnode->entries.end() && lix->key <= r; lix++) {
        ret = min(ret, lix->value);
    }
    //------additional-------
    return (ret == MIN_INT) ? -1 : ret;
}
// ------------- BPlusTree -------------

// --------------- Leaf ----------------

leafNode::leafNode() {
    entries.reserve(N);
}

inline void leafNode::insert(int key, int value) {
    auto a = upper_bound(entries.begin(), entries.end(), key, [](const int& n, const entry& e) {
        return n < e.key;
    });
    entries.insert(a, entry(key, value));
    x = max(x, value);
}

inline leafNode* leafNode::insertAndSplit(int key, int value, int& kick) {
    const int reqSize = (N + 1) / 2;
    const int remSize = N + 1 - reqSize;

    leafNode* ret = new leafNode();
    int j = 0;
    bool put = 0;
    for (int i = 0; i < reqSize; i++) {
        if (put || entries[j].key < key) {
            ret->x = max(ret->x, entries[j].value);
            ret->entries.push_back(entries[j++]);
        } 
        else {
            ret->x = max(ret->x, value);
            ret->entries.emplace_back(key, value);
            put = 1;
        }
    }

    x = MIN_INT;
    for (int i = 0; i < remSize; i++) {
        if (j == entries.size() || (!put && key < entries[j].key)) {
            entries[i].key = key;
            entries[i].value = value;
            x = max(x, value);
            put = 1;
        }  //
        else {
            x = max(x, entries[j].value);
            entries[i] = entries[j++];
        }
    }
    entries.resize(remSize);
    entries.reserve(N);

    ret->next = this;
    ret->prev = prev;
    if (prev) prev->next = ret;
    prev = ret;

    kick = entries[0].key;
    return ret;
}

inline int leafNode::query(int key) const {
    auto a = lower_bound(entries.begin(), entries.end(), key, [](const entry& e, int n) {
        return e.key < n;
    });
    if (a == entries.end()) return -1;
    return a->key == key ? a->value : -1;
}

// --------------- Leaf ----------------

//-------------- Internal --------------

internalNode::internalNode() {
    children.reserve(M);
    keys.reserve(M - 1);
}

inline void internalNode::insert(int key, void* child) {
    auto a = upper_bound(keys.begin(), keys.end(), key);
    int index = a - keys.begin();
    keys.insert(a, key);
    children.insert(children.begin() + index, child);
}

inline internalNode* internalNode::insertAndSplit(int key, void* newChild, int& kick) {
    const int reqSize = M / 2;  // ceil;
    const int remSize = M - 1 - reqSize;

    internalNode* ret = new internalNode();
    int kp = 0, cp = 0;
    bool put = 0;
    for (int i = 0; i < reqSize; i++) {
        if (put || keys[kp] < key) {
            ret->keys.push_back(keys[kp++]);
            ret->children.push_back(children[cp++]);
        }  
        else {
            ret->keys.push_back(key);
            ret->children.push_back(newChild);
            put = 1;
        }
    }

    if (put || keys[kp] < key) {
        kick = keys[kp++];
        ret->children.push_back(children[cp++]);
    } 
    else {
        kick = key;
        ret->children.push_back(newChild);
        put = 1;
    }

    int j = 0;
    for (int i = 0; i < remSize; i++, j++) {
        if (kp == keys.size() || (!put && key < keys[kp])) {
            keys[i] = key;
            children[j++] = newChild;
            children[j] = children[cp++];
            put = 1;
        } else {
            keys[i] = keys[kp++];
            children[j] = children[cp++];
        }
    }
    if (cp < M) {
        children[j] = children[cp++];
    }

    keys.resize(remSize);
    keys.reserve(M - 1);
    children.resize(remSize + 1);
    children.reserve(M);
    return ret;
}

inline void* internalNode::find(int key) const {
    auto a = upper_bound(keys.begin(), keys.end(), key);
    int index = a - keys.begin();
    return children[index];
}
// ------------- Internal ---------------

// --------------- Index ----------------

// Constructs a B+ tree index by inserting the key-value pairs into the B+ tree ​one by 
// one​.
Index::Index(int num_rows, const vector<int>& keys, const vector<int>& values) {
    for (int i = 0; i < keys.size(); i++) {
        // One by one
        b.insert(keys[i], values[i]);
    }
}


// Outputs a file key_query_out.txt, each row consists of an integer which is the value
// corresponds to the keys in query_keys; or -1 if the key is not found.
void Index::key_query(vector<int>& keys) {
    int i=0;
    while(i<keys.size()){
        keys[i]=b.query(keys[i]);
        i++;
    }

    FILE* f = fopen("key_query_out.txt", "w");
    writeFile(keys, f);
    fclose(f);
}


// Outputs a file range_query_out.txt, each row consists of an integer which is the
// MINIMUM​ value in the given query key range; or -1 if no key found in the range.
void Index::range_query(const vector<pair<int, int>>& keys) {
    vector<int> ans(keys.size());
    int i=0;
    while(i<keys.size()){
        ans[i] = b.query(keys[i].first, keys[i].second);
        i++;
    }
    FILE* f = fopen("range_query_out.txt", "w");
    writeFile(ans, f);
    fclose(f);
}
//clear the heap blocks
void Index::clear_index() {

}