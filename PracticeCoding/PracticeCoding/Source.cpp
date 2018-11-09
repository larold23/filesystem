

#include <iostream>
#include <fstream>
#include <String>
#include <vector>
#include <stdio.h>

using namespace std;

struct openfile{

	bool newfile;
	bool edited;
	string filename;
	vector<char> filedata;
	int returnaddress;
	string mode;
};


//This is the function to call if you want to read a file
char* readWord(int address, fstream& myfile)
{
	//creates the object using the fstream function
	myfile.seekg(address);
	char* buffer = new char[2];
	myfile.read(buffer, 2);
	return buffer;
}
//driver to write values to file system

void writeWord(int nAddress, char test[2], fstream& myfile)
//This function is similar to read but we need to write values to the file
{
	//Checks whether the file is actually there
	if ((nAddress % 2) == 0)
	{
		char* oldword = readWord(nAddress, myfile);
		char* newword = new char[2];
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
bool select(bool exit, fstream& myfile) {


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
		// when user chooses to open a file

		
		
		break;
	case 2:
		// when user chooses to read a file
		


		break;
	case 3:
		// when user chooses to write a file

		
		break;
	case 4:
		// when user chooses to edit a file


		
		break;
	case 5:
		// when user chooses to find a file


		
		break;
	case 6:
		// when user chooses to display all files


		
		break;
	case 7:
		// when user chooses to delete a file
		
		break;

	case 8:
		// when user chooses to exit
		exit = false;
		return exit;
		break;

	case 9:
		char buffer[] = { 0xFF, 0xFF };
		for (int i = 0; i < 640000; i++)
		{
			myfile.write(buffer, 2);
		}

	default:
		cout << "Invalid selection, please make sure you are inputting a value between 1 and 8" << "/n";
		break;
	}
	return true;
}




int main()
{

	vector<openfile> ramStorage;
	bool exit = true;
	fstream myfile;
	myfile.open("filesystem.bin", ios::binary | ios::out);

	while (exit)
	{
		exit = select(exit, myfile);
	}

	myfile.close();
	return 0;
}