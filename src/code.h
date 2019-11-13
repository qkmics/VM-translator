#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
using namespace std;
class Code
{
public:
	
	Code(string outputFileName);

	~Code();
	
	void generateCode(const vector<vector<string>>& fields);
	
private:
	ofstream outputFile;

	string outputFileName;

	// It is used for "eq", "gt" and "lt". Translating these vm language code needs lables in assemble language 
	// code. It is used to make sure that these lables do not repeat.
	int compAriNumber;

	int compReturnNumber;

	unordered_map<string, vector<string>> codeTable;
	
	void output(const vector<string>& outLines);

	void generateArithmetic(string operation);

	void generateMemAccess(string operation, string segment, string offset);

	void generateProgramFlow(string operation, string label);

	void generateFunctionCall(string operation, string label);

	void generateFunctionReturn();

	void generateFunctionDef(string operation, string label);



	void generatConstant(string offset);								//segment=constant

	void generatLATT(string operation, string segment, string offset);  //segment=local,argument,this,that

	void generatStatic(string operation,string offset);  //segment=static

	void generatPointer(string operation, string offset);  //segment=pointer

	void generatTemp(string operation, string offset);  //segment=temp

	string extractFileName(const string& fileName);

	vector<string> split(const string& str, char c); 
};