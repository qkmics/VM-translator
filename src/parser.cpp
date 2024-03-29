#include "parser.h"

Parser::Parser(string inputFile)
{
	this->inputFile = inputFile;
}

/*
	Parse all of the code input, return code of underlying fields
	
	@param lines Vm code after preprocessing
	@return Code after parsing. All in underlying fields
*/
vector<vector<string>> Parser::parse(vector<string> lines)
{
	vector<vector<string>> fields;
	for (int i = 0;i < lines.size();i++)
		fields.push_back(parseOneLine(lines[i]));
	return fields;
}

//Split a string, returns the vector of string after spliting 
vector<string> Parser::split(const string& str, char c)
{
	vector<string> result;
	string temp = "";
	for (int i = 0;i < str.size();i++)
	{
		if (str[i] != c)
		{
			temp.push_back(str[i]);
		}
		else
		{
			result.push_back(temp);
			temp = "";
		}
	}
	if(temp.size())
		result.push_back(temp);
	return result;
}

//Parse one line of code
vector<string> Parser::parseOneLine(string line)
{
	vector<string> parts = split(line, ' ');  
	vector<string> fields;

	if (parts[0] == "push")		//push instruction
	{
		fields.push_back("C_PUSH");
		fields.push_back(parts[1]);
		if (parts[1] == "static")
			fields.push_back(inputFile + "." + parts[2]);
		else
			fields.push_back(parts[2]);
	}
	else if (parts[0] == "pop")	//pop instruction
	{
		fields.push_back("C_POP");
		fields.push_back(parts[1]);
		if (parts[1] == "static")
			fields.push_back(inputFile + "." + parts[2]);
		else
			fields.push_back(parts[2]);
	}
	else if (parts[0] == "label") //lable instruction
	{
		fields.push_back("C_LABEL");
		fields.push_back(parts[1]);
	}
	else if (parts[0] == "goto")
	{
		fields.push_back("C_GOTO");
		fields.push_back(parts[1]);
	}
	else if (parts[0] == "if-goto")
	{
		fields.push_back("C_IF-GOTO");
		fields.push_back(parts[1]);
	}
	else if (parts[0] == "function")
	{
		fields.push_back("C_FUNCTION");
		fields.push_back(parts[1]);
		fields.push_back(parts[2]);
	}
	else if (parts[0] == "call")
	{
		fields.push_back("C_CALL");
		fields.push_back(parts[1]);
		fields.push_back(parts[2]);
	}
	else if (parts[0] == "return")
	{
		fields.push_back("C_RETURN");
	}
	else				
	{
		fields.push_back("C_ARITHMETIC");
		fields.push_back(parts[0]);
	}
	return fields;
}