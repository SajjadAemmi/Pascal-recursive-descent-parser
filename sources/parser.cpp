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
#include "parser.hpp"

using namespace std;

#define EPSILON			-1
#define CLOSURE			'*'
#define OPEN_PAREN		'('
#define CLOSE_PAREN		')'
#define OR				'+'
#define CONCAT			'.'

Parser::Parser()
{
	token_index = 0;
	good_program = true;
	see_epsilon = false;
}

void Parser::createTree()
{
	token = nextToken();
	root = program();
}

void Parser::Error()
{
	cout << "Syntax Error near " << token.value << "\n";
	token_index++;
	good_program = false;
}

Token Parser::nextToken()
{
	if (see_epsilon == true)
	{
		see_epsilon = false;
		return token;
	}
	return tokens[token_index++];
}

TreeNode* Parser::factor()
{
	if (token.value == "not")
	{
		TreeNode* not_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* factor_node = factor();
		if (factor_node != NULL)
		{
			TreeNode* myroot = new TreeNode("factor");
			myroot->childs.push_back(not_node);
			myroot->childs.push_back(factor_node);
			return myroot;
		}
	}
	else if (token.value == "(")
	{
		TreeNode* parantez_open_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* expression_list_node = expression_list();
		if (expression_list_node != NULL)
		{
			token = nextToken();
			if (token.value == ")")
			{
				TreeNode* parantez_close_node = new TreeNode(token.value);
				TreeNode* myroot = new TreeNode("factor");
				myroot->childs.push_back(parantez_open_node);
				myroot->childs.push_back(expression_list_node);
				myroot->childs.push_back(parantez_close_node);
				return myroot;
			}
		}
	}
	else if (token.type == "CONSTANT")
	{
		TreeNode* CONSTANT_node = new TreeNode(token.value);
		TreeNode* myroot = new TreeNode("factor");
		myroot->childs.push_back(CONSTANT_node);
		return myroot;
	}
	else
	{
		TreeNode* variable_node = variable();
		if (variable_node != NULL)
		{
			TreeNode* myroot = new TreeNode("factor");
			myroot->childs.push_back(variable_node);
			return myroot;
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::term_prime()
{
	if (token.type == "MULOP")
	{
		TreeNode* MULOP_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* factor_node = factor();
		if (factor_node != NULL)
		{
			token = nextToken();
			TreeNode* term_prime_node = term_prime();
			if (term_prime_node != NULL)
			{
				TreeNode* myroot = new TreeNode("term_prime");
				myroot->childs.push_back(MULOP_node);
				myroot->childs.push_back(factor_node);
				myroot->childs.push_back(term_prime_node);
				return myroot;
			}
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode();
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::term()
{
	TreeNode* factor_node = factor();
	if (factor_node != NULL)
	{
		token = nextToken();
		TreeNode* term_node = term_prime();
		if (term_node != NULL)
		{
			TreeNode* myroot = new TreeNode("term");
			myroot->childs.push_back(factor_node);
			myroot->childs.push_back(term_node);
			return myroot;
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::simple_expression_prime()
{
	if (token.type == "ADDOP")
	{
		TreeNode* ADDOP_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* term_node = term();
		if (term_node != NULL)
		{
			token = nextToken();
			TreeNode* simple_expression_prime_node = simple_expression_prime();
			if (simple_expression_prime_node != NULL)
			{
				TreeNode* myroot = new TreeNode("simple_expression_prime");
				myroot->childs.push_back(ADDOP_node);
				myroot->childs.push_back(term_node);
				myroot->childs.push_back(simple_expression_prime_node);
				return myroot;
			}
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode();
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::simple_expression()
{
	if (token.value == "+" || token.value == "-")
	{
		TreeNode* sign_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* term_node = term();
		if (term_node != NULL)
		{
			token = nextToken();
			TreeNode* simple_expression_prime_node = simple_expression_prime();
			if (simple_expression_prime_node != NULL)
			{
				TreeNode* myroot = new TreeNode("simple_expression");
				myroot->childs.push_back(sign_node);
				myroot->childs.push_back(term_node);
				myroot->childs.push_back(simple_expression_prime_node);
				return myroot;
			}
		}
	}
	else
	{
		TreeNode* term_node = term();
		if (term_node != NULL)
		{
			token = nextToken();
			TreeNode* simple_expression_prime_node = simple_expression_prime();
			if (simple_expression_prime_node != NULL)
			{
				TreeNode* myroot = new TreeNode("simple_expression");
				myroot->childs.push_back(term_node);
				myroot->childs.push_back(simple_expression_prime_node);
				return myroot;
			}
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::expression_prime()
{
	if (token.type == "RELOP")
	{
		TreeNode* RELOP_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* simple_expression_node = simple_expression();
		if (simple_expression_node != NULL)
		{
			TreeNode* myroot = new TreeNode("expression_prime");
			myroot->childs.push_back(RELOP_node);
			myroot->childs.push_back(simple_expression_node);
			return myroot;
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode();
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::expression()
{
	TreeNode* simple_expression_node = simple_expression();
	if (simple_expression_node != NULL)
	{
		token = nextToken();
		TreeNode* expression_prime_node = expression_prime();
		if (expression_prime_node != NULL)
		{
			TreeNode* myroot = new TreeNode("expression");
			myroot->childs.push_back(simple_expression_node);
			myroot->childs.push_back(expression_prime_node);
			return myroot;
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::expression_list_prime()
{
	if (token.value == ",")
	{
		TreeNode* comma_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* expression_node = expression();
		if (expression_node != NULL)
		{
			token = nextToken();
			TreeNode* expression_list_prime_node = expression_list_prime();
			if (expression_list_prime_node != NULL)
			{
				TreeNode* myroot = new TreeNode("expression_list_prime");
				myroot->childs.push_back(comma_node);
				myroot->childs.push_back(expression_node);
				myroot->childs.push_back(expression_list_prime_node);
				return myroot;
			}
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode();
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::expression_list()
{
	TreeNode* expression_node = expression();
	if (expression_node != NULL)
	{
		token = nextToken();
		TreeNode* expression_list_prime_node = expression_list_prime();
		if (expression_list_prime_node != NULL)
		{
			TreeNode* myroot = new TreeNode("expression_list");
			myroot->childs.push_back(expression_node);
			myroot->childs.push_back(expression_list_prime_node);
			return myroot;
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::variable_prime()
{
	if (token.value == "(")
	{
		TreeNode* parantez_open_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* expression_list_node = expression_list();
		if (expression_list_node != NULL)
		{
			token = nextToken();
			if (token.value == ")")
			{
				TreeNode* parantez_close_node = new TreeNode(token.value);
				TreeNode* myroot = new TreeNode("variable_prime");
				myroot->childs.push_back(parantez_open_node);
				myroot->childs.push_back(expression_list_node);
				myroot->childs.push_back(parantez_close_node);
				return myroot;
			}
		}
	}
	else if (token.value == "[")
	{
		TreeNode* bracket_open_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* expression_node = expression();
		if (expression_node != NULL)
		{
			token = nextToken();
			if (token.value == "]")
			{
				TreeNode* bracket_close_node = new TreeNode(token.value);
				TreeNode* myroot = new TreeNode("variable_prime");
				myroot->childs.push_back(bracket_open_node);
				myroot->childs.push_back(expression_node);
				myroot->childs.push_back(bracket_close_node);
				return myroot;
			}
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode();
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::variable()
{
	if (token.type == "IDENTIFIER")
	{
		TreeNode* IDENTIFIER_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* variable_prime_node = variable_prime();
		TreeNode* myroot = new TreeNode("variable");
		myroot->childs.push_back(IDENTIFIER_node);
		myroot->childs.push_back(variable_prime_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::elementary_statement_prime()
{
	if (token.type == "ASSIGNOP")
	{
		TreeNode* ASSIGNOP_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* expression_node = expression();
		if (expression_node != NULL)
		{
			TreeNode* myroot = new TreeNode("elementary_statement_prime");
			myroot->childs.push_back(ASSIGNOP_node);
			myroot->childs.push_back(expression_node);
			return myroot;
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode("elementary_statement_prime");
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::elementary_statement()
{
	TreeNode* variable_node = variable();
	if (variable_node != NULL)
	{
		token = nextToken();
		TreeNode* elementary_statement_prime_node = elementary_statement_prime();
		if (elementary_statement_prime_node != NULL)
		{
			TreeNode* myroot = new TreeNode("elementary_statement");
			myroot->childs.push_back(variable_node);
			myroot->childs.push_back(elementary_statement_prime_node);
			return myroot;
		}
	}
	else
	{
		TreeNode* compound_statement_node = compound_statement();
		if (compound_statement_node != NULL)
		{
			TreeNode* myroot = new TreeNode("elementary_statement");
			myroot->childs.push_back(compound_statement_node);
			return myroot;
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::restricted_statement()
{
	if (token.value == "if")
	{
		TreeNode* if_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* expression_node = expression();
		if (expression_node != NULL)
		{
			token = nextToken();
			if (token.value == "then")
			{
				TreeNode* then_node = new TreeNode(token.value);
				token = nextToken();
				TreeNode* restricted_statement_node = restricted_statement();
				if (restricted_statement_node != NULL)
				{
					token = nextToken();
					if (token.value == "else")
					{
						TreeNode* else_node = new TreeNode(token.value);
						token = nextToken();
						TreeNode* restricted_statement2_node = restricted_statement();
						if (restricted_statement2_node != NULL)
						{
							TreeNode* myroot = new TreeNode("restricted_statement");
							myroot->childs.push_back(if_node);
							myroot->childs.push_back(expression_node);
							myroot->childs.push_back(then_node);
							myroot->childs.push_back(restricted_statement_node);
							myroot->childs.push_back(else_node);
							myroot->childs.push_back(restricted_statement2_node);
							return myroot;
						}
					}
				}
			}
		}
	}
	else if (token.value == "while")
	{
		TreeNode* while_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* expression_node = expression();
		if (expression_node != NULL)
		{
			token = nextToken();
			if (token.value == "do")
			{
				TreeNode* do_node = new TreeNode(token.value);
				token = nextToken();
				TreeNode* restricted_statement_node = restricted_statement();
				if (restricted_statement_node != NULL)
				{
					TreeNode* myroot = new TreeNode("restricted_statement");
					myroot->childs.push_back(while_node);
					myroot->childs.push_back(expression_node);
					myroot->childs.push_back(do_node);
					myroot->childs.push_back(restricted_statement_node);
					return myroot;
				}
			}
		}
	}
	else
	{
		TreeNode* elementary_statement_node = elementary_statement();
		if (elementary_statement_node != NULL)
		{
			TreeNode* myroot = new TreeNode("restricted_statement");
			myroot->childs.push_back(elementary_statement_node);
			return myroot;
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::statement_prime()
{
	TreeNode* restricted_statement_node = restricted_statement();
	if (restricted_statement_node != NULL)
	{
		token = nextToken();
		if (token.value == "else")
		{
			TreeNode* else_node = new TreeNode(token.value);
			token = nextToken();
			TreeNode* statement_node = statement();
			if (statement_node != NULL)
			{
				TreeNode* myroot = new TreeNode("statement_prime");
				myroot->childs.push_back(restricted_statement_node);
				myroot->childs.push_back(else_node);
				myroot->childs.push_back(statement_node);
				return myroot;
			}
		}
	}
	else
	{
		TreeNode* statement_node = statement();
		if (statement_node != NULL)
		{
			TreeNode* myroot = new TreeNode();
			myroot->childs.push_back(statement_node);
			return myroot;
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::statement()
{
	if (token.value == "if")
	{
		TreeNode* if_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* expression_node = expression();
		if (expression_node != NULL)
		{
			token = nextToken();
			if (token.value == "then")
			{
				TreeNode* then_node = new TreeNode(token.value);
				token = nextToken();
				TreeNode* statement_prime_node = statement_prime();
				if (statement_prime_node != NULL)
				{
					TreeNode* myroot = new TreeNode("statement");
					myroot->childs.push_back(if_node);
					myroot->childs.push_back(expression_node);
					myroot->childs.push_back(then_node);
					myroot->childs.push_back(statement_prime_node);
					return myroot;
				}
			}
		}
	}
	else if (token.value == "while")
	{
		TreeNode* while_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* expression_node = expression();
		if (expression_node != NULL)
		{
			token = nextToken();
			if (token.value == "do")
			{
				TreeNode* do_node = new TreeNode(token.value);
				token = nextToken();
				TreeNode* statement_node = statement();
				if (statement_node != NULL)
				{
					TreeNode* myroot = new TreeNode("statement");
					myroot->childs.push_back(while_node);
					myroot->childs.push_back(expression_node);
					myroot->childs.push_back(do_node);
					myroot->childs.push_back(statement_node);
					return myroot;
				}
			}
		}
	}
	else
	{
		TreeNode* elementary_statement_node = elementary_statement();
		if (elementary_statement_node != NULL)
		{
			TreeNode* myroot = new TreeNode("statement");
			myroot->childs.push_back(elementary_statement_node);
			return myroot;
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::statement_list_prime()
{
	if (token.value == ";")
	{
		TreeNode* semicolon_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* statement_node = statement();
		if (statement_node != NULL)
		{
			TreeNode* myroot = new TreeNode("statement_list_prime");
			myroot->childs.push_back(semicolon_node);
			myroot->childs.push_back(statement_node);
			return myroot;
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode();
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::statement_list()
{
	TreeNode* statement_node = statement();
	if (statement_node != NULL)
	{
		token = nextToken();
		TreeNode* statement_list_prime_node = statement_list_prime();
		if (statement_list_prime_node != NULL)
		{
			TreeNode* myroot = new TreeNode("statement_list");
			myroot->childs.push_back(statement_node);
			myroot->childs.push_back(statement_list_prime_node);
			return myroot;
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::compound_statement()
{
	if (token.value == "begin")
	{
		TreeNode* begin_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* statement_list_node = statement_list();
		if (statement_list_node != NULL)
		{
			token = nextToken();
			if (token.value == "end")
			{
				TreeNode* end_node = new TreeNode(token.value);
				TreeNode* myroot = new TreeNode("compound_statement");
				myroot->childs.push_back(begin_node);
				myroot->childs.push_back(statement_list_node);
				myroot->childs.push_back(end_node);
				return myroot;
			}
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::parameter_list_prime()
{
	if (token.value == ";")
	{
		TreeNode* semicolon_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* identifier_list_node = identifier_list();
		if (identifier_list_node != NULL)
		{
			token = nextToken();
			if (token.value == ":")
			{
				TreeNode* colon_node = new TreeNode(token.value);
				token = nextToken();
				TreeNode* type_node = type();
				if (type_node != NULL)
				{
					token = nextToken();
					TreeNode* parameter_list_prime_node = parameter_list_prime();
					if (parameter_list_prime_node != NULL)
					{
						TreeNode* myroot = new TreeNode("parameter_list_prime");
						myroot->childs.push_back(semicolon_node);
						myroot->childs.push_back(identifier_list_node);
						myroot->childs.push_back(colon_node);
						myroot->childs.push_back(type_node);
						myroot->childs.push_back(parameter_list_prime_node);
						return myroot;
					}
				}
			}
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode();
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::parameter_list()
{
	TreeNode* identifier_list_node = identifier_list();
	if (identifier_list_node != NULL)
	{
		token = nextToken();
		if (token.value == ":")
		{
			TreeNode* colon_node = new TreeNode(token.value);
			token = nextToken();
			TreeNode* type_node = type();
			if (type_node != NULL)
			{
				token = nextToken();
				TreeNode* parameter_list_prime_node = parameter_list_prime();
				if (parameter_list_prime_node != NULL)
				{
					TreeNode* myroot = new TreeNode("parameter_list");
					myroot->childs.push_back(identifier_list_node);
					myroot->childs.push_back(colon_node);
					myroot->childs.push_back(type_node);
					myroot->childs.push_back(parameter_list_prime_node);
					return myroot;
				}
			}
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::argumenrs()
{
	if (token.value == "(")
	{
		TreeNode* parantez_open_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* parameter_list_node = parameter_list();
		if (parameter_list_node != NULL)
		{
			token = nextToken();
			if (token.value == ")")
			{
				TreeNode* parantez_close_node = new TreeNode(token.value);
				TreeNode* myroot = new TreeNode("argumenrs");
				myroot->childs.push_back(parantez_open_node);
				myroot->childs.push_back(parameter_list_node);
				myroot->childs.push_back(parantez_close_node);
				return myroot;
			}
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode();
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::subprogram_head()
{
	if (token.value == "function")
	{
		TreeNode* function_node = new TreeNode(token.value);
		token = nextToken();
		if (token.type == "IDENTIFIER")
		{
			TreeNode* IDENTIFIER_node = new TreeNode(token.value);
			token = nextToken();
			TreeNode* argumenrs_node = argumenrs();
			if (argumenrs_node != NULL)
			{
				token = nextToken();
				if (token.value == ":")
				{
					TreeNode* colon_node = new TreeNode(token.value);
					token = nextToken();
					if (token.value == "result")
					{
						TreeNode* result_node = new TreeNode(token.value);
						token = nextToken();
						TreeNode* standard_type_node = standard_type();
						if (standard_type_node != NULL)
						{
							token = nextToken();
							if (token.value == ";")
							{
								TreeNode* semicolon_node = new TreeNode(token.value);
								TreeNode* myroot = new TreeNode("subprogram_head");
								myroot->childs.push_back(function_node);
								myroot->childs.push_back(IDENTIFIER_node);
								myroot->childs.push_back(argumenrs_node);
								myroot->childs.push_back(colon_node);
								myroot->childs.push_back(result_node);
								myroot->childs.push_back(standard_type_node);
								myroot->childs.push_back(semicolon_node);
								return myroot;
							}
						}
					}
				}
			}
		}
	}
	else if (token.value == "procedure")
	{
		TreeNode* procedure_node = new TreeNode(token.value);
		token = nextToken();
		if (token.type == "IDENTIFIER")
		{
			TreeNode* IDENTIFIER_node = new TreeNode(token.value);
			token = nextToken();
			TreeNode* argumenrs_node = argumenrs();
			if (argumenrs_node != NULL)
			{
				token = nextToken();
				if (token.value == ";")
				{
					TreeNode* semicolon_node = new TreeNode(token.value);
					TreeNode* myroot = new TreeNode("subprogram_head");
					myroot->childs.push_back(procedure_node);
					myroot->childs.push_back(IDENTIFIER_node);
					myroot->childs.push_back(argumenrs_node);
					myroot->childs.push_back(semicolon_node);
					return myroot;
				}
			}
		}
	}
	else
	{
		return NULL;
	}
	Error();
	return NULL;
}

TreeNode* Parser::subprogram_declaration()
{
	TreeNode* subprogram_head_node = subprogram_head();
	if (subprogram_head_node != NULL)
	{
		token = nextToken();
		TreeNode* declarations_node = declarations();
		if (declarations_node != NULL)
		{
			token = nextToken();
			TreeNode* compound_statement_node = compound_statement();
			if (compound_statement_node != NULL)
			{
				TreeNode* myroot = new TreeNode("subprogram_declaration");
				myroot->childs.push_back(subprogram_head_node);
				myroot->childs.push_back(declarations_node);
				myroot->childs.push_back(compound_statement_node);
				return myroot;
			}
		}
	}
	else
	{
		return NULL;
	}
	Error();
	return NULL;
}

TreeNode* Parser::subprogram_declarations()
{
	TreeNode* subprogram_declaration_node = subprogram_declaration();
	if (subprogram_declaration_node != NULL)
	{
		token = nextToken();
		TreeNode* subprogram_declarations_node = subprogram_declarations();
		if (subprogram_declarations_node != NULL)
		{
			TreeNode* myroot = new TreeNode("subprogram_declarations");
			myroot->childs.push_back(subprogram_declaration_node);
			myroot->childs.push_back(subprogram_declarations_node);
			return myroot;
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode();
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::array_type()
{
	if (token.value == "array")
	{
		TreeNode* array_node = new TreeNode(token.value);
		token = nextToken();
		if (token.value == "[")
		{
			TreeNode* bracket_open_node = new TreeNode(token.value);
			token = nextToken();
			if (token.type == "CONSTANT")
			{
				TreeNode* CONSTANT_node = new TreeNode(token.value);
				token = nextToken();
				if (token.value == "..")
				{
					TreeNode* dot_dot_node = new TreeNode(token.value);
					token = nextToken();
					if (token.type == "CONSTANT")
					{
						TreeNode* CONSTANT2_node = new TreeNode(token.value);
						token = nextToken();
						if (token.value == "]")
						{
							TreeNode* bracket_close_node = new TreeNode(token.value);
							token = nextToken();
							if (token.value == "of")
							{
								TreeNode* of_node = new TreeNode(token.value);
								token = nextToken();
								TreeNode* standard_type_node = standard_type();
								if (standard_type_node != NULL)
								{
									TreeNode* myroot = new TreeNode("array_type");
									myroot->childs.push_back(array_node);
									myroot->childs.push_back(bracket_open_node);
									myroot->childs.push_back(CONSTANT_node);
									myroot->childs.push_back(dot_dot_node);
									myroot->childs.push_back(CONSTANT2_node);
									myroot->childs.push_back(of_node);
									myroot->childs.push_back(standard_type_node);
									return myroot;
								}
							}
						}
					}
				}
			}
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::standard_type()
{
	if (token.value == "integer")
	{
		TreeNode* integer_node = new TreeNode(token.value);
		TreeNode* myroot = new TreeNode("standard_type");
		myroot->childs.push_back(integer_node);
		return myroot;
	}
	else if (token.value == "real")
	{
		TreeNode* real_node = new TreeNode(token.value);
		TreeNode* myroot = new TreeNode("standard_type");
		myroot->childs.push_back(real_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::type()
{
	TreeNode* standard_type_node = standard_type();
	if (standard_type_node != NULL)
	{
		TreeNode* myroot = new TreeNode("type");
		myroot->childs.push_back(standard_type_node);
		return myroot;
	}
	else
	{
		TreeNode* array_type_node = array_type();
		if (array_type_node != NULL)
		{
			TreeNode* myroot = new TreeNode("type");
			myroot->childs.push_back(array_type_node);
			return myroot;
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::declaration_list_prime()
{
	TreeNode* identifier_list_node = identifier_list();
	if (identifier_list_node != NULL)
	{
		if (token.value == ":")
		{
			TreeNode* colon_node = new TreeNode(token.value);
			token = nextToken();
			TreeNode* type_node = type();
			if (type_node != NULL)
			{
				token = nextToken();
				if (token.value == ";")
				{
					TreeNode* semicolon_node = new TreeNode(token.value);
					token = nextToken();
					TreeNode* declaration_list_prime_node = declaration_list_prime();
					if (declaration_list_prime_node != NULL)
					{
						TreeNode* myroot = new TreeNode("declaration_list_prime");
						myroot->childs.push_back(identifier_list_node);
						myroot->childs.push_back(colon_node);
						myroot->childs.push_back(type_node);
						myroot->childs.push_back(semicolon_node);
						myroot->childs.push_back(declaration_list_prime_node);
						return myroot;
					}
				}
			}
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode();
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::declaration_list()
{
	TreeNode* identifier_list_node = identifier_list();
	if (identifier_list_node != NULL)
	{
		token = nextToken();
		if (token.value == ":")
		{
			TreeNode* colon_node = new TreeNode(token.value);
			token = nextToken();
			TreeNode* type_node = type();
			if (type_node != NULL)
			{
				token = nextToken();
				if (token.value == ";")
				{
					TreeNode* semicolon_node = new TreeNode(token.value);
					token = nextToken();
					TreeNode* declaration_list_prime_node = declaration_list_prime();
					if (declaration_list_prime_node != NULL)
					{
						TreeNode* myroot = new TreeNode("declaration_list");
						myroot->childs.push_back(identifier_list_node);
						myroot->childs.push_back(colon_node);
						myroot->childs.push_back(type_node);
						myroot->childs.push_back(semicolon_node);
						myroot->childs.push_back(declaration_list_prime_node);
						return myroot;
					}
				}
			}
		}
	}
	Error();
	return NULL;
}

TreeNode* Parser::declarations()
{
	if (token.value == "var")
	{
		TreeNode* var_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* declaration_list_node = declaration_list();
		if (declaration_list_node != NULL)
		{
			TreeNode* myroot = new TreeNode("declarations");
			myroot->childs.push_back(var_node);
			myroot->childs.push_back(declaration_list_node);
			return myroot;
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode();
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::identifier_list_prime()
{
	if (token.value == ",")
	{
		TreeNode* comma_node = new TreeNode(token.value);
		token = nextToken();
		if (token.type == "IDENTIFIER")
		{
			TreeNode* IDENTIFIER_node = new TreeNode(token.value);
			token = nextToken();
			TreeNode* identifier_list_prime_node = identifier_list_prime();
			if (identifier_list_prime_node != NULL)
			{
				TreeNode* myroot = new TreeNode("identifier_list_prime");
				myroot->childs.push_back(comma_node);
				myroot->childs.push_back(IDENTIFIER_node);
				myroot->childs.push_back(identifier_list_prime_node);
				return myroot;
			}
		}
	}
	else
	{
		see_epsilon = true;
		TreeNode* epsilon_node = new TreeNode(EPSILON + "");
		TreeNode* myroot = new TreeNode();
		myroot->childs.push_back(epsilon_node);
		return myroot;
	}
	Error();
	return NULL;
}

TreeNode* Parser::identifier_list()
{
	if (token.type == "IDENTIFIER")
	{
		TreeNode* IDENTIFIER_node = new TreeNode(token.value);
		token = nextToken();
		TreeNode* identifier_list_prime_node = identifier_list_prime();
		if (identifier_list_prime_node != NULL)
		{
			TreeNode* myroot = new TreeNode("identifier_list");
			myroot->childs.push_back(IDENTIFIER_node);
			myroot->childs.push_back(identifier_list_prime_node);
			return myroot;
		}
	}
	else
	{
		// Error();
		return NULL;
	}
}

TreeNode* Parser::program()
{
	if (token.value == "program")
	{
		TreeNode* program_node = new TreeNode(token.value);
		token = nextToken();
		if (token.type == "IDENTIFIER")
		{
			TreeNode* IDENTIFIER_node = new TreeNode(token.value);
			token = nextToken();
			if (token.value == "(")
			{
				TreeNode* parantez_open_node = new TreeNode(token.value);
				token = nextToken();
				TreeNode* identifier_list_node = identifier_list();
				if (identifier_list_node != NULL)
				{
					token = nextToken();
					if (token.value == ")")
					{
						TreeNode* parantez_close_node = new TreeNode(token.value);
						token = nextToken();
						if (token.value == ";")
						{
							TreeNode* semicolon_node = new TreeNode(token.value);
							token = nextToken();
							TreeNode* declarations_node = declarations();
							if (declarations_node != NULL)
							{
								token = nextToken();
								TreeNode* subprogram_declarations_node = subprogram_declarations();

								if (subprogram_declarations_node != NULL)
								{
									token = nextToken();
									TreeNode* compound_statement_node = compound_statement();

									if (compound_statement_node != NULL)
									{
										TreeNode* myroot = new TreeNode("program");
										myroot->childs.push_back(program_node);
										myroot->childs.push_back(IDENTIFIER_node);
										myroot->childs.push_back(parantez_open_node);
										myroot->childs.push_back(identifier_list_node);
										myroot->childs.push_back(parantez_close_node);
										myroot->childs.push_back(semicolon_node);
										myroot->childs.push_back(declarations_node);
										myroot->childs.push_back(subprogram_declarations_node);
										myroot->childs.push_back(compound_statement_node);
										return myroot;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	Error();
	return NULL;
}

void Parser::createHtmlTree(TreeNode* x)
{
	html_tree += "<table border=\"1px\"><tr><td colspan=\"100%\">";
	html_tree += x->data;
	html_tree += "</td></tr>";
	html_tree += "<tr>";

	for(TreeNode* tree_node : x->childs) 
	{
		html_tree += "<td>";
		createHtmlTree(tree_node);
		html_tree += "</td>";
	}
	
	html_tree += "</tr></table>";
}

void Parser::writeHtmlTreeToFile()
{
	fstream f;
	f.open("IO/output/tree.html", ios::out);

	if (!f)
	{
		cout << "no file available\n";
		system("pause");
		exit(0);
	}

	html_tree = "<html><head></head><body>";
	createHtmlTree(root);
	html_tree += "</body></html>";

	for (int i = 0; i < html_tree.size(); i++)
	{
		f << html_tree[i];
	}

	f.close();
}

char depth[2056];
int di = 0;

void Parser::printTree(TreeNode* x)
{
	cout << x->data << endl;

	for(TreeNode* tree_node : x->childs) 
	{
		cout << depth << " +--";

		if (tree_node != x->childs.back()) // check if node is the last child
		{
			depth[di++] = ' ';
			depth[di++] = '|';
			depth[di++] = ' ';
			depth[di] = 0;
		}
		else
		{
			depth[di++] = ' ';
			depth[di++] = ' ';
			depth[di++] = ' ';
			depth[di] = 0;
		}

		printTree(tree_node);
		depth[di -= 3] = 0;
	}
}
