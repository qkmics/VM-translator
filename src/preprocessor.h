/* remove comments 
*/
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace::std;
class Preprocessor
{
public:
	Preprocessor(string inputFileName);
	~Preprocessor();

	/*@return code without comments and spaces except space*/
	vector<string> preprocess(); // use finite automaton

private:
	ifstream inputFile;
};
