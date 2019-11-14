#include <string>
#include <vector>
using namespace std;

class Parser
{
public:
	Parser(string inputFile);

	vector<vector<string>> parse(vector<string> lines);

private:
	string inputFile;

	vector<string> parseOneLine(string line);

	vector<string> split(const string& str, char c);  //split a string 
};