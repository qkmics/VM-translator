#include "code.h"

Code::Code(string outputFileName)
{
	compAriNumber = 0;
	returnNumber = 0;
	funcCallNumber = 0;
	this->functionName = "";
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

void Code::bootstrap()
{
	vector<string> lines = {"@256","D = A","@SP","M = D"};
	output(lines);
	generateFunctionCall("Sys.init", "0");
}

/*
	Generate assembler code from the underlying fields of code produced by parser

	@para Underlying fields of code produced by parser.
*/
void Code::generateCode(const vector<vector<string>>& fields)
{
	bootstrap();
	for (auto onelineField : fields)
	{
		string code;
		if (onelineField[0] == "C_PUSH" || onelineField[0] == "C_POP")
		{
			generateMemAccess(onelineField[0], onelineField[1], onelineField[2]);
		}
		else if (onelineField[0] == "C_ARITHMETIC")
		{
			generateArithmetic(onelineField[1]);
		}
		else if (onelineField[0] == "C_GOTO" || onelineField[0] == "C_IF-GOTO" || onelineField[0] == "C_LABEL")
		{
			generateProgramFlow(onelineField[0], onelineField[1]);
		}
		else if (onelineField[0] == "C_CALL")
		{
			generateFunctionCall(onelineField[1], onelineField[2]);
		}
		else if (onelineField[0] == "C_RETURN")
		{
			generateFunctionReturn();
		}
		else if (onelineField[0] == "C_FUNCTION")
		{
			generateFunctionDef(onelineField[1], onelineField[2]);
		}
	}
}

/*
	Generate an program flow instruction, including goto label, if-goto label and (label).

	@para operation A string of operation. 
	@para label
*/
void Code::generateProgramFlow(string operation, string label)
{
	//important, different functions need to have different label contexts
	label = this->functionName + "." + label;

	if (operation == "C_GOTO")
	{
		outputFile << "@" << label << endl;
		outputFile << "0;JMP" << endl;
	}
	else if (operation == "C_IF-GOTO")
	{
		outputFile << "@SP" << endl;
		outputFile << "AM = M - 1" << endl;
		outputFile << "D = M" << endl;
		outputFile << "@" << label << endl;
		outputFile << "D;JNE" << endl;
	}
	else if(operation == "C_LABEL")
	{
		outputFile << "(" << label << ")" << endl;
	}
}

/*
	Generate an function define instruction.

	@para name function name
	@para locNum number of local variable
*/
void Code::generateFunctionDef(string name, string locNum)
{
	this->functionName = name;   //store this function name, used for label

	outputFile << "(" << name << ")" << endl;

	int number = stoi(locNum);
	for (int i = 0;i < number;i++)
	{
		outputFile << "@SP" << endl;
		outputFile << "A = M" << endl;
		outputFile << "M = 0" << endl;
		outputFile << "@SP" << endl;
		outputFile << "M = M + 1" << endl;
	}
}

/*
	Generate an return instruction.
*/
void Code::generateFunctionReturn()
{
	vector<string> outLines = {"@LCL","D = M","@Frame","M = D","D = D -1"
								,"D = D-1","D = D-1","D = D-1","D = D-1","A = D",
								"D = M","@RET","M = D","@SP","M = M - 1","A = M",
								"D = M","@ARG","A = M","M = D","@ARG","D = M + 1",
								"@SP","M = D","@Frame","AD = M - 1",
								"D = M","@THAT","M = D","@Frame","D = M-1",
								"AD = D - 1","D = M","@THIS","M = D","@Frame",
								"D = M-1","D = D -1","AD = D -1","D = M","@ARG","M = D",
								"@Frame","D = M - 1","D = D -1","D = D -1","AD = D -1","D = M","@LCL",
								"M = D","@RET","A = M","0;JMP" };
	for (string outLine : outLines)
	{
		if (outLine == "@Frame" || outLine == "@RET")
			outputFile << outLine + to_string(returnNumber) << endl;
		else
			outputFile << outLine << endl;
	}
	returnNumber++;
}

/*
	Generate an function call construction.

	@para name function name
	@para locNum number of parameter
*/
void Code::generateFunctionCall(string name, string paraNum)
{
	vector<string> outLinesP1 = { "@f.return.address","D = A","@SP","A = M","M = D","@SP","M = M+ 1"};
	vector<string> outLinesP2 = { "D = M","@SP","A = M ","M = D","@SP","M = M + 1", };
	vector<string> outLinesP3 = { "@ (n+5)","D = A ","@SP" ,"D = M - D" ,"@ARG" ,"M = D" ,"@SP" ,"D = M" ,"@LCL" ,"M = D" ,"@f" ,"0;JMP" ,"(f.return.address)" };
	
	for (string outLine : outLinesP1)
	{
		if (outLine == "@f.return.address")
			outputFile << "@" + name + ".return.address" << funcCallNumber << endl;
		else
			outputFile << outLine << endl;
	}

	vector<string> segs = { "@LCL","@ARG","@THIS","@THAT" };
	for (int i = 0;i < segs.size();i++)
	{
		outputFile << segs[i] << endl;
		for (string outLine : outLinesP2)
		{
			outputFile << outLine << endl;
		}
	}
	
	for (string outLine : outLinesP3)
	{
		if (outLine == "@ (n+5)")
			outputFile << "@" + to_string(stoi(paraNum) + 5) << endl;
		else if (outLine == "@f")
			outputFile << "@" + name << endl;
		else if (outLine == "(f.return.address)")
			outputFile << "(" + name + ".return.address" << funcCallNumber << ")" << endl;
		else
			outputFile << outLine << endl;
	}

	funcCallNumber++;
}

/*
	Generate an arithmetic operation. Including add, sub, neg, eq, gt, lt, and, or, not
	
	@para operation A string of operation. Such as "add", "sub"
*/
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

/*
	Generate an memory access operation. 

	@para operation "C_PUSH" or "C_POP"
	@para segment "constant", "local", "static", and so on. 
	@para offset A integer used to add to the segment to get the real address.
*/
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

//segment = local, argument, this, that
void Code::generatLATT(string operation, string segment, string offset)  
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

//segment=constant
void Code::generatConstant(string offset)								
{
	outputFile << "@" + offset << endl;
	outputFile << "D=A" << endl;
	outputFile << "@SP" << endl;
	outputFile << "A=M" << endl;
	outputFile << "M=D" << endl;
	outputFile << "@SP" << endl;
	outputFile << "M=M+1" << endl;
}

//segment=static
void Code::generatStatic(string operation, string offset)  
{
	if (operation == "C_PUSH")
	{
		vector<string> outLines = { "@","D=M","@SP","A=M","M=D","@SP","M=M+1" };
		outLines[0] += ("static." + offset); //			@paddle.5 for example
		output(outLines);

	}
	else
	{
		vector<string> outLines = { "@SP","AM=M-1","D=M","@","M=D" };
		outLines[3] += ("static." + offset); //			@paddle.5 for example
		output(outLines);
	}
}

//segment=pointer
void Code::generatPointer(string operation, string offset) 
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

//segment=temp
void Code::generatTemp(string operation, string offset)  
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

//Output the generated assembler code to the output file
void Code::output(const vector<string>& outLines)
{
	for (string outLine : outLines)
	{
		outputFile << outLine << endl;
	}

}

//split a string 
vector<string> Code::split(const string& str, char c)
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
	vector<string> strs = split(fileName, '\\');
	for (int i = 0;i < 4;i++)			// pop .asm
		strs[strs.size() - 1].pop_back();
	return strs[strs.size() - 1];
}
