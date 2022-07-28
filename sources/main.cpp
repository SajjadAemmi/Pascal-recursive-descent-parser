/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
*  Pascal recursive descent parser
*  Copyright (C) 2017 SajjadAemmi ( Telegram: @Sajjad_Aemmi )
*
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/
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
#include "lex_analyser.hpp"
#include "parser.hpp"

using namespace std;

#define EPSILON			-1
#define CLOSURE			'*'
#define OPEN_PAREN		'('
#define CLOSE_PAREN		')'
#define OR				'+'
#define CONCAT			'.'


int main()
{
	LexAnalyser LA;
	Parser PA;

	cout << "LexAnalyser" << endl;
	LA.InputFileInfixRE();
	LA.PreProcessInfixRE();
	LA.ConvertInfixREToPostfixRE();
	LA.OutputFilePostfixRE();
	LA.ConvertPostfixREToNFA();
	LA.OutputFileNFATable();
	LA.ConvertNFAtoDFA();
	LA.InputFileProgram();
	LA.Scanner();
	LA.OutputFileTokens();

	cout << "Parser" << endl;
	PA.Tokens = LA.Tokens;
	PA.CreateTree();

	if (PA.GoodProgram == true)
	{
		PA.OutputFileHTMLTree();
		PA.PrintTree(PA.root);
	}

	return 0;
}
