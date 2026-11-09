#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <algorithm>
#endif

#include "equal-paths.h"
using namespace std;

// You may add any prototypes of helper functions here
int subtreeHeight(Node* root);

bool equalPaths(Node * root)
{
    if (root == nullptr) return true;
    if (subtreeHeight(root) != -1) return true;
    return false;
}

int subtreeHeight(Node* root) {
  // post-order traversal: left -> right -> root
  if (root == nullptr) return 0;

  int lh = subtreeHeight(root->left);
  int rh = subtreeHeight(root->right);

  if (lh == -1 || rh == -1) return -1; // mismatch found

  // root processing
  if (root->left != nullptr && root->right != nullptr) {
    // if both children exist, subtree heights must match
    if (lh == rh) return lh + 1;
    else return -1;
  }

  return max(lh, rh) + 1;
}

