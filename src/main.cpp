#include "code.h"
#include "parser.h"
#include "preprocessor.h"
int main(int argc, char* argv[])
{
	string inputName, outputName;
	if (argc > 1)
		inputName = argv[1];
	else
	{
		cout << "Command line parameter should be name of input file" << endl;
		return -1;
	}

	// calculate output file name
	outputName = inputName;
	outputName.pop_back();
	outputName.pop_back();
	outputName += "asm";

	Preprocessor preprocessor(inputName);
	vector<string> inLines = preprocessor.preprocess();		//preprocess input file
	Parser parser;
	vector<vector<string>> fields = parser.parse(inLines);			//parse
	Code code(outputName);												//build code tables
	code.generateCode(fields);	//do translation

	cout << "Output file is " << outputName << endl;
	return 0;
}