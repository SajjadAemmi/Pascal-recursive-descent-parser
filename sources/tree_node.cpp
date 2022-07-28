#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stack>
#include <set>
#include <list>
#include <string>
#include <sstream>
#include <map>
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
