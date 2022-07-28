#pragma once 

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
#include "token.hpp"
#include "state.hpp"
#include "tree_node.hpp"


using namespace std;

class Parser
{
public:
	TreeNode* 			root;
	int 				TokenIndex;
	Token 				token;
	string 				nexttoken;
	bool 				GoodProgram;
	string 				HTMLTree;
	bool 				See_Epsilon;
	vector<Token> 		Tokens;

	Parser();

	TreeNode* program();
	TreeNode* identifier_list();
	TreeNode* identifier_list_prime();
	TreeNode* declarations();
	TreeNode* declaration_list();
	TreeNode* declaration_list_prime();
	TreeNode* type();
	TreeNode* standard_type();
	TreeNode* array_type();
	TreeNode* argumenrs();
	TreeNode* parameter_list();
	TreeNode* parameter_list_prime();
	TreeNode* statement_list();
	TreeNode* statement_list_prime();
	TreeNode* statement();
	TreeNode* statement_prime();
	TreeNode* restricted_statement();
	TreeNode* elementary_statement();
	TreeNode* elementary_statement_prime();
	TreeNode* variable();
	TreeNode* variable_prime();
	TreeNode* subprogram_declarations();
	TreeNode* subprogram_declarations_prime();
	TreeNode* subprogram_declaration();
	TreeNode* subprogram_head();
	TreeNode* compound_statement();
	TreeNode* expression();
	TreeNode* expression_prime();
	TreeNode* expression_list();
	TreeNode* expression_list_prime();
	TreeNode* simple_expression();
	TreeNode* simple_expression_prime();
	TreeNode* term();
	TreeNode* term_prime();
	TreeNode* factor();

	void Error();
	Token NextToken();
	void CreateTree();

	void OutputFileHTMLTree();
	void CreateHTMLTree(TreeNode*);
	void PrintTree(TreeNode*);
};
