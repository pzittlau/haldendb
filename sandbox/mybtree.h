#pragma once

#include <vector>
#include  <iostream>

class mybtree
{
};

template <typename KeyType, typename ValueType, typename CacheType>
class BPlusTreeNode {
public:
    BPlusTreeNode* parent;
    std::vector<KeyType> keys;
    std::vector<ValueType> values;
    BPlusTreeNode* next; // For leaf nodes
    std::vector<BPlusTreeNode*> children;
    bool is_leaf;

    BPlusTreeNode() : parent(nullptr), next(nullptr), is_leaf(true) {}

    // Implement methods for insertion, deletion, and splitting here.
};

// B+-tree class
template <typename KeyType, typename ValueType, typename CacheType>
class BPlusTree {
private:
    BPlusTreeNode<KeyType, ValueType>* root;
    int degree;  // Adjust this according to your needs.

public:
    BPlusTree(int degree) : root(nullptr), degree(degree) {}

    // Insertion method
    void insert(const KeyType& key, const ValueType& value) {
        if (root == nullptr) {
            root = new BPlusTreeNode<KeyType, ValueType>();
        }

        insertRecursive(root, key, value);
    }

    // Helper method to perform insertion recursively
    void insertRecursive(BPlusTreeNode<KeyType, ValueType>* current, const KeyType& key, const ValueType& value) {
        // If the current node is a leaf node, insert the value
        if (current->is_leaf) {
            insertInLeafNode(current, key, value);
        }
        else {
            // Find the appropriate child to descend into
            int i = 0;
            while (i < current->keys.size() && key > current->keys[i]) {
                i++;
            }

            // Recursively insert into the appropriate child
            insertRecursive(current->children[i], key, value);
        }

        // Check if the current node is overflowing and perform splitting if necessary
        if (current->keys.size() > degree) {
            splitNode(current);
        }
    }

    // Helper method to insert into a leaf node
    void insertInLeafNode(BPlusTreeNode<KeyType, ValueType>* node, const KeyType& key, const ValueType& value) {
        // Find the appropriate position for the new key
        int i = 0;
        while (i < node->keys.size() && key > node->keys[i]) {
            i++;
        }

        // Insert the key and value
        node->keys.insert(node->keys.begin() + i, key);
        node->values.insert(node->values.begin() + i, value);
    }

    // Helper method to split a node when necessary
    void splitNode(BPlusTreeNode<KeyType, ValueType>* node) {
        if (node->keys.size() <= degree) {
            return;  // No need to split
        }

        // Create a new node for the right side
        BPlusTreeNode<KeyType, ValueType>* new_node = new BPlusTreeNode<KeyType, ValueType>();

        // Calculate the split point
        int mid = node->keys.size() / 2;

        // Copy the keys and values to the new node
        new_node->keys.assign(node->keys.begin() + mid, node->keys.end());
        new_node->values.assign(node->values.begin() + mid, node->values.end());

        // Update the parent and children pointers
        if (node->parent) {
            int insert_index = node->parent->keys.size();
            for (int i = 0; i < node->parent->keys.size(); ++i) {
                if (node->keys[mid] < node->parent->keys[i]) {
                    insert_index = i;
                    break;
                }
            }

            // Insert the middle key into the parent node
            node->parent->keys.insert(node->parent->keys.begin() + insert_index, node->keys[mid]);
            node->parent->children.insert(node->parent->children.begin() + insert_index + 1, new_node);
            new_node->parent = node->parent;
        }
        else {
            // Create a new root node
            BPlusTreeNode<KeyType, ValueType>* new_root = new BPlusTreeNode<KeyType, ValueType>();
            new_root->keys.push_back(node->keys[mid]);
            new_root->children.push_back(node);
            new_root->children.push_back(new_node);
            node->parent = new_root;
            new_node->parent = new_root;
            root = new_root;

            new_root->is_leaf = false;
        }

        // Update the original node
        node->keys.erase(node->keys.begin() + mid, node->keys.end());
        node->values.erase(node->values.begin() + mid, node->values.end());
    }

    // Deletion method
    void remove(const KeyType& key) {
        if (!root) {
            std::cerr << "Tree is empty. Cannot delete key." << std::endl;
            return;
        }

        remove_recursive(root, key);
    }

    void remove_recursive(BPlusTreeNode<KeyType, ValueType>* current, const KeyType& key) {
        if (current->is_leaf) {
            remove_from_leaf(current, key);
        }
        else {
            int child_index = find_child_index(current, key);
            remove_recursive(current->children[child_index], key);
        }

        // Handle underflow and merge nodes if necessary.
        if (current != root && current->keys.size() < degree / 2) {
            // Implement underflow handling logic.
        }
    }

    void remove_from_leaf(BPlusTreeNode<KeyType, ValueType>* node, const KeyType& key) {
        auto it = std::lower_bound(node->keys.begin(), node->keys.end(), key);

        if (it != node->keys.end() && *it == key) {
            int index = it - node->keys.begin();
            node->keys.erase(it);
            node->values.erase(node->values.begin() + index);
        }
    }

    int find_child_index(BPlusTreeNode<KeyType, ValueType>* parent, const KeyType& key) {
        for (size_t i = 0; i < parent->keys.size(); ++i) {
            if (key < parent->keys[i]) {
                return i;
            }
        }
        return parent->keys.size();
    }

    // Search method
    ValueType search(const KeyType& key) {
        if (!root) {
            std::cerr << "Tree is empty. Cannot search for key." << std::endl;
            // Return an appropriate value or error indication.
        }

        return search_recursive(root, key);
    }

    ValueType search_recursive(BPlusTreeNode<KeyType, ValueType>* current, const KeyType& key) {
        if (current->is_leaf) {
            auto it = std::lower_bound(current->keys.begin(), current->keys.end(), key);
            if (it != current->keys.end() && *it == key) {
                int index = it - current->keys.begin();
                return current->values[index];
            }
            else {
                // Key not found in the leaf node.
                // Return an appropriate value or error indication.
            }
        }
        else {
            int child_index = find_child_index(current, key);
            return search_recursive(current->children[child_index], key);
        }
    }
};