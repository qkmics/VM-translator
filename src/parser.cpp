#include "parser.h"
vector<vector<string>> Parser::parse(vector<string> lines)
{
	vector<vector<string>> fields;
	for (int i = 0;i < lines.size();i++)
		fields.push_back(parseOneLine(lines[i]));
	return fields;
}
vector<string> Parser::split(const string& str, char c)  //split a string 
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
vector<string> Parser::parseOneLine(string line)
{
	vector<string> parts = split(line, ' ');  
	vector<string> fields;

	if (parts[0] == "push")		//push instruction
	{
		fields.push_back("C_PUSH");
		fields.push_back(parts[1]);
		fields.push_back(parts[2]);
	}
	else if (parts[0] == "pop")	//pop instruction
	{
		fields.push_back("C_POP");
		fields.push_back(parts[1]);
		fields.push_back(parts[2]);
	}
	else				
	{
		fields.push_back("C_ARITHMETIC");
		fields.push_back(parts[0]);
	}
	return fields;
}