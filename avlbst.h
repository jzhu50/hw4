#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    static AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* current);
    AVLNode<Key, Value>* internalFind(const Key& key) const;
    void rotateLeft(AVLNode<Key, Value>* curr);
    void rotateRight(AVLNode<Key, Value>* curr);
    // int height(AVLNode<Key, Value>* curr);
    void insertFix(AVLNode<Key, Value>* child);
    void removeFix(AVLNode<Key, Value>* curr, int diff);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    // bst insertion
    const Key& ikey = new_item.first;
    const Value& ival = new_item.second;

    // if empty tree
    if (this->root_ == nullptr) {
      this->root_ = new AVLNode<Key, Value>(ikey, ival, nullptr);
      return;
    }

    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr; // keep track of parent node

    while (curr != nullptr) {
      parent = curr;

      if (curr->getKey() == ikey) {
        curr->setValue(ival);
        return;
      }
      else if (ikey < curr->getKey()) {
        curr = curr->getLeft();
      }
      else if (ikey > curr->getKey()) {
        curr = curr->getRight();
      }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(ikey, ival, parent);

    // link newNode to tree
    if (ikey < parent->getKey()) {
      parent->setLeft(newNode);
    }
    else {
      parent->setRight(newNode);
    }

    // fix imbalance & update balance factors
    insertFix(newNode);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* curr = internalFind(key);
    if (curr == nullptr) return;

    // removing a node w/ two children
    if (curr->getLeft() != nullptr && curr->getRight() != nullptr) {
      AVLNode<Key, Value>* pred = predecessor(curr);
      nodeSwap(curr, pred);
    }

    // node has at most one child
    AVLNode<Key, Value>* parent = curr->getParent();
    AVLNode<Key, Value>* child = nullptr;
    if (curr->getLeft() != nullptr) {
      child = curr->getLeft();
    } else {
      child = curr->getRight();
    }

    // see which subtree shrinks
    int diff = 0;
    if (parent != nullptr) {
      if (parent->getLeft() == curr) {
        diff = 1;
      }
      else {
        diff = -1;
      }
    }

    // if curr is root
    if (parent == nullptr) this->root_ = child;
    else if (parent->getLeft() == curr) {
      // if curr is left child
      parent->setLeft(child);
    }
    else {
      // if curr is right child
      parent->setRight(child);
    }

    if (child != nullptr) {
      child->setParent(parent);
    }

    delete curr;

    // fix imbalance
    if (parent != nullptr) {
      removeFix(parent, diff);
    }
  }

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* curr)
{
  AVLNode<Key, Value>* parent = curr->getParent();
  AVLNode<Key, Value>* right = curr->getRight();
  AVLNode<Key, Value>* middle = right->getLeft();

  if (parent == nullptr) {
    // curr is root
    this->root_ = right;
  }
  else if (parent->getLeft() == curr) {
    parent->setLeft(right);
  }
  else {
    parent->setRight(right);
  }
  right->setParent(parent);

  curr->setRight(middle);
  if (middle != nullptr) {
    middle->setParent(curr);
  }

  right->setLeft(curr);
  curr->setParent(right);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* curr)
{
  AVLNode<Key, Value>* parent = curr->getParent();
  AVLNode<Key, Value>* left = curr->getLeft();
  AVLNode<Key, Value>* middle = left->getRight();

  if (parent == nullptr) {
    // curr is root
    this->root_ = left;
  }
  else if (parent->getLeft() == curr) {
    parent->setLeft(left);
  }
  else {
    parent->setRight(left);
  }
  left->setParent(parent);

  curr->setLeft(middle);
  if (middle != nullptr) {
    middle->setParent(curr);
  }

  left->setRight(curr);
  curr->setParent(left);
}


template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* child)
{
  // child = newly inserted leaf node
  AVLNode<Key, Value>* curr = child->getParent();

  while (curr != nullptr) {
    // update curr's balance factor
    if (child == curr->getLeft()) {
      curr->updateBalance(-1);
    }
    else {
      curr->updateBalance(1);
    }

    // if balance is 0, stop
    if (curr->getBalance() == 0) {
      return;
    }

    // if balance is 1 or -1, continue to curr's parent
    if (curr->getBalance() == 1 || curr->getBalance() == -1) {
      child = curr;
      curr = curr->getParent();
      continue;
    }

    // if balance is 2 or -2, rotate & update balance
    // 4 cases: LL, LR, RL, RR
    if (curr->getBalance() == -2) {
      AVLNode<Key, Value>* left = curr->getLeft();

      // LL
      if (left->getBalance() == -1) {
        rotateRight(curr);
        // update balance factors
        curr->setBalance(0);
        left->setBalance(0);
      }
      
      // LR
      else {
        AVLNode<Key, Value>* middle = left->getRight();
        int mb = middle->getBalance(); // tracks middle balance

        rotateLeft(left);
        rotateRight(curr);

        if (mb == -1) {
          curr->setBalance(1);
          left->setBalance(0);
        }
        else if (mb == 0) {
          curr->setBalance(0);
          left->setBalance(0);
        }
        else {
          curr->setBalance(0);
          left->setBalance(-1);
        }

        middle->setBalance(0);
      }

      return;
    }
    
    if (curr->getBalance() == 2) {
      AVLNode<Key, Value>* right = curr->getRight();

      // RR
      if (right->getBalance() == 1) {
        rotateLeft(curr);
        curr->setBalance(0);
        right->setBalance(0);
      }

      // RL
      else {
        AVLNode<Key, Value>* middle = right->getLeft();
        int mb = middle->getBalance(); // tracks middle balance

        rotateRight(right);
        rotateLeft(curr);

        if (mb == 1) {
          curr->setBalance(-1);
          right->setBalance(0);
        }
        else if (mb == 0) {
          curr->setBalance(0);
          right->setBalance(0);
        }
        else {
          curr->setBalance(0);
          right->setBalance(1);
        }

        middle->setBalance(0);
      }

      return;
    }
  }
}


template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* curr, int diff)
{
  // curr = parent of node getting removed
  while (curr != nullptr) {
    // update curr's balance factor
    curr->updateBalance(diff);

    // if balance is 1 or -1, stop
    if (curr->getBalance() == 1 || curr->getBalance() == -1) return;

    // 
    AVLNode<Key, Value>* parent = curr->getParent();
    int nextDiff = 0;

    if (parent != nullptr) {
      if (parent->getLeft() == curr) {
        nextDiff = 1;
      }
      else {
        nextDiff = -1;
      }
    }

    // if balance is 0, continue
    if (curr->getBalance() == 0) {
      curr = parent;
      diff = nextDiff;
      continue;
    }

    // if balance is -2
    if (curr->getBalance() == -2) {
      AVLNode<Key, Value>* left = curr->getLeft();
      int lb = left->getBalance();

      // LL
      if (lb == -1 || lb == 0) {
        rotateRight(curr);
        
        if (lb == 0) {
          curr->setBalance(-1);
          left->setBalance(1);
          return;
        }
        
        curr->setBalance(0);
        left->setBalance(0);
      }

      // LR
      else {
        AVLNode<Key, Value>* middle = left->getRight();
        int mb = middle->getBalance();
        
        rotateLeft(left);
        rotateRight(curr);
        
        if (mb == -1) {
          curr->setBalance(1);
          left->setBalance(0);
        }
        else if (mb == 0) {
          curr->setBalance(0);
          left->setBalance(0);
        }
        else {
          curr->setBalance(0);
          left->setBalance(-1);
        }
        
        middle->setBalance(0);
      }
    }

    // if balance is 2
    else if (curr->getBalance() == 2) {
      AVLNode<Key, Value>* right = curr->getRight();
      int rb = right->getBalance();
      
      // RR
      if (rb == 1 || rb == 0) {
        rotateLeft(curr);
        
        if (rb == 0) {
          curr->setBalance(1);
          right->setBalance(-1);
          return;
        }
        
        curr->setBalance(0);
        right->setBalance(0);
      }

      // RL
      else {
        AVLNode<Key, Value>* middle = right->getLeft();
        int mb = middle->getBalance();
        
        rotateRight(right);
        rotateLeft(curr);
        
        if (mb == 1) {
          curr->setBalance(-1);
          right->setBalance(0);
        }
        else if (mb == 0) {
          curr->setBalance(0);
          right->setBalance(0);
        }
        else {
          curr->setBalance(0);
          right->setBalance(1);
        }
        
        middle->setBalance(0);
      }
    }
    
    curr = parent;
    diff = nextDiff;
  }
}

// // get height
// template<class Key, class Value>
// int AVLTree<Key, Value>::height(AVLNode<Key, Value>* curr)
// {
//   if (curr == nullptr) return 0;

//   int rh = height(curr->getRight());
//   int lh = height(curr->getLeft());

//   return std::max(rh, lh) + 1;
// }

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::internalFind(const Key& key) const
{
  AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);

  while (curr != nullptr) {
    if (key == curr->getKey()) return curr;
    else if (key < curr->getKey()) {
      curr = curr->getLeft();
    }
    else if (key > curr->getKey()) {
      curr = curr->getRight();
    }
  }

  return nullptr;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* current) 
{
  if (current == nullptr) return nullptr;

  // 1) no left child, search upward
  if (current->getLeft() == nullptr) {
    AVLNode<Key, Value>* parent = current->getParent();
    // stop when current is a right child of its parent
    // otherwise keep going up
    while (parent != nullptr && current == parent->getLeft()) {
      current = parent;
      parent = parent->getParent();
    }
    return parent;
  }

  // 2) yes right child, go left once then all the way right
  current = current->getLeft();

  while (current->getRight() != nullptr) {
      current = current->getRight();
  }

  return current;
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
