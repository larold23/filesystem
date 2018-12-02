
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <String>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct openfile{

	bool newfile;
	bool edited;
	string filename;
	vector<char*> filedata;
	int returnaddress;
	string mode;
};

struct CSS322FILE {
	bool newfile;
	bool edited;
	string filename;
	vector<char*> filedata;
	int returnaddress;
	string mode;
};


//This is the function to call if you want to read a file
char* readWord(int address, fstream& myfile)
{
	//creates the object using the fstream function
	myfile.seekg(address);
	char* buffer = new char[3];
	myfile.read(buffer, 2);
	return buffer;
	delete buffer;
}

//driver to write values to file system
void writeWord(int nAddress, char test[2], fstream& myfile)
//This function is similar to read but we need to write values to the file
{
	//Checks whether the file is actually there
	if ((nAddress % 2) == 0)
	{
		char* oldword = readWord(nAddress, myfile);
		char* newword = new char[3];
		newword[0] = (oldword[0] & test[0]);
		newword[1] = (oldword[1] & test[1]);
		myfile.seekp(nAddress);
		myfile.write(newword, 2);
	}
	else
	{
		cout << "Writing failed not in word position";
		system("pause");
	}

}


void CSC322_fopen(const char *filename, const char *mode, fstream& myfile, vector<openfile>& ramStorage)
{
	int filefound = 0;

	//checking for if file is already in ram
	for (int i = 0; i < ramStorage.size(); i++) {
		if (ramStorage[i].filename == filename) {
			cout << "file already found!";
			filefound = 1;
			break;
		}
	}

	int addresstemp = 0;
	if (filefound == 0) {

		//temp values to access the headers for information
		int address, length, ramSize, index;
		char* next = new char[3];
		unsigned char* math = new unsigned char[3];
		char* buffer = new char[3];
		char* header = new char[3];
		char used[3] = { 0xFF,0x00 };
		char* used2 = used;
		char empty[3] = { 0xFF,0xFF };
		char deleted[3] = { 0x00,0x00 };
		char* empty2 = empty;
		char* deleted2 = deleted;
		char* name = new char[sizeof(filename)];

		//accesses the headers for both file names and next address 
		while (filefound == 0) {

			//locates the header value
			buffer = readWord(addresstemp, myfile);
			header = buffer;
			cout << endl << header[0] << header[1];
			string teste(empty2), testh(header), testd(deleted2), testu(used2), testm(mode);
			delete buffer;
			//Checks the flag to see if the file is used or not
			if (testh == testd || testh == teste) {
				cout << "header displayed no file";
				addresstemp = addresstemp + 66;
				next = readWord(addresstemp, myfile);
				stringstream ss;
				math = (unsigned char*)next;
				for (int i = 0; i < 2; ++i) {
					ss << hex << (int)math[i];
				}
				string mystr = ss.str();
				addresstemp = unsigned int(stoul(mystr, 0, 16));
				delete(next);
				delete(math);
				if (next == empty) {
					int choice;
					cout << "File does not exist, would you like to create it? \n" << "1.yes  2.no";
					cin >> choice;
					if (header == empty) {
						if (choice = 1) {
							ramSize = ramStorage.size();
							ramStorage.resize(ramSize + 1);
							index = ramStorage.size() - 1;
							ramStorage[index].newfile = true;
							ramStorage[index].edited = false;
							ramStorage[index].filename = filename;
							ramStorage[index].mode = mode;
						}
					}
				}
			}
			//flag shows file exists
			else if ((testh == testu)) {
				address = addresstemp;
				//increments by 1 word to start reading at filename
				addresstemp = addresstemp + 4;

				//fills out the buffer with filename
				for (int j = 0; j < sizeof(*filename); j = j + 2) {
					//acceses that filename for that location
					buffer = readWord(addresstemp, myfile);
					name[j] = buffer[0];
					name[j + 1] = buffer[1];
					delete(buffer);
				}
				//Found location, importing into main memmory
				if (name == filename) {
					filefound = 1;
					//declaring a new file imorted to ramStorage
					ramSize = ramStorage.size();
					ramStorage.resize(ramSize + 1);
					index = ramStorage.size() - 1;
					//importing new file
					ramStorage[index].newfile = false;
					//importing edited
					if (testm == "rb") {
						ramStorage[index].edited = false;
					}
					else {
						ramStorage[index].edited = true;
					}
					//importing filename
					ramStorage[index].filename = string(name);
					//importing filedata
					addresstemp = addresstemp + 58;
					buffer = readWord(addresstemp, myfile);
					stringstream ss;
					math = (unsigned char*)buffer;
					for (int i = 0; i < 2; ++i) {
						ss << hex << (int)math[i];
					}
					string mystr = ss.str();
					length = unsigned int(stoul(mystr, 0, 16));
					addresstemp = addresstemp + 8;

					for (int k = 0; k < length; k = k + 2) {
						buffer = readWord(addresstemp, myfile);
						ramStorage[index].filedata.push_back(buffer);
					}
					delete(buffer);
					delete(math);
					//import return address
					ramStorage[index].returnaddress = address;
					//importing mode
					ramStorage[index].mode = testm;
					//determing based on mode what to do with the recently opened file
					delete(name);
				}
				//moves to the next file location
				else {
					delete(name);
					addresstemp = addresstemp + 33;
					next = readWord(addresstemp, myfile);
					stringstream ss;
					math = (unsigned char*)next;
					for (int i = 0; i < 2; ++i) {
						ss << hex << (int)math[i];
					}
					string mystr = ss.str();
					addresstemp = unsigned int(stoul(mystr, 0, 16));
					delete(next);
					delete(math);
					if (addresstemp == 65535) {
						int choice;
						cout << "File does not exist, would you like to create it? \n" << "1.yes  2.no" << endl;
						cin >> choice;
						if (choice == 1) {

							//For this portions we need to make sure we make the address after this header file location because 
							//a file exists at this header, but a file doesnt exist afterwards.
							ramSize = ramStorage.size();
							ramStorage.resize(ramSize + 1);
							index = ramStorage.size() - 1;
							ramStorage[index].newfile = true;
							ramStorage[index].edited = false;
							ramStorage[index].filename = filename;
							ramStorage[index].mode = mode;

						}
					}
					delete(header);
				}
			}
		}
	}
}

char* data_for_file;//added for data in tempfile
//Storage location for data/Item size in bytes/ items to be read/Pointer to FILE structure.

void fread(FILE* tempF) {
	char* buffer = new char[2];

	while (!feof(tempF)) {
		fread(buffer, sizeof(buffer), 2, tempF);
		cout << buffer;
	}

}

void fwrite(FILE* tempF, int offset, vector<char*> tempdata) {

	bool answer;//added for input
	bool answer1;//added for input


	//char* data_for_file;//added for data in tempfile
	cout << "Do you want to append" << "Enter 1 for Yes or 2 for No" << endl;
	cin >> answer;
	if (answer == 1) {
		cout << "Do you wish to append to end of the file?" << endl
			<< "Enter 1 for Yes or 2 for No" << endl;
		cin >> answer1;

		if (answer1 == 1) {
			//makes the end of the file/ 2 byte offset from end
			// To set file point to 2 byte / character in the file
			fseek(tempF, 2, SEEK_END);

			cout << "Input data to append to the end of the file:" << endl
				<< " If you wish to exit Enter Exit" << endl;
			string exit;
			//char* data_for_file;//added for data in tempfile
			while (data_for_file != exit) {
				cin >> data_for_file;

				tempdata.push_back(data_for_file);

				if (data_for_file == exit) {
					break;
				}
			}
		}
		else {
			//Seek beginning position then set offset 
			cout << "Enter offset position you wish to insert data from (0-FF): " << endl;
			cin >> offset;
			fseek(tempF, offset, SEEK_SET);//begining of file
			cout << "Input data to file:" << endl
				<< " If you wish to exit Enter Exit" << endl;
			string exit;
			while (data_for_file != exit) {
				cin >> data_for_file;
				tempdata.push_back(data_for_file);
				if (data_for_file == exit) {
					break;
				}
			}

		}
	}

}

void fseek(int offset, FILE* tempF) {

	fseek(tempF, offset, SEEK_SET);//begining of file

}

long ftell(FILE* tempF) {
	//Returns the current value of the position of the stream
	return ftell(tempF);

}




void edit(string check_fileN, vector<openfile>& ramStorage) {
	//string mode;

	int offset;//offset for setting position 
	vector<char*> tempdata;
	for (int i = 0; i < ramStorage.size(); i++) {
		//check to see if name is in RAM
		if (ramStorage[i].filename == check_fileN) {
			cout << "File Name found" << endl;
			bool edit = ramStorage[i].edited;//set to true in struct 
			edit = true;
			//Create Temp File to store data
			FILE* tempF;
			//Mode
			string mode = ramStorage[i].mode;//added

			//mode = ramStorage[i].mode;//
			vector<char*> tempdata = ramStorage[i].filedata;


			/////tempF.open("tempfile.bin");

			//Temp file opened up in write/read
			fopen_s(&tempF, "tempfile.bin", "w+b");//added

			char* buffer = new char[2];
			//loop  
			for (int j = 0; j < tempdata.size(); j++) {
				buffer = tempdata[j];

				fwrite(buffer, sizeof(char), sizeof(buffer), tempF);//added
				fseek(tempF, 2, ftell(tempF));//added

				//tempF.write(buffer, 2);//
				//tempF.seekg(2 + tempF.tellp());//
			}

			cout << "Input number of mode you wish to open file:"
				<< "0. Write ( w + b/ab/wb)"
				<< "1. Read (rb/w+b)" << endl;
			int Y;//Write/Read
			cin >> Y;
			switch (Y) {
			case 0: {
				fwrite(tempF, 2, tempdata);
				break; }
			case 1: {
				//Read 
				char* buffer = new char[2];

				while (!feof(tempF)) {
					fread(buffer, sizeof(buffer), 2, tempF);
					cout << buffer;
				}
				break;
			}
			default: cout << "Wrong Input";

			}
			fclose(tempF);
		}


	}

}

int CSC322fclose(string filename, fstream& myfile, CSS322FILE& file)
{
	int position;
	int headerpos = 0;
	char unusedbuffer[] = { 0xFF, 0xFF };
	char usedbuffer[] = { 0xFF, 0x00 };
	char* nextfilebuf[2];
	bool end = true;
	string result;
	

		if (file.newfile == true)
		{
			position = 0;
			while (end)
			{
				if (readWord(position, myfile) == unusedbuffer)
				{
					writeWord(position, usedbuffer, myfile);
					position = position + 4;
					char *cstr = new char[filename.length() + 1];
					strcpy(cstr, filename.c_str());
					writeWord(position, cstr, myfile);
					position = position + 32;
					string s = std::to_string(file.returnaddress);
					char const *pchar = s.c_str();
					writeWord(position, _strdup(pchar), myfile);
					int nextfile = position + 4 + file.filedata.size() + 1;
					string n = std::to_string(nextfile);
					char const *dchar = n.c_str();
					writeWord(position, _strdup(dchar), myfile);
					position = position + 4;
					for (int f = 0; f < file.filedata.size(); f++)
					{
						writeWord(position, file.filedata[f], myfile);
						position = position + 4;
					}
					
					return 1;
				}
				else
				{
					position = position + 65;
					char* buffer = new char[2];
					buffer = readWord(position, myfile);
					position = stoi(buffer, nullptr, 16);
				}
			}
		}
		else if (file.edited == true)
		{
			position = file.returnaddress;
			while (end)
			{
				if (readWord(position, myfile) == unusedbuffer)
				{
					writeWord(position, usedbuffer, myfile);
					position = position + 4;
					char *cstr = new char[filename.length() + 1];
					strcpy(cstr, filename.c_str());
					writeWord(position, cstr, myfile);
					position = position + 32;
					string s = std::to_string(file.returnaddress);
					char const *pchar = s.c_str();
					writeWord(position, _strdup(pchar), myfile);
					int nextfile = position + 4 + file.filedata.size() + 1;
					string n = std::to_string(nextfile);
					char const *dchar = n.c_str();
					writeWord(position, _strdup(dchar), myfile);
					position = position + 4;
					for (int f = 0; f < file.filedata.size(); f++)
					{
						writeWord(position, file.filedata[f], myfile);
						position = position + 4;
					}
					
					end = false;
					return 1;
				}
				else
				{
					position = position + 65;
					char* buffer = new char[2];
					buffer = readWord(position, myfile);
					position = stoi(buffer, nullptr, 16);
				}
			}
		}
		else
		{
			
			end = false;
			return 1;
		}
		return 0;
}


int CSC322remove(string filename, fstream& myfile)
{
	int position = 0;
	int headerpos = 0;
	char usedbuffer[] = { 0xFF, 0x00 };
	char deletebuffer[] = { 0x00, 0x00 };
	char* nextfilebuf[2];
	string result;
	bool end = true;
	while (end)
	{
		if (readWord(position, myfile) == usedbuffer)
		{
			position = position + 4;
			char* filenamebuf[16];

			for (int i = 0; i < 16; i++)
			{
				filenamebuf[i] = readWord(position, myfile);
				position = position + 2;
			}
			ostringstream buffer;
			for (int i = 0; i < 16; ++i)
			{
				buffer << filenamebuf[i] << " ";
				result = buffer.str();

			}
			if (result == filename)
			{
				writeWord(headerpos, deletebuffer, myfile);
				end == false;
			}
			else
			{
				position = position + 4;
				nextfilebuf[0] = readWord(position, myfile);
				ostringstream buffer2;
				for (int i = 0; i < 2; ++i)
				{
					buffer2 << nextfilebuf[i] << " ";
					result = buffer2.str();
				}
				position = stoi(result);
				headerpos = stoi(result);
			}

		}
		else if (readWord(position, myfile) == deletebuffer)
		{
			position = position + 63;
			nextfilebuf[0] = readWord(position, myfile);
			ostringstream buffer2;
			for (int i = 0; i < 2; ++i)
			{
				buffer2 << nextfilebuf[i] << " ";
				result = buffer2.str();
			}
			position = stoi(result);
			headerpos = stoi(result);
		}
		else
		{
			cout << "file not found";
			end = false;

		}
	}

	return 0;
}
//This is the function to call if you want to read a file

//The next two functions just reuse the write word function to erase the sectors
void deleteallsectors(fstream& myfile)
{
	
	char buffer[] = { 0xFF, 0xFF };
	for (int i = 0; i < 640000; i++)
	{
		myfile.write(buffer, 2);
	}

}
//driver for deleting specific sectors
void deletesector(int nSector, fstream& myfile) {
	if ((nSector > 20) || (nSector < 0)) {
		cout << "Invalid Sector" << "/n";
	}
	else {
		char erase[63999];
		for (int i = 0; i < 63999; i++) {
			erase[i] = 0xff;
		}
		nSector = 32000 * (nSector);
		myfile.seekp(nSector);//
		myfile.write(erase, 63999);
	}
}




//These functions are meant to use the drivers from part 1
//Function is used to open the file system and point to it in RAM

//User input and selection on what happens
bool select(bool exit, fstream& myfile, vector<openfile>& ramStorage, CSS322FILE& file) {


	int choice;
	string mode;
	string filename = "";
	bool filefound = false;
	char* name = new char[filename.size() + 1];
	char buffer[] = { 0xFF, 0xFF };
	cout << "==Please select what number==\n\n"
		<< "1-Open File" << endl
		<< "2-Read Open File" << endl
		<< "3-Display all Open Files" << endl
		<< "4-Edit File" << endl
		<< "5-Close Open File" << endl
		<< "6-Delete File" << endl
		<< "7-Exit program" << endl
		<< "8-newfilesystem-used for testing"<< endl;
	cin >> choice;
	cout << "Choice Selected: " << choice << endl;

	//This case statement should be recursive except for case 8 where it leaves the select function and goes to main
	switch (choice) {
		//This should deal with users inputing an interger that does not fit any case statement available

	case 1:
		
		// when user chooses to open a file
		cout << "Name of file to open?";
		cin >> filename;
		cout << "Open file as rb, wb, w+b, ab";
		cin >> mode;
		name[filename.size()] = 0;
		memcpy(name, filename.c_str(), filename.size());
		if ((mode == "rb") || (mode == "wb") || (mode == "w+b") || (mode == "ab"))
		{
			
			char *cstr = new char[mode.length() + 1];
			strcpy(cstr, mode.c_str());
			CSC322_fopen(name, cstr, myfile, ramStorage);
			delete[] cstr;
		}
		break;
	case 2:
		cout << "Input File Name: ";
		cin >> filename;
		int i;
		filefound = false;
		
			for (i = 0; i < ramStorage.size(); i++)
			{
				if (filename == ramStorage[i].filename)
				{
					filefound = true;
				}
			}
			if (ramStorage[i].mode != "wb" | ramStorage[i].mode != "ab")
			{
				if (filefound == true)
				{
					cout << "Data of File: " << filename << endl;
					for (int j = 0; j <= ramStorage[i].filedata.size(); j++)
					{
						cout << ramStorage[i].filedata[j];
					}

				}
			}
		
		
		break;
	case 3:
		cout << "OpenFiles:" << endl;
		for (int i = 0; i < ramStorage.size(); i++)
		{
			filename = ramStorage[i].filename;
			cout << filename << endl;
		}
		break;
	case 4:
		// when user chooses to edit a file
		cout << "Input File Name: ";
		cin >> filename;
		edit(filename, ramStorage);

		
		break;
	case 5:
		// when user chooses to find a file
		int j;
		cout << "Name of file to close?";
		cin >> filename;
		for (j = 0; j < ramStorage.size(); j++)
		{
			if (filename == ramStorage[j].filename)
			{
				filefound = true;
			}
		}
		if (filefound == false)
		{
			file.edited = ramStorage[j].edited;
			file.newfile = ramStorage[j].newfile;
			file.filedata = ramStorage[j].filedata;
			file.returnaddress = ramStorage[j].returnaddress;
			int clear = CSC322fclose(filename, myfile, file);
			if (clear == 1)
			{
				ramStorage.erase(ramStorage.begin() + j);
			}
		}
		else
		{
			cout << "File not closed";
		}
		filefound = false;
		break;

		
		break;
	case 6:
		// when user chooses to delete a file
		
		int f;
		cout << "Name of file to delete?";
		cin >> filename;
		for (f = 0; f < ramStorage.size(); f++)
		{
			if (filename == ramStorage[f].filename)
			{
				filefound = true;
			}
		}
		if (filefound == false)
		{
			CSC322remove(filename, myfile);
		}
		else
		{
			cout << "File not closed";
		}
		filefound = false;
		break;

	case 7:
		// when user chooses to exit
		exit = false;
		return exit;
		break;

	case 8:
		deleteallsectors(myfile);
	default:
		cout << "Invalid selection, please make sure you are inputting a value between 1 and 8" << "/n";
		break;
	}
	return true;
}




int main()
{

	vector<openfile> ramStorage;
	CSS322FILE file;
	bool exit = true;
	fstream myfile;
	myfile.open("filesystem.bin", ios::binary | ios::out | ios::in);

	while (exit)
	{
		exit = select(exit, myfile, ramStorage, file);
	}

	myfile.close();
	return 0;
}