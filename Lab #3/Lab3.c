/* CS315 Lab 3: C data types */

#include <stdio.h>
/*Included for use of exits.*/
#include <stdlib.h>

#define ONE_MEGABYTE	1048576.00
#define MAX_FILE_SIZE	10485760

int bitRate[14][2]= {
	{0x10, 32},
	{0x20, 40},
	{0x30, 48},
	{0x40, 56},
	{0x50, 64},
	{0x60, 80},
	{0x70, 96},
	{0x80, 112},
	{0x90, 128},
	{0xA0, 160},
	{0xB0, 192},
	{0xC0, 224},
	{0xD0, 256},
	{0xE0, 320}
};

int frequencyRate[3][2] = {
	{0x00, 44100}, // 0000 0000
	{0x04, 48000}, // 0000 0100
	{0x08, 32000}  // 0000 1000 
};
		

/*Structure for a file object. Will hold 
a pointer to the referenced file and 
the size of that file in bytes.*/
typedef struct{
	FILE * pointerToFile;
	float fileSize;
} file;

/*Find the size of the file in bytes.*/
int findFileSize(FILE * file_pointer)
{
	float size = 0;
	fseek(file_pointer, 0, SEEK_END);	//go from 0 bytes to the end of the file.
	size = ftell(file_pointer);			//return the amount of bytes traversed.
	rewind(file_pointer);				//return the pointer to the beginning.
	
	//Error checking for the size of the file.
	if( size < 1 || size > MAX_FILE_SIZE )
	{
		printf("File size is not within the allowed range\n"); 
		return(EXIT_FAILURE);
	}
	
	return size;
}

/*Method that takes in a pointer to a memory location for a file, the 
size of that file and what bytes it needs to find. This will return only
the index of the first occurance of this sequence.*/
int findHeader(unsigned char * data, size_t fileSize, unsigned char byteToFind, unsigned char secondNibb)
{
	int i;
	
	for(i = 0; i < fileSize; i++)
	{
		/*Consider two case. We find two byte patters, one patter is 0xFF 0xF* 
		OR the second patter is 0x*F 0xFF*/
	if((data[i] == 0xFF) && ((data[i+1] & 0xF0) == 0xF0))
		{
			printf("found: %02x ", data[i]);
			printf("%02x \n", data[i+1]);
			//Return index.
			return i;
		}
		
	}
	return 1; //The method did not find the header. 
}

int findVersion(unsigned char * data, int headerLocation)
{
	/*Storage for the next byte in the sequence.
	This byte will contain the information for the 
	Version, Layer and Error Protection bits.
	*/
	unsigned char nextByte = (data[headerLocation + 1] & 0x0F);
	
	if(nextByte == 0x0A || nextByte == 0x0B)
	{
		printf("This file is: MPEG Layer 3\n");
		return 0;
	}
	else{
		printf("This is not MPEG Layer 3\n");
	}
	return 1; //The file is not MPEG Layer 3.
}

/*
This will find the bitRate of a MPEG Layer 3 file by accessing the
2-d table at the top of the code. Returns the bit rate as an int.
*/
int findBitRate(unsigned char * data, int headerLocation)
{
	//Get just the upper nibble for the bitRate.
	unsigned char bitRateByte = (data[headerLocation + 2] & 0xF0);
	
	int row = sizeof(bitRate)/sizeof(bitRate[0]);
	int i;
	
	for(i = 0; i < row; i++)
	{

		if(bitRateByte == bitRate[i][0]){
			//printf("Bit Rate: %d \n", bitRate[i][1]);
			return bitRate[i][1];
		}
		
	}
	return 1; 
}
/*
Locates the frequency rate bits, compares those bits to the 
frequency array record and outputs the specified rate as an int.
*/
int findFrequency(unsigned char * data, int headerLocation)
{
	unsigned char frequencyByte = ((data[headerLocation + 2] & 0x0F) & 0x0C);
	
	int row = sizeof(frequencyRate)/sizeof(frequencyRate[0]);
	int i;
	
	for(i = 0; i< row; i++){
		
		if(frequencyByte == frequencyRate[i][0]){
			
			return frequencyRate[i][1];
		}
	}
	return 1;
}


int findCopyRight(unsigned char * data, int headerLocation)
{
	unsigned char copyRightByte = ((data[headerLocation + 3] & 0x0F) & 0x08);
	
	if(copyRightByte == 0x08)
	{
		return 1; //Return true.
	}
	else{
		return 0; //Return false,
	}
	
}

int findCopyOfOriginalBit(unsigned char * data, int headerLocation)
{
	unsigned char copyBit = ((data[headerLocation + 3] & 0x0F) & 0x04);
	
	if(copyBit == 0x04)
	{
		return 1; //return true;
	}
	else{
		return 0; //return false --> This is a original file;
	}
	
}

/*Read the file into a block of memory*/
unsigned char * readFile(file * myFile)
{
	float size = (*myFile).fileSize;
	FILE * fp = (*myFile).pointerToFile;
	
	printf( "File size: %.2f MB\n", size/ONE_MEGABYTE);
	// Allocate memory on the heap for a copy of the file
	unsigned char * data = (unsigned char *)malloc(size);
	// Read it into our block of memory
	size_t bytesRead = fread( data, sizeof(unsigned char), size, fp );
	
	//unsigned char * fileHeaderLocation = findHeader(data, bytesRead, 0xFF, 0xF0);
	
	
	
	printf("\n");
	//Free data variable. 
	//free(data);
	
	if( bytesRead != size )
	{
		printf( "Error reading file. Unexpected number of bytes read: %zu\n",bytesRead );
		//return(EXIT_FAILURE);
	}
	
	return data;
}


/*Function to get the file name and open it.*/
int initilize(int argc, char *argv[], file * myFile)
{
	// Open the file given on the command line
	if( argc != 2 )
	{
		printf( "Usage: %s filename.mp3\n", argv[0] );
		return(EXIT_FAILURE);
	}
	//Create a global variable for the file pointer.
	FILE * fp = fopen(argv[1], "rb");
	//add the file pointer to the struct variable.
	(*myFile).pointerToFile = fp;
	
	if( fp == NULL )
	{
		printf( "Can't open file %s\n", argv[1] );
		return(EXIT_FAILURE);
	}
	
	return 0;
}

int main( int argc, char ** argv )
{
	
	//Declare the struct.
	file myFile;
	
	//initilize the program and set the struct variable for the file pointer.
	initilize(argc, argv, &myFile);

	//Find and set the size of the file in the struct.
	myFile.fileSize = findFileSize(myFile.pointerToFile);
	
	unsigned char * data = readFile(&myFile);
	
	int fileHeaderLocation = findHeader(data, myFile.fileSize, 0xFF, 0xF0);
	
	//If the method comes back false then exit program.
	//Program requires MPEG Layer 3 to continue.
	if(findVersion(data, fileHeaderLocation) == 1)
	{
		exit(EXIT_FAILURE);
	};
	
	int bitRate = findBitRate(data, fileHeaderLocation);
	
	int frequencyRate = findFrequency(data, fileHeaderLocation);
	
	int copyRight = findCopyRight(data, fileHeaderLocation);
	
	int originalCopy = findCopyOfOriginalBit(data, fileHeaderLocation);
	
	printf("Bit Rate: %d kbps.\n", bitRate);
	
	printf("Frequency Rate: %d kHz.\n", frequencyRate);
	
	if(copyRight == 1)
	{
		printf("This file is copyrighted.\n");
	}
	else{
		printf("This file is not copyrighted.\n");
	}
	
	if(originalCopy == 1)
	{
		printf("This file is the original copy. \n");
	}
	else{
		printf("This file is copy of original media. \n");
	}
	
	
	// Clean up
	
	free(data);
	fclose(myFile.pointerToFile);				// close and free the file
	exit(EXIT_SUCCESS);		// or return 0;
}
