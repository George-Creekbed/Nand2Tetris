// Assembler for Nand 2 tetris chapter 6, transforms .asm assembly files into .hack machine language rom files
#include <iostream>
#include <fstream>
#include <varargs.h>
#include <cstring>
#include <string>
#include <stdlib.h> 
using namespace std;

class parser
{
	/* encapsulates (breaks into basic units of information, identifies role of such information)
	original assembly code in file .asm */
private:
	char inputname[30];
public:
	bool endfile, ignore;
	int count;
	bool a_command, l_command, c_command; 
	string destParsedLine, compParsedLine, jumpParsedLine;
	string parsedLine;
	ifstream inputfile;
	char linechar[256];
	string line;

	// default parser constructor
	parser()
	{
		endfile = false;
		ignore = false;
		count = 0;
	}
	//parser(char* argv[])
	//{
	//	reader(argv[]);
	//}

	// ignores inputfile line if comment or empty
	bool ignoreline()
	{
		ignore = false;
		if (line[0] == '/' || line.empty())
		{
			//inputfile.ignore(line.length(),'\n');
		    ignore = true;
		}
		return ignore;
	}

	// ignores initial spaces in lines and final comments
	void cleanLine()
	{
		while (line[0] == ' ')
			line.erase(line.begin());
		size_t found = line.find("//");
        if (found != string::npos) //string::npos means no '//' was found
			line.erase(found,line.length());
	}

	// composes inputfile name and opens input file
	bool reader(char* argv[])
	{
		strcpy(inputname,argv[1]);
		strcat(inputname,".asm");
		// opens input .asm file
		inputfile.open(inputname);
		if (inputfile.is_open(), fstream::in)
		{
		// reads first line
		//inputfile.getline(linechar,256);
		    getline(inputfile,line);
			cleanLine();
		//line = string() + linechar;
		//cout << line << endl;
		}
		else cout << inputfile << " not open" << endl;
		ignoreline();
		//inputfile.sync();
		return ignore, endfile;
	}
	// checks if at end file
	bool hasMoreCommands()
	{
		a_command = false;
		l_command = false;
		c_command = false;
		endfile = false;
		if (inputfile.eof())
		{
			endfile = true;
			//inputfile.close();
		}			
		return endfile;
	}
	// advances read of inputfile
	bool advance()
	{
		//if (!endfile)
		//{
		line.clear();
		strcpy(linechar, "");
		//cout << linechar << endl;
		if (inputfile.good())
		{			
		//inputfile.getline(linechar,256);
			getline(inputfile,line);
			cleanLine();
		}
		//line = string() + linechar;
		//cout << line << endl;
		ignoreline();
		//inputfile.sync();
		return ignore;
		//}
	}
	// function for labelling the type of command (address,computation,label)
	bool commandType()
	{
		if (line[0] == '@')
			a_command = true;
		else if (line[0] == '(')
			l_command = true;
		else
			c_command = true;
		return a_command, l_command, c_command;
	}
	// function to select parsing function
	string selector()
	{
		if (a_command || l_command)
			symbol();
		else if (c_command)
		{
			dest();
			comp();
			jump();
			if (!destParsedLine.empty())
			    parsedLine = destParsedLine + " " + compParsedLine + " " + jumpParsedLine;
			else
                parsedLine = compParsedLine + " " + jumpParsedLine;
		}
		return parsedLine;
	}
	// function returning address or label symbol
	string symbol()
	{
		if (a_command)
			parsedLine = line.substr(1,line.length());
		else if (l_command)
			parsedLine = line.substr(1,line.length()-2);
		return parsedLine;
	}
	// functions returning computation destination
	string dest()
	{
		size_t equal = line.find('='); //no '=' found = returns 'npos'
		if (equal != string::npos)
		    destParsedLine = line.substr(0,equal);
		else
			destParsedLine = string();
		return destParsedLine;
	}
	string comp()
	{
		size_t equal = line.find('=');
		size_t semicolon = line.find(';');
		if (equal < 10)
			compParsedLine = line.substr(equal+1,semicolon);
		else if (semicolon < 10)
			compParsedLine = line.substr(0,semicolon);
		else
			compParsedLine = string();
		return compParsedLine;
	}
	string jump()
	{
		size_t semicolon = line.find(';');
		if (semicolon < 10)
		    jumpParsedLine = line.substr(semicolon+1, line.length());
		else
			jumpParsedLine = string();
		return jumpParsedLine;
	}

	int increaseCount()
	{
		count++;
		return count;
	}
};

class code
{
	/* translates c-commands and a-commands from assembly to binary */
private:
	char outputname[30];
public:
	string destBinary, compBinary, jumpBinary;
	string a_binary;
	ofstream outputfile;

	// default code constructor
	code()
	{
	}

	// binary code for dest part of dest=comp;jump command
	string dest(string destParsedLine)
	{
		if (destParsedLine == string())
			destBinary = "000";
		else if (destParsedLine == "M")
			destBinary = "001";
        else if (destParsedLine == "D")
			destBinary = "010";
		else if (destParsedLine == "MD")
			destBinary = "011";
		else if (destParsedLine == "A")
			destBinary = "100";
		else if (destParsedLine == "AM")
			destBinary = "101";
		else if (destParsedLine == "AD")
			destBinary = "110";
		else if (destParsedLine == "AMD")
			destBinary = "111";
		else
			destBinary = "XXX";
		return destBinary;
	}

	// binary code for comp part of c-command
	string comp(string compParsedLine)
	{
		if (compParsedLine == "0")
			compBinary = "0101010";
		else if (compParsedLine == "1")
			compBinary = "0111111";
        else if (compParsedLine == "-1")
			compBinary = "0111010";
		else if (compParsedLine == "D")
			compBinary = "0001100";
		else if (compParsedLine == "A")
			compBinary = "0110000";
		else if (compParsedLine == "!D")
			compBinary = "0001101";
		else if (compParsedLine == "!A")
			compBinary = "0110001";
		else if (compParsedLine == "-D")
			compBinary = "0001111";
		else if (compParsedLine == "-A")
			compBinary = "0110011";
		else if (compParsedLine == "D+1")
			compBinary = "0011111";
		else if (compParsedLine == "A+1")
			compBinary = "0110111";
		else if (compParsedLine == "D-1")
			compBinary = "0001110";
		else if (compParsedLine == "A-1")
			compBinary = "0110010";
		else if (compParsedLine == "D+A")
			compBinary = "0000010";
		else if (compParsedLine == "D-A")
			compBinary = "0010011";
		else if (compParsedLine == "A-D")
			compBinary = "0000111";
		else if (compParsedLine == "D&A")
			compBinary = "0000000";
		else if (compParsedLine == "D|A")
			compBinary = "0010101";
		else if (compParsedLine == "M")
			compBinary = "1110000";
		else if (compParsedLine == "!M")
			compBinary = "1110001";
		else if (compParsedLine == "-M")
			compBinary = "1110011";
		else if (compParsedLine == "M+1")
			compBinary = "1110111";
		else if (compParsedLine == "M-1")
			compBinary = "1110010";
		else if (compParsedLine == "D+M")
			compBinary = "1000010";
		else if (compParsedLine == "D-M")
			compBinary = "1010011";
		else if (compParsedLine == "M-D")
			compBinary = "1000111";
		else if (compParsedLine == "D&M")
			compBinary = "1000000";
		else if (compParsedLine == "D|M")
			compBinary = "1010101";
		else
			compBinary = "YYYYYYY";
		return compBinary;
	}

    // binary code for the jump part of the c-command
	string jump(string jumpParsedLine)
	{
		if (jumpParsedLine == string())
			jumpBinary = "000";
		else if (jumpParsedLine == "JGT")
			jumpBinary = "001";
        else if (jumpParsedLine == "JEQ")
			jumpBinary = "010";
		else if (jumpParsedLine == "JGE")
			jumpBinary = "011";
		else if (jumpParsedLine == "JLT")
			jumpBinary = "100";
		else if (jumpParsedLine == "JNE")
			jumpBinary = "101";
		else if (jumpParsedLine == "JLE")
			jumpBinary = "110";
		else if (jumpParsedLine == "JMP")
			jumpBinary = "111";
		else
			jumpBinary = "ZZZ";
		return jumpBinary;
	}

	// binary code for address-command
	string address(string parsedLine)
	{
		int a;
		//no stoi for this visual studio, convert string to char to integer
		//string to char
		char *y = new char[parsedLine.length() + 1];
        strcpy(y, parsedLine.c_str());
		//char to integer
		a = atoi(y); //convert string parsedLine to integer a
		a_binary = "0000000000000000";
		// most significant binary cipher has to stay "0" (a-command identifier)
		if (a & 16384)
			a_binary[1] = '1';
		if (a & 8192)
			a_binary[2] = '1';
		if (a & 4096)
			a_binary[3] = '1';
		if (a & 2048)
			a_binary[4] = '1';
		if (a & 1024)
			a_binary[5] = '1';
		if (a & 512)
			a_binary[6] = '1';
		if (a & 256)
			a_binary[7] = '1';
		if (a & 128)
			a_binary[8] = '1';
		if (a & 64)
			a_binary[9] = '1';
		if (a & 32)
			a_binary[10] = '1';
		if (a & 16)
			a_binary[11] = '1';
		if (a & 8)
			a_binary[12] = '1';
		if (a & 4)
			a_binary[13] = '1';
		if (a & 2)
			a_binary[14] = '1';
		if (a & 1)
			a_binary[15] = '1';
		return a_binary;
	}

	// creates and opens output file .hack
	void output(char* argv[])
	{
		strcpy(outputname,argv[1]);
		strcat(outputname,".hack");
		// opens output .hack file
		outputfile.open(outputname);
	}
};

int main (int argc, char *argv[])
{
	bool endfile;
	bool ignore;
	//bool a_command, l_command, c_command; 
	//string a_binary;
	//string parsedLine;
	//string destParsedLine, compParsedLine, jumpParsedLine;
	string destBinary, compBinary, jumpBinary;
	string lineBinary;
	
	int count = 0;

	if ((argc != 2) || (strchr(argv[1],'.') != NULL))
	{
		cout << argv[0] << ": assembly .asm file argument should be supplied, without .asm extension\n";
		return 1;
	}
	
	endfile = false;
	ignore = false;

	//parser initialiser = parser();
	parser attempt1 = parser();
	//code initialiser = code();
	code attempt1translator = code();
	//create outputfile
	attempt1translator.output(argv);
	//start parsing and binary translation
	attempt1.reader(argv);
	while (!attempt1.endfile)
	{
		attempt1.hasMoreCommands();
		if (attempt1.endfile)
			return 0;
		if (attempt1.count > 0)
			attempt1.advance();
		if (!attempt1.ignore)
		{
			attempt1.commandType();
			attempt1.selector();
			//cout << attempt1.parsedLine << endl;
			if (attempt1.a_command)
			{
			    attempt1translator.address(attempt1.parsedLine);
				lineBinary = attempt1translator.a_binary;
			}
			else if (attempt1.c_command)
			{
                attempt1translator.dest(attempt1.destParsedLine);
			    attempt1translator.comp(attempt1.compParsedLine);
				attempt1translator.jump(attempt1.jumpParsedLine);
				lineBinary = "111" + attempt1translator.compBinary + attempt1translator.destBinary + attempt1translator.jumpBinary;
			}
			if (attempt1translator.outputfile.is_open(), fstream::out)
			{
				attempt1translator.outputfile << lineBinary << endl;
			}
		}
		attempt1.increaseCount();
		attempt1.ignore = false;
	}
	attempt1translator.outputfile.close();
	return 0;
}


//	char auxiliary[20];
//    strcpy (auxiliary, argv[1]);
//	ifstream inputfile (strcat(argv[1],".asm"));
//	ofstream outputfile (strcat(auxiliary,".hack"));
//	if (inputfile.is_open() && outputfile.is_open())
//	{
//		while (getline(inputfile,line))
//		{
//			if (line[0] != '/' && line.length() != 0)
//			{
//				outputfile << line << '\n';
//				cout << line[0] << '\n';
//			}
//		}
//		inputfile.close();
//		outputfile.close();
//	}
//    else cout << "Unable to open file";
//    return 0;
//}

//int main (int argc, char *argv[])
//{
//	if ((argc != 2) || (strchr(argv[1],'.') != NULL))
//	{
//		cout << argv[0] << ": assembly .asm file argument should be supplied, without .asm extension\n";
//		return 1;
//	}
//
//	string line;
//	char auxiliary[20];
//    strcpy (auxiliary, argv[1]);
//	ifstream inputfile (strcat(argv[1],".asm"));
//	ofstream outputfile (strcat(auxiliary,".hack"));
//	if (inputfile.is_open() && outputfile.is_open())
//	{
//		while (getline(inputfile,line))
//		{
//			if (line[0] != '/' && line.length() != 0)
//			{
//				outputfile << line << '\n';
//				cout << line[0] << '\n';
//			}
//		}
//		inputfile.close();
//		outputfile.close();
//	}
//    else cout << "Unable to open file";
//    return 0;
//}