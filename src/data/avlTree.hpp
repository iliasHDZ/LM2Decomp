#pragma once

#include "../common.hpp"

struct AVLTreeNode {
    AVLTreeNode* children[2];
    AVLTreeNode* parent;
};

class AVLTreeUntemplated {
public:
    AVLTreeNode* AddAVLNode(AVLTreeNode** root, void* key, void* value, AVLTreeNode** found_node);

    // Remove Node func ...

protected:
    virtual AVLTreeNode* AllocateEntry(void* key, void* value) const = 0;

    virtual AVLTreeNode* Find(void* key, AVLTreeNode* root) const = 0;

    virtual int CompareNodes(AVLTreeNode* node1, AVLTreeNode* node2) const = 0;
};

template <typename _Key, typename _Value>
class AVLTreeBase : public AVLTreeUntemplated {
public:
    struct Node : public AVLTreeNode {
        _Key key;
        _Value value;
    };

public:
    inline void AddNode(_Key key, _Value value);

    inline Node* Find(_Key key) const;

public:
    virtual AVLTreeNode* AllocateEntry(void* key, void* value) const;

    virtual AVLTreeNode* Find(void* key, AVLTreeNode* root) const;

    virtual int CompareNodes(AVLTreeNode* node1, AVLTreeNode* node2) const;

private:
    Node* root = nullptr;
};

template <typename _Key, typename _Value>
class nlAVLTree : public AVLTreeBase<_Key, _Value> {
public:
};