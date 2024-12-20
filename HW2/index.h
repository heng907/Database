#include <utility>
#include <vector>
using namespace std;
typedef pair<int, int> entry;

class internalNode {
    public:
    internalNode();
    vector<void*> children;
    vector<int> keys;
    void insert(int key, void* child);
    internalNode* insertAndSplit(int key, void* newChild, int& kick);
    void* find(int key) const;
};

class leafNode {
    public:
    vector<entry> entries;
    leafNode* prev;
    leafNode* next;
    int x;
    leafNode();
    void insert(int key, int value);
    leafNode* insertAndSplit(int key, int value, int& kick);
    int query(int key) const;
};

class BPTree {
    private:
    void* root = nullptr;
    int level = 0;

    public:
    BPTree();
    ~BPTree();
    void insert(int k, int v);
    int query(const int k) const;
    int query(const int from, const int to) const;
    void test(int s) const;
};

class Index {
    private:
    BPTree b;

    public:
    Index(int num_rows, const vector<int>& keys, const vector<int>& values);
    void key_query(vector<int>& keys);
    void range_query(const vector<pair<int, int>>& keys);
    void clear_index();
};