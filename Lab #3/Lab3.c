/* CS315 Lab 3: C data types */

#include <stdio.h>
/*Included for use of exits.*/
#include <stdlib.h>

#define ONE_MEGABYTE	1048576.00
#define MAX_FILE_SIZE	10485760

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
the first occurance of this sequence as an unsigned char pointer
to the first byte.*/
unsigned char * findHeader(unsigned char * data, size_t fileSize, unsigned char byteToFind, unsigned char secondNibb)
{
	int i;
	
	for(i = 0; i < fileSize; i++)
	{
		//Look at the first byte and compare to 0xFF and then 
		//increment by one for the next byte, alter byte with 0xF0 (1111 0000)
		//to see if it compares to 0xF0
		if(data[i] == byteToFind && (data[i+1] & 0xF0) == secondNibb)
		{
			printf("found: %02x ", data[i]);
			printf("%02x ", (data[i+1]& 0xF0));
			return &data[i];
		}
		
	}
	return 1; //The method did not find the header. 
}

/*Read the file into a block of memory*/
int readFile(file * myFile)
{
	float size = (*myFile).fileSize;
	FILE * fp = (*myFile).pointerToFile;
	
	printf( "File size: %.2f MB\n", size/ONE_MEGABYTE);
	// Allocate memory on the heap for a copy of the file
	unsigned char * data = (unsigned char *)malloc(size);
	// Read it into our block of memory
	size_t bytesRead = fread( data, sizeof(unsigned char), size, fp );
	
	unsigned char * fileHeaderLocation = findHeader(data, bytesRead, 0xFF, 0xF0);
	
	printf("\n");
	//Free data variable. 
	free(data);
	
	if( bytesRead != size )
	{
		printf( "Error reading file. Unexpected number of bytes read: %zu\n",bytesRead );
		return(EXIT_FAILURE);
	}
	
	return 0;
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
	
	readFile(&myFile);
	
	// Clean up

	fclose(myFile.pointerToFile);				// close and free the file
	exit(EXIT_SUCCESS);		// or return 0;
}
