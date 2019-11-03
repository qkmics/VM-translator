#include "preprocessor.h"

Preprocessor::Preprocessor(string inputFileName)
{
	inputFile.open(inputFileName);
	if (inputFile.is_open())
	{
		;
	}
	else
	{
		cout << "no such file" << endl;
	}

}
Preprocessor::~Preprocessor()
{

	//close the input file
	inputFile.close();
}


/*@return code without comments and spaces*/
vector<string> Preprocessor::preprocess(void)
{
	//open the input file and output file
	vector<string> outputs;

	int state = 0;
	string inputLine;
	string outputLine;

	while (getline(inputFile, inputLine))  //get a line from the input file
	{
		outputLine = "";
		if (state == 2)       //handle one line comments
			state = 0;
		for (char ele : inputLine)
		{
			switch (state)     //the automaton has 5 states
			{
			case 0:
				if (ele == '/')						//it might be the start of a commemt
					state = 1;
				else if (ele != '\t')				//space should not be moved
					outputLine.push_back(ele);
				break;

			case 1:
				if (ele == '/')						//one line comment
					state = 2;
				else if (ele == '*')				//could be a comment
					state = 3;
				else
				{
					state = 0;
					outputLine.push_back('/');
					outputLine.push_back(ele);
				}
				break;

			case 2:						//the whole line is an one line comment, so do nothing 
				break;

			case 3:
				if (ele == '*')
					state = 4;
				break;

			case 4:
				if (ele == '/')
					state = 0;
				else if (ele != '*')
					state = 3;
				break;
			}

		}

		if (outputLine.size() > 1)	//this input line is not part of a comment
		{
			outputs.push_back(outputLine);
		}
	}
	
	
	return outputs;

}