

#include <iostream>
#include <fstream>
#include <String>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

//These variables are what gets imported 
//ramStorage is the vector we put the struct in
//ex: ramStorage[1]={newfile,edited,filename,filedata,returnaddress,mode}
struct openfile{

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
	char* buffer=new char[3];
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

//The next two functions just reuse the write word function to erase the sectors
void deleteallsectors(fstream& myfile){
	
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
	else if(nSector==20){
		deleteallsectors(myfile);
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
		int address,length,ramSize,index;
		char* next=new char[3];
		unsigned char* math = new unsigned char[3];
		char* buffer=new char[3];
		char* header=new char[3];
		char used[3] = {0xFF,0x00};
		char* used2 = used;
		char empty[3]={0xFF,0xFF};
		char deleted[3] = {0x00,0x00};
		char* empty2 = empty;
		char* deleted2 = deleted;
		char* name=new char[sizeof(filename)];

		//accesses the headers for both file names and next address 
		while (filefound == 0){

			//locates the header value
			buffer = readWord(addresstemp, myfile);
			header = buffer;
			cout <<endl<< header[0] << header[1];
			string teste(empty2),testh(header),testd(deleted2),testu(used2),testm(mode);
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
				for (int j = 0; j < sizeof(*filename); j = j + 2){
				//acceses that filename for that location
					buffer = readWord(addresstemp, myfile);
					name[j] = buffer[0];
					name[j + 1] = buffer[1];
					delete(buffer);
				}
				//Found location, importing into main memmory
				if (name == filename){
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
					else{
						ramStorage[index].edited = true;
					}
					//importing filename
					ramStorage[index].filename = string (name);
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
					addresstemp = unsigned int(stoul(mystr,0,16));
					delete(next);
					delete(math);
					if (addresstemp == 65535) {
						int choice;
						cout << "File does not exist, would you like to create it? \n" << "1.yes  2.no"<<endl;
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
//User input and selection on what happens
bool select(bool exit, fstream& myfile, vector<openfile>& ramStorage) {


	int choice;
	cout << "==Please select what number==\n\n"
		<< "1-Open File" << endl
		<< "2-Read File" << endl
		<< "3-Write File " << endl
		<< "4-write to file" << endl
		<< "5-seek where a file is" << endl
		<< "6-Display all files" << endl
		<< "7-Remove file" << endl
		<< "8-Exit program" << endl
		<< "9-newfilesystem-used for testing"<< endl;
	cin >> choice;
	cout << "Choice Selected: " << choice << endl;

	//This case statement should be recursive except for case 8 where it leaves the select function and goes to main
	switch (choice) {
		//This should deal with users inputing an interger that does not fit any case statement available

	case 1:
		deleteallsectors(myfile );
		char test2[2] = { 0xFF,0x00 };
		char* test=test2;
		writeWord(0, test, myfile);
		char* filename = new char[2];
		filename = readWord(0, myfile);
		cout << filename[0] << filename[1];
		//filename = readWord(1, myfile);
		char* mode = new char[1];
		mode;
		
		CSC322_fopen(filename, mode, myfile,ramStorage);
		break;
	//case 2:
		// when user chooses to read a file
	//	break;
	//case 3:
		// when user chooses to write a fil
		//break;
	//case 4:
		// when user chooses to edit a fill
	//	break;
	//case 5:
		// when user chooses to find a file
	//	break;
	//case 6:
		// when user chooses to display all files
	//	break;
	//case 7:
		// when user chooses to delete a file
	//	break;
	//case 8:
		// when user chooses to exit
	//	exit = false;
	//	return exit;
	//	break;
	//case 9:
		//char buffer[] = { 0xFF, 0xFF };
		//for (int i = 0; i < 640000; i++)
		//{
	//		myfile.write(buffer, 2);
	//	}
//	default:
	//	cout << "Invalid selection, please make sure you are inputting a value between 1 and 8" << "/n";
	//	break;
	}
	return true;
}


int main()
{
	//set up the vector as global for easier access 
	vector<openfile> ramStorage;
	bool exit = true;
	fstream myfile;
	myfile.open("filesystem.bin", ios::binary | ios::out | ios::in);

	while (exit)
	{
		exit = select(exit, myfile,ramStorage);
	}

	myfile.close();
	return 0;
}