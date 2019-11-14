#include "code.h"
#include "parser.h"
#include "preprocessor.h"
#include <filesystem>
//split a string 
vector<string> split(const string& str, char c)
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
string extractFileName(const string& fileName)
{
	vector<string> strs = split(fileName, '\\');
	for (int i = 0;i < 3;i++)			// pop .vm
		strs[strs.size() - 1].pop_back();
	return strs[strs.size() - 1];
}

vector<string> getFileName(string inputName, string& outputName)
{
	vector<string> fileNames;
	if (inputName.size() > 3 && inputName.substr(inputName.size() - 3) == ".vm") //singel file
	{
		fileNames.push_back(inputName);
		outputName = inputName.substr(0, inputName.size() - 2) + "asm";
	}
	else
	{
		//directory

		std::string path = inputName;
		outputName = inputName + "\\"  + *(split(path, '\\').end() - 1) + ".asm";
		for (const auto& entry : filesystem::directory_iterator(path))
		{
			string fileName = entry.path().string();
			if (fileName.size() > 3 && fileName.substr(fileName.size() - 3) == ".vm")
				fileNames.push_back(fileName);
		}
	}
	return fileNames;
}
int main(int argc, char* argv[])
{
	string inputName, outputName;
	if (argc > 1)
		inputName = argv[1];
	else
	{
		cout << "Command line parameter should be name of input file or fold" << endl;
		return -1;
	}

	vector<string> inputFiles = getFileName(inputName, outputName);

	vector<vector<string>> fields;
	for (string inputFile : inputFiles)
	{
		Preprocessor preprocessor(inputFile);
		vector<string> inLines = preprocessor.preprocess();				//preprocess input file
		Parser parser(extractFileName(inputFile));
		vector<vector<string>> fieldsOneFile = parser.parse(inLines);			//parse
		for (vector<string> field : fieldsOneFile)
			fields.push_back(field);
	}
	Code code(outputName);											//build code tables
	code.generateCode(fields);										//do translation
	
	cout << "Output file is " << outputName << endl;
	return 0;
}