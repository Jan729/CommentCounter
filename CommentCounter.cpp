/* CommentCounter.cpp accepts file names, then scans those files and
 * reports the total number of:
 * 1) Lines
 * 2) Comment lines (lines with both code and a comment are included)
 * 3) Single comment lines
 * 4) Comment lines within block comments
 * 5) Block line comments
 * 6) TODOs

 * Assumptions:
 * 1) User will enter the name of the file they wish to check, one file per line
		(subsequent file names will be ignored)
 * 2) Output will appear on the console
 * 3) Files won't contain nested block comments
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

constexpr auto NIL = "NIL";
using namespace std;
namespace fs = experimental::filesystem;

/* Helper function prototypes */
void getCommentSyntax(const string& extension,
	string & singleCommSym, string& blkCommSym, string& blkCommSymEnd);

/* Helper function implementations*/
void getCommentSyntax(const string& extension,
	string& singleCommSym, string& blkCommSym, string& blkCommSymEnd) {

	//By default, if the given extension isn't in this list,
	//the program will look for // and /**/ when searching for comments
	string defaultSingleComment = "//";
	string defaultBlockComment = "/*";
	string defaultBlockCommentEnd = "*/";

	//Here is my humble attempt at checking for programming files
	//that don't use // and /**/ for comments.

	//Entries are organized like this:
	//{"file extension", "single line comment", "start of block comment", "end of block comment"}
	//languages that don't support single or block comments may contain 'NIL' entries
	vector<vector<string>> languages = {
		{".ada", "--", NIL, NIL},
		{".asm", ";", "/*", "*/"},
		{".html", NIL, "<!--", "--!>"},
		{".py", "#", NIL, NIL}, //ignore docstrings
		{".pl", "#", NIL, NIL},
		{".ruby", "#", "=begin" , "=end"},
		{".tcl", "#", NIL, NIL},
		{".sh", "#", NIL, NIL},
		{".sql", "#", "/*", "*/", "--"}, //SQL also uses -- for single line comments. hmm.
		{".vb", "'", NIL, NIL}
	};

	for (vector<vector<string>>::iterator langPos = languages.begin(); langPos != languages.end(); langPos++)
		if (*(langPos ->begin()) == extension) {
			singleCommSym = *(langPos -> begin() + 1);
			blkCommSym = *(langPos ->begin() + 2);
			blkCommSymEnd = *(langPos ->begin() + 3);
			return;
		}

	//if language not found in list, use default comment symbols
	singleCommSym = defaultSingleComment;
	blkCommSym = defaultBlockComment;
	blkCommSymEnd = defaultBlockCommentEnd;
}

int main() {
	//declare variables
	string fileName;
	string extension = "";
	vector<string>::iterator extPos;
	ifstream inFile;
	string line;

	int numLines = 0;
	int numCommentLines = 0;
	int singleCommLines = 0;
	int blkCommLines = 0;
	int blkComments = 0;
	int numTODOs = 0;
	int currentBlkLineCount = 0;
	bool insideBlkComment = false;
	string nil = "NIL";
	string TODO = "TODO";

	string singleCommSym;
	string blkCommSym;
	string blkCommSymEnd;
	
	//Repeatedly ask user for a file name, then scan the file and print results.
	//To quit the loop, enter ctrl-d.
	do {
		bool isValidInput = false;

		//Program will keep prompting for input until it receives a valid file name.
		//Invalid inputs will be ignored.
		while (!isValidInput) {
			cout << "Please enter the name of your file:\n>> ";
			cin >> fileName;
			
			if (cin.fail()) {
				cin.clear();
				cin.ignore(1000, '\n'); //Flush input
			}
			else
				extension = fs::path(fileName).extension().string();
			

			if (extension != "") { //found an extension
				getCommentSyntax(extension, singleCommSym, blkCommSym, blkCommSymEnd);
				
				inFile.open(fileName, ifstream::in);

				if (!inFile) {
					inFile.clear();
					cerr << "Sorry, can't open file." << endl;
				} else
					isValidInput = true;
			}
			if (!isValidInput)
				cout << "Invalid file name. Please try again." << endl;

		} //invalid input loop
		
		getline(inFile, line);

		//for each line in the file, count comment lines and TODOs
		while (inFile) {
			
			for (int idx = 0; idx < (line.length() - 3); idx++) {

				if ((singleCommSym != NIL) && (line.substr(idx, idx + 2) == singleCommSym))
					singleCommLines++;

				if ((blkCommSym != NIL) && (line.substr(idx, idx + 2) == blkCommSym)) {
					blkCommLines++;
					blkComments++;
					insideBlkComment = true;
				}

				if ((blkCommSym != NIL) && (line.substr(idx, idx + 2) == blkCommSymEnd))
					insideBlkComment = false;

				//check for TODO
				if ((idx + TODO.size() < line.size()) && (line.substr(idx, idx + TODO.size() + 1) == TODO))
					numTODOs++;
			}

			if (insideBlkComment)
				currentBlkLineCount++;

			numLines++;
			getline(inFile, line);
		}

		inFile.close();

		cout << "Total # of lines: " << numLines << endl;
		cout << "Total # of comment lines: " << numCommentLines << endl;
		cout << "Total # of single line comments: " << singleCommLines << endl;
		cout << "Total # of comment lines within block comments: " << blkCommLines << endl;
		cout << "Total # of block line comments: " << blkComments << endl;
		cout << "Total # of TODO's: " << numTODOs << endl;

		cout << "To scan another file, enter any key to continue. To quit, enter ctrl-d.\n>> ";
		string input;
		cin >> input;

	} while (!cin.eof());
}