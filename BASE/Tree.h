//
// Created by swagger on 2021/4/6.
//

#ifndef WEBSERVER_TREE_H
#define WEBSERVER_TREE_H

template<typename Key, typename Value>
class BinaryTree{

private:

    struct Node{
        Key key;
        Value value;
        Node* left;
        Node* right;

        Node(Key key, Value value){
            this->key = key;
            this->value = value;
            this->left = this->right = nullptr;
        }

        Node(Node* node){
            this->key = node->key;
            this->value = node->value;
            this->left = node->left;
            this->right = right;
        }
    };


    int count;

    Node* root;

public:

    BinaryTree(){
        root = nullptr;
        count = 0;
    }

};

#endif //WEBSERVER_TREE_H
