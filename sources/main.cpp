/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
*  Pascal recursive descent parser
*  Copyright (C) 2017 SajjadAemmi ( Telegram: @Sajjad_Aemmi )
*
** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/
#include <iostream>
#include "lex_analyser.hpp"
#include "parser.hpp"

using namespace std;

int main()
{
	LexAnalyser LA;
	Parser PA;

	cout << "LexAnalyser" << endl;
	LA.readInfixREFromFile();
	LA.preProcessInfixRE();
	LA.convertInfixREToPostfixRE();
	LA.writePostfixREToFile();
	LA.convertPostfixREToNFA();
	LA.writeNFATableToFile();
	LA.convertNFAToDFA();
	LA.readProgramFromFile();
	LA.scanner();
	LA.writeTokensToFile();

	cout << "Parser" << endl;
	PA.tokens = LA.tokens;
	PA.createTree();

	if (PA.good_program == true)
	{
		PA.writeHtmlTreeToFile();
		PA.printTree(PA.root);
	}

	return 0;
}
