#include "code.h"
vector<string> Code::split(const string& str, char c)  //split a string 
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
	if (temp.size())
		result.push_back(temp);
	return result;
}

string Code::extractFileName(const string& fileName)
{
	vector<string> strs = split(fileName,'\\');  
	for(int i=0;i<4;i++)			// pop .asm
		strs[strs.size()-1].pop_back();
	return strs[strs.size() - 1];
}


Code::Code(string outputFileName)
{
	compAriNumber = 0;
	this->outputFileName = extractFileName(outputFileName);
	outputFile.open(outputFileName);

	codeTable["add"] = { "@SP","AM = M - 1","D = M","A = A - 1","M = M + D" };
	codeTable["sub"] = { "@SP","AM = M - 1","D = M","A = A - 1","M = M - D" };
	codeTable["neg"] = { "@SP","A=M-1","M=-M" };
	codeTable["eq"] = { "@SP","AM = M - 1","D = M","A = A - 1","D = M - D","@FALSE","D;JNE","@SP","A=M-1","M=-1",
						"@CONTINUE","0;JMP","FALSE","@SP","A=M-1","M=0","CONTINUE" };
	codeTable["gt"] = { "@SP","AM = M - 1","D = M","A = A - 1","D = M - D","@FALSE","D;JLE","@SP","A=M-1","M=-1",
						"@CONTINUE","0;JMP","FALSE","@SP","A=M-1","M=0","CONTINUE" };
	codeTable["lt"] = { "@SP","AM = M - 1","D = M","A = A - 1","D = M - D","@FALSE","D;JGE","@SP","A=M-1","M=-1",
						"@CONTINUE","0;JMP","FALSE","@SP","A=M-1","M=0","CONTINUE" };
	codeTable["and"] = { "@SP","AM = M - 1","D = M","A = A - 1","M = M & D" };
	codeTable["or"] = { "@SP","AM = M - 1","D = M","A = A - 1","M = M | D" };
	codeTable["not"] = { "@SP","A=M-1","M=!M" };
	

}
Code::~Code()
{
	outputFile.close();
}

void Code::generateArithmetic(string operation)
{
	vector<string> outlines = codeTable[operation];
	for (string outLine : outlines)
	{

		if (outLine == "@FALSE" || outLine == "@CONTINUE")
			outLine += to_string(compAriNumber);
		else if (outLine == "FALSE" || outLine == "CONTINUE")
			outLine = "(" + outLine + to_string(compAriNumber) + ")";
		outputFile << outLine << endl;
	}

	if (operation == "eq" || operation == "gt" || operation == "lt")
		compAriNumber++;
}

void Code::generateMemAccess(string operation, string segment, string offset)
{
	if (segment == "constant")
		generatConstant(offset);
	else if (segment == "local" || segment == "argument" || segment == "this" || segment == "that")
		generatLATT(operation, segment, offset);
	else if (segment == "static")
		generatStatic(operation, offset);
	else if (segment == "pointer")
		generatPointer(operation, offset);
	else if (segment == "temp")
		generatTemp(operation, offset);
}


void Code::generatLATT(string operation, string segment, string offset)  //segment=local,argument,this,that
{
	string outSegment = "";
	if (segment == "local")
		outSegment = "LCL";
	else if (segment == "argument")
		outSegment = "ARG";
	else if (segment == "this")
		outSegment = "THIS";
	else if (segment == "that")
		outSegment = "THAT";

	if (operation == "C_PUSH")
	{
		vector<string> outLines = {"@","D = M","@","A = D + A","D=M","@SP","A=M","M=D","@SP","M=M+1"};
		outLines[0] += outSegment;
		outLines[2] += offset;
		output(outLines);
	}
	else if (operation == "C_POP")
	{
		vector<string> outLines = { "@","D = M","@","D = D + A","@R13","M=D","@SP","AM=M-1","D=M","@R13","A=M","M=D" };
		outLines[0] += outSegment;
		outLines[2] += offset;
		output(outLines);
	}
}

void Code::generatConstant(string offset)								//segment=constant
{
	outputFile << "@" + offset << endl;
	outputFile << "D=A" << endl;
	outputFile << "@SP" << endl;
	outputFile << "A=M" << endl;
	outputFile << "M=D" << endl;
	outputFile << "@SP" << endl;
	outputFile << "M=M+1" << endl;
}

void Code::generatStatic(string operation, string offset)  //segment=static
{
	if (operation == "C_PUSH")
	{
		vector<string> outLines = { "@","D=M","@SP","A=M","M=D","@SP","M=M+1" };
		outLines[0] += (outputFileName + "." + offset); //			@paddle.5 for example
		output(outLines);

	}
	else
	{
		vector<string> outLines = { "@SP","AM=M-1","D=M","@","M=D" };
		outLines[3] += (outputFileName + "." + offset); //			@paddle.5 for example
		output(outLines);
	}
}
void Code::generatPointer(string operation, string offset)  //segment=pointer
{
	string pointer;
	if (offset == "0")
		pointer = "THIS";
	else
		pointer = "THAT";
	if (operation == "C_PUSH")
	{
		vector<string> outLines = { "@","D=M","@SP","A=M","M=D","@SP","M=M+1" };
		outLines[0] += pointer;
		output(outLines);

	}
	else
	{
		vector<string> outLines = { "@SP","AM=M-1","D=M","@","M=D" };
		outLines[3] += pointer;
		output(outLines);
	}
}
void Code::generatTemp(string operation, string offset)  //segment=temp
{
	string segment = to_string(stoi(offset)+5);
	if (operation == "C_PUSH")
	{
		vector<string> outLines = { "@","D=M","@SP","A=M","M=D","@SP","M=M+1" };
		outLines[0] += segment; 		
		output(outLines);

	}
	else
	{
		vector<string> outLines = { "@SP","AM=M-1","D=M","@","M=D" };
		outLines[3] += segment;
		output(outLines);
	}
}
void Code::generateCode(const vector<vector<string>>& fields)
{
	for (auto onelineField : fields)
	{
		string code;
		if (onelineField[0] == "C_PUSH"|| onelineField[0] == "C_POP")
		{
			generateMemAccess(onelineField[0], onelineField[1], onelineField[2]);
		}
		else if (onelineField[0] == "C_ARITHMETIC")
		{
			generateArithmetic(onelineField[1]);
		}

	}
}
void Code::output(const vector<string>& outLines)
{
	for (string outLine : outLines)
	{
		outputFile << outLine << endl;
	}

}