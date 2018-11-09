

#include <iostream>
#include <fstream>
#include <String>
#include <vector>
#include <stdio.h>

using namespace std;



//This is the function to call if you want to read a file
char* readWord(int address)
{
	//creates the object using the fstream function
	ifstream file;
	//Using the object to open (or create if its not found) a file name examples.bin
	// binary opens the file in binary mode, the out means you are outputting the contents of the file

	file.open("example.bin", ios::binary | ios::out);

	//The seek function moves the starting position of where you read in the address ex: if the value is 4 the starting 
	//position where you read is shifted over by 4 bits
	file.seekg(address);
	//This is an array. To output the contents we must put it in an array to display each portion
	//Each portion of the array displays one word (4 bytes)
	char* buffer = new char[2];
	//This function takes the opened file and reads the contents into array (buffer) and has 2 word places to read (2)
	file.read(buffer, 2);
	return buffer;
}
//driver to write values to file system
void writeWord(int nAddress, char test[2])
//This function is similar to read but we need to write values to the file
{
	//Checks whether the file is actually there
	if ((nAddress % 2) == 0)
	{
		//we must AND the values in the file to change them
		char* oldword = readWord(nAddress);
		char* newword = new char[2];
		//we AND the word at the address given from user input with the user input array value
		newword[0] = (oldword[0] & test[0]);
		newword[1] = (oldword[1] & test[1]);
		//cout << newword;
		//system("pause");
		//Same as read, makes an object and uses that object to open the file
		fstream myfile;
		//We need both out and in so we can ready the value there and overwrite only that portion

		myfile.open("example.bin", ios::binary | ios::out | ios::in);

		//The write version of seek
		myfile.seekp(nAddress);
		//Similar to the read function except it writes the array to the selected amount of words
		myfile.write(newword, 2);
		//We must close the file after we are done with each function
		myfile.close();
	}
	else
	{
		cout << "Writing failed not in word position";
		system("pause");
	}

}
//The next two functions just reuse the write word function to erase the sectors
void deleteallsectors()
{
	ofstream myfile;
	myfile.open("example.bin", ios::binary | ios::out);
	char buffer[] = { 0xFF, 0xFF };
	for (int i = 0; i < 640000; i++)
	{
		myfile.write(buffer, 2);
	}

	myfile.close();
}
//driver for deleting specific sectors
void deletesector(int nSector) {
	if ((nSector > 20) || (nSector < 0)) {
		cout << "Invalid Sector" << "/n";
	}
	else {
		char erase[63999];
		for (int i = 0; i < 63999; i++) {
			erase[i] = 0xff;
		}
		fstream myfile;

		myfile.open("example.bin", ios::binary | ios::out | ios::in);

		nSector = 32000 * (nSector);
		myfile.seekp(nSector);//
		myfile.write(erase, 63999);
		myfile.close();
	}
}

//These functions are meant to use the drivers from part 1
//Function is used to open the file system and point to it in RAM

CSC322FILE* CSC322_fopen(const char *filename, const char *mode) {
	
	//This is an example of the actual stdio.h fopen. Using as reference
	/*
	FILE *stream;

  stream = malloc(sizeof(FILE));
  if (!stream) {
	errno = ENFILE;
	return NULL;
  }

  if (open_file(stream, filename, mode) < 0) {
	free(stream);
	return NULL;
  }

  return stream;
}
	*/

	CSC322FILE *stream;
	stream = malloc(sizeof(CSC322FILE));
	if (!stream) {
		errno = ENFILE;
		return NULL;
	}
	if (open_file(stream, filename, mode) < 0) {
		free(stream);
		return NULL;
	}
}

//User input and selection on what happens
bool select(bool exit) {


	int choice;
	cout << "==Please select what number==\n\n"
		<< "1-Open File" << endl
		<< "2-Read File" << endl
		<< "3-Write File " << endl
		<< "4-write to file" << endl
		<< "5-seek where a file is" << endl
		<< "6-Display all files" << endl
		<< "7-Remove file" << endl
		<< "8-Exit program" << endl;
	cin >> choice;
	cout << "Choice Selected: " << choice << endl;

	//This case statement should be recursive except for case 8 where it leaves the select function and goes to main
	switch (choice) {
		//This should deal with users inputing an interger that does not fit any case statement available

	case 1:
		const char *filename;
		cout << "Choose what file you would like to open" << endl;
		cout >> filename;
		CSC322_fopen(filename,"r");
		break;
	case 2:

		
		break;
	case 3:

		
		break;
	case 4:

		
		break;
	case 5:

		
		break;
	case 6:

		
		break;
	case 7:

		
		break;

	case 8:
		exit = false;
		return exit;
		break;

	default:
		cout << "Invalid selection, please make sure you are inputting a value between 1 and 8" << "/n";
		break;
	}
	return true;
}

int main()
{
	
	bool exit = true;

	ofstream myfile;

	myfile.open("example.bin", ios::binary | ios::out);
	char buffer[] = { 0xFF, 0xFF };
	for (int i = 0; i < 640000; i++)
	{
		myfile.write(buffer, 2);
	}
	myfile.close();

	char word[2] = { 0x0F,0xF0 };
	char word2[2] = { 0x00,0x00 };
	for (int i = 0; i < 15; i = i + 2) {
		writeWord(i, word);
	}
	deletesector(0);

	while (exit)
	{
		exit = select(exit);
	}

	return 0;
}