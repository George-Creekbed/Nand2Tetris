// Assembler for Nand 2 tetris chapter 6, transforms .asm assembly files into .hack machine language rom files
#include <iostream>
#include <fstream>
#include <varargs.h>
#include <cstring>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <map>
using namespace std;

class parser
{
	/* encapsulates (breaks into basic units of information, identifies role of such information)
	original assembly code in file .asm */
private:
	char inputname[30];
public:
	bool endfile, ignore;
	int count, generalCount;
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
		generalCount = 0;
	}

	// ignores line in case it is comment, or a empty line
	bool ignoreline()
	{
		ignore = false;
		if (line[0] == '/' || line.empty())
		{
		    ignore = true;
		}
		return ignore;
	}

	// ignores initial spaces in lines, and comments at the end of the line
	void cleanLine()
	{
		while (line[0] == ' ')
			line.erase(line.begin());    // eliminate leading line spaces
		size_t found = line.find("//");    // eliminate comments at the end of the line
        if (found != string::npos)    //(string::npos means no '//' was found)
			line.erase(found,line.length());
		size_t findTrailingSpaces = line.find(' ');    // eliminate trailing line spaces
		if (findTrailingSpaces != string::npos)    //(string::npos means no ' ' was found)
			line.erase(findTrailingSpaces,line.length());
	}

	// composes inputfile name, and opens input file
	bool reader(char* argv[])
	{
		strcpy(inputname,argv[1]);
		strcat(inputname,".asm");
		inputfile.open(inputname);    // opens input .asm file
		if (inputfile.is_open(), fstream::in)
		{
		    getline(inputfile,line);    // reads first line
			cleanLine();
		}
		else cout << inputfile << " not open" << endl;
		ignoreline();
		return ignore, endfile;
	}

	// checks if @endfile
	bool hasMoreCommands()
	{
		a_command = false;
		l_command = false;
		c_command = false;
		endfile = false;
		if (inputfile.eof())
		{
			endfile = true;
		}
		return endfile;
	}

	// advances reading another line out of inputfile
	bool advance()
	{
		line.clear();
		strcpy(linechar, "");
		if (inputfile.good())
		{
			getline(inputfile,line);
			cleanLine();
		}
		else
			cout << "inputfile problem" << endl;
		ignoreline();
		return ignore;
	}

	// identifies the type of command (address,computation,label)
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

	// selects parsing function
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

	// returns address or label symbol
	string symbol()
	{
		if (a_command)
			parsedLine = line.substr(1,line.length());
		else if (l_command)
			parsedLine = line.substr(1,line.length()-2);
		return parsedLine;
	}

	// following three functions return c-command type line
	string dest()
	{
		size_t equal = line.find('=');    //(no '=' found = returns 'npos')
		if (equal != string::npos)
		    destParsedLine = line.substr(0,equal);
		else
			destParsedLine = string();
		return destParsedLine;
	}

	string comp()
	{
		size_t j;
		size_t equal = line.find('=');
		size_t semicolon = line.find(';');
		if (equal < 10)
			compParsedLine = line.substr(equal+1,semicolon);
		else if (semicolon < 10)
			compParsedLine = line.substr(0,semicolon);
		else
			compParsedLine = string();
		j = 0;
		while (j < compParsedLine.length())
			if (compParsedLine[j] == ' ')
			    compParsedLine.erase(j);
			else
				j++;
		return compParsedLine;
	}

	string jump()
	{
		size_t j;
		size_t semicolon = line.find(';');
		if (semicolon < 10)
		    jumpParsedLine = line.substr(semicolon+1, line.length());
		else
			jumpParsedLine = string();
		j = 0;
		while (j < jumpParsedLine.length())
			if (jumpParsedLine[j] == ' ')
			    jumpParsedLine.erase(j);
			else
				j++;
		return jumpParsedLine;
	}

	// following two functions increase RAM registries usage counter and ROM instruction line counter, respectively
	int increaseCount()
	{
		count++;
		return count;
	}

	int increaseGeneralCount()
	{
		generalCount++;
		return generalCount;
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
		/* no stoi function available for this version of visual studio (2008),
		convert string to char to integer */
		char *y = new char[parsedLine.length() + 1];    // string to char
        strcpy(y, parsedLine.c_str());
		a = atoi(y);    // char to integer
		a_binary = "0000000000000000";
		if (a & 16384)
			a_binary[1] = '1'; // most significant binary cipher (a_binary[0]) has to remain "0" (a-command identifier)
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
		outputfile.open(outputname);    // opens output .hack file
	}
};

class symbolTable
{
	/* creates symbols hashtable using C++ <map> */
private:

public:
	 map <string, int> hashTable;
	 bool symbolPresence;
	 int counterProgram;
	 int RAMaddress;
	 int wantedAddress;

	// default symbolTable constructor and initialiser
	symbolTable()
	{
		symbolPresence = false;
		RAMaddress = 16;
		wantedAddress = 0;
		hashTable.insert(make_pair("SP", 0));
		hashTable.insert(make_pair("LCL", 1));
		hashTable.insert(make_pair("ARG", 2));
		hashTable.insert(make_pair("THIS", 3));
		hashTable.insert(make_pair("THAT", 4));
		hashTable.insert(make_pair("R0", 0));
		hashTable.insert(make_pair("R1", 1));
		hashTable.insert(make_pair("R2", 2));
		hashTable.insert(make_pair("R3", 3));
		hashTable.insert(make_pair("R4", 4));
		hashTable.insert(make_pair("R5", 5));
		hashTable.insert(make_pair("R6", 6));
		hashTable.insert(make_pair("R7", 7));
		hashTable.insert(make_pair("R8", 8));
		hashTable.insert(make_pair("R9", 9));
		hashTable.insert(make_pair("R10", 10));
		hashTable.insert(make_pair("R11", 11));
		hashTable.insert(make_pair("R12", 12));
		hashTable.insert(make_pair("R13", 13));
		hashTable.insert(make_pair("R14", 14));
		hashTable.insert(make_pair("R15", 15));
		hashTable.insert(make_pair("SCREEN", 16384));
		hashTable.insert(make_pair("KBD", 24576));
	}

	// determines if a pair <symbol,address> is already in the symbols table
	bool contains(string symbol)
	{
		symbolPresence = false;
		if (hashTable.find(symbol) != hashTable.end())
			symbolPresence = true;
		return symbolPresence;
	}

    // adds a pair <symbol(key),address(value)> to the symbols table
	void addEntryRAM(string symbol)
	{
		if (!symbolPresence)
		{
			hashTable.insert(make_pair(symbol, RAMaddress));
			RAMaddress++;
		}
	}

	// adds a symbols table entry with a (label) command and its next line number
	void addEntryROM(string symbol, int address)
	{
		if (!symbolPresence)
		{
			hashTable.insert(make_pair(symbol, address));
		}
	}

	// returns address associated with symbol
	int getAddress(string symbol)
	{
		wantedAddress = hashTable[symbol];
		return wantedAddress;
	}
};

int main (int argc, char *argv[])
{
	bool endfile;
	bool ignore;
	string destBinary, compBinary, jumpBinary;
	string lineBinary;

	int count = 0;
	size_t index;
	bool a_commandIsSymbol;
	char intToChar[33];

	if ((argc != 2) || (strchr(argv[1],'.') != NULL))
	{
		cout << argv[0] << ": assembly .asm file argument should be supplied, without .asm extension\n";
		return 1;
	}

	endfile = false;
	ignore = false;

	parser attempt1 = parser();    //parser initialiser = parser();
	code attempt1translator = code();    //code initialiser = code();
	symbolTable symbolTableInstance = symbolTable();   //symbolTable initialiser = symbolTable()

	// first pass: fill symbolTable with label symbols
	attempt1.reader(argv);
	while (!attempt1.endfile)
	{
		attempt1.hasMoreCommands();
		if (attempt1.endfile) break;    // get out of the while-loop after last line in inputfile
		if (attempt1.generalCount > 0)
			attempt1.advance();
		if (!attempt1.ignore)
		{
			attempt1.commandType();
			attempt1.selector();
			if (attempt1.a_command || attempt1.c_command)
				attempt1.increaseCount();
			else if (attempt1.l_command)
			{
				symbolTableInstance.contains(attempt1.parsedLine);
				symbolTableInstance.addEntryROM(attempt1.parsedLine,attempt1.count);
			}
		}
		attempt1.increaseGeneralCount();
		attempt1.ignore = false;
	}

	// second pass: converts RAM @address symbols and writes down machine code .hack file in one go
	a_commandIsSymbol = false;
	index = 0;
	parser attempt2 = parser(); 	// parser initialiser = parser(); a second instance of this object was needed
	attempt2.count = 0;
	attempt2.generalCount = 0;
	attempt1translator.output(argv);    // creates outputfile
	// start parsing and binary translation
	attempt2.reader(argv);
	while (!attempt2.endfile)
	{
		attempt2.hasMoreCommands();
		if (attempt2.endfile)
			return 0;
		if (attempt2.generalCount > 0)
			attempt2.advance();
		if (!attempt2.ignore)
		{
			attempt2.commandType();
			attempt2.selector();
			if (attempt2.a_command)
			{
				for (index = 0; index < attempt2.parsedLine.length(); index++)    // parses @address line, and determines if it is numeric or a symbol
				{
					if (attempt2.parsedLine[index] < '0' || attempt2.parsedLine[index] > '9')
						a_commandIsSymbol = true;
				}
				if (a_commandIsSymbol)    // if @address is not numeric
				{
					symbolTableInstance.contains(attempt2.parsedLine);
					symbolTableInstance.addEntryRAM(attempt2.parsedLine);    // adds @address symbol to hash table
					symbolTableInstance.getAddress(attempt2.parsedLine);    // gets @address symbol coupled numeric address
					itoa(symbolTableInstance.wantedAddress,intToChar,10);    // transforms above hashtable address from int to char
					string wantedAddressString = string(intToChar);    // transforms above char to string
					attempt1translator.address(wantedAddressString);    // translates above sring to machine code
					lineBinary = attempt1translator.a_binary;    // saves above machine code line in lineBinary string variable
					a_commandIsSymbol = false;     // resets @address line flag
				}
				else    // if @address is not a symbol then don't bother to update the hash table
				{
					attempt1translator.address(attempt2.parsedLine);
					lineBinary = attempt1translator.a_binary;
				}
				attempt2.increaseCount();    // increases RAM counter by one
			}
			else if (attempt2.c_command)   // set up c-command machine code line
			{
                attempt1translator.dest(attempt2.destParsedLine);
			    attempt1translator.comp(attempt2.compParsedLine);
				attempt1translator.jump(attempt2.jumpParsedLine);
				lineBinary = "111" + attempt1translator.compBinary + attempt1translator.destBinary + attempt1translator.jumpBinary;
				attempt2.increaseCount();
			}
			if (!attempt2.l_command)    // write @address or c-command machine line on output .hack file
			{
				if (attempt1translator.outputfile.is_open(), fstream::out)
				{
					attempt1translator.outputfile << lineBinary << endl;
				}
			}
		}
		attempt2.increaseGeneralCount();    // increase ROM counter and proceed to work on next inputfile line
		attempt2.ignore = false;
	}
	attempt1translator.outputfile.close();
	return 0;
}
