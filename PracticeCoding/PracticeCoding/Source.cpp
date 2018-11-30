

#include <iostream>
#include <fstream>
#include <String>
#include <vector>
#include <stdio.h>

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
char readWord(int address, fstream& myfile)
{
	//creates the object using the fstream function
	myfile.seekg(address);
	char buffer[2];
	myfile.read(buffer, 2);
	return buffer[0];
	return buffer[1];
}
//driver to write values to file system
void writeWord(int nAddress, char test[2], fstream& myfile)
//This function is similar to read but we need to write values to the file
{
	char newword[2];
	//Checks whether the file is actually there
	if ((nAddress % 2) == 0){
		char* oldword = readWord(nAddress, myfile);
		newword[0] = (oldword[0] & test[0]);
		newword[1] = (oldword[1] & test[1]);
		myfile.seekp(nAddress);
		myfile.write(newword, 2);
	}
	else{
		cout << "Writing failed not in word position";
		system("pause");
	}

}
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

void CSC322_fopen(const char *filename, int mode, fstream& myfile, vector<openfile>& ramStorage) 
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
		int addresstemp = 0;
		char* next = new char[2];
		//temp char that i use for loops to transfer to other char
		char* buffer=new char[2];
		char* header=new char[2];

		char used[2] = {0xFF,0x00};
		char empty[2] = {0xFF,0xFF};
		char deleted[2] = {0x00,0x00};
		char* name = new char[sizeof(filename)];
		//accesses the headers for both file names and next address 
		while (filefound == 0){

			//locates the header value
			buffer = readWord(addresstemp, myfile);
			header = buffer;
			cout << header << endl;
			//Checks the flag to see if the file is used or not
			if (header == deleted | header== empty) {
				cout << "header displayed no file";
				addresstemp = addresstemp + 66;
				next = readWord(addresstemp, myfile);
				//atoi converts a string to an interger. Basically its taking the array of seperate numbers 
				//and setting it equal to address
				address = stoi(next, nullptr, 2);
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
			else if (header == used) {
				system("pause");
				address = addresstemp;
				//increments by 1 word to start reading at filename
				addresstemp = addresstemp + 4;

				//fills out the buffer with filename
				for (int j = 0; j < sizeof(filename); j = j + 1){
				//acceses that filename for that location
					buffer = readWord(addresstemp, myfile);
					name[j] = buffer[0];
					name[j + 1] = buffer[1];
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
					ramStorage[index].edited = false;
					//importing filename
					ramStorage[index].filename = string (name);

					//importing filedata
					addresstemp = addresstemp + 58;
					buffer = readWord(addresstemp, myfile);
					length = stoi(buffer, nullptr, 16);
					addresstemp = addresstemp + 8;

					for (int k = 0; k < length; k = k + 2) {
						buffer = readWord(addresstemp, myfile);
						ramStorage[index].filedata.push_back(buffer);
					}

					//import return address
					ramStorage[index].returnaddress = address;
					//importing mode
					ramStorage[index].mode = mode;
					//determing based on mode what to do with the recently opened file
				}
				//moves to the next file location
				else{
					addresstemp = addresstemp + 33;
					next = readWord(addresstemp, myfile);
					addresstemp = stoi(next, nullptr, 16);
					if (addresstemp == 65535) {
						int choice;
						cout << "File does not exist, would you like to create it? \n" << "1.yes  2.no";
						cin >> choice;
						if (choice = 1) {

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
		char* filename = readWord(0, myfile);
		writeWord(0, filename, myfile);
		CSC322_fopen(filename, 1, myfile,ramStorage);
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