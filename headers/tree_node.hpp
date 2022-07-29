#pragma once 

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

using namespace std;

class TreeNode
{
public:
	string data;
	vector<TreeNode*> childs;

	TreeNode();
	TreeNode(string label);
};
