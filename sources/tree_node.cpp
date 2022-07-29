#include <iostream>
#include "tree_node.hpp"

using namespace std;

TreeNode::TreeNode(string label)
{
    data = label;
}

TreeNode::TreeNode()
{
    data = "";
}
