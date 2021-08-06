#include "reducer.h"
#include <stdio.h>//so we can use file I/O
#include <stdlib.h> // for exit() call
#include <string.h>//so we can use strcat()

finalKeyValueDS *tempoNode;
int reducerID;

// create a key value node
finalKeyValueDS *createFinalKeyValueNode(char *word, int count){
	finalKeyValueDS *newNode = (finalKeyValueDS *)malloc (sizeof(finalKeyValueDS));
	strcpy(newNode -> key, word);
	newNode -> value = count;
	newNode -> next = NULL;
	return newNode;
}

// insert or update an key value
finalKeyValueDS *insertNewKeyValue(finalKeyValueDS *root, char *word, int count){
	finalKeyValueDS *tempNode = root;
	if(root == NULL)
		return createFinalKeyValueNode(word, count);
	while(tempNode -> next != NULL){
		if(strcmp(tempNode -> key, word) == 0){
			tempNode -> value += count;
			return root;
		}
		tempNode = tempNode -> next;
	}
	if(strcmp(tempNode -> key, word) == 0){
		tempNode -> value += count;
	} else{
		tempNode -> next = createFinalKeyValueNode(word, count);
	}
	return root;
}

// free the DS after usage. Call this once you are done with the writing of DS into file
void freeFinalDS(finalKeyValueDS *root) {
    if(root == NULL) return;

    finalKeyValueDS *tempNode = NULL;
    while (root != NULL){
        tempNode = root;
        root = root -> next;
        free(tempNode);
    }
}

//Questions for TA: how to adjust allocation according to string I receive in .txt file, if data structure use makes sense

// reduce function
void reduce(char *key) {
    FILE *iStream;//file pointer; offset

    char word[MAXKEYSZ] = {0};//initialize with null terminated string
  	int wordLength = 0;//index for the word C string
  	int readChar;//holds the particular character read at a given instant

    int numOnes = 0;

    iStream = fopen(key, "r");//open the .txt file: just wish to read the text file at "key" directory

    //check whether file was opened or not
    if(iStream == NULL)
    {
        printf("This file could not be opened");
        fclose(iStream);
        exit(PROCESS_ERROR);
    }

  	/** Algorithm idea
    	Find first space separating the word and the first 1
      then count the number one after that

      word111 1 1 1
             ^
             |
             find this
    */


  	// Find first space
  	while((readChar=fgetc(iStream)) != ' ')
    {
        if(readChar == EOF)
        {
            // Found EOF before the space, so there must be something wrong with the file format
            printf("Reduce_%d: Error, wrong file format %s", reducerID, key);
            exit(PROCESS_ERROR);
        }
        // Save the character of the word
        word[wordLength] = readChar;
        // Increment character counter
        wordLength++;
    }

    //read the .txt file
    while((readChar = fgetc(iStream)) != EOF)
    {
        if(readChar == '1')
        {
            // count number of '1' in the file after the word
            numOnes++;
        }
    }

    // We reached EOF so close it
    fclose(iStream);

    //update the node
    tempoNode = insertNewKeyValue(tempoNode, word, numOnes);
}


// write the contents of the final intermediate structure
// to output/ReduceOut/Reduce_reducerID.txt
void writeFinalDS(int reducerID){
    FILE *oStream;

    //string to hold reducerID
    char path2OutputFile[MAXKEYSZ];

    //Creat the output file for this reducer
    sprintf(path2OutputFile, "./output/ReduceOut/Reducer_%d.txt", reducerID);

    //since the mode is "write", this "open" will create this file
    //open the Reduce_<reducerID>.txt file: just wish to write the contents of IDS into Reduce_reducerID.txt
    oStream = fopen(path2OutputFile, "w");

    if(oStream == NULL)
    {
        // Error
        printf("Ruducer_%d: Encountering error when openning file %s to write\n",reducerID, path2OutputFile);
        fclose(oStream);
    }

    if(tempoNode == NULL)
    {
        printf("Reducer %d: Intermediate data structure is empty\n", reducerID);
        fclose(oStream);
      	exit(PROCESS_ERROR);
    }

    //initialized to first node because tempoNode will be updated
    finalKeyValueDS *start = tempoNode;

    while(tempoNode != NULL)
    {
        // Print the word and the counter value for each word
        fprintf(oStream, "%s %d\n", tempoNode -> key, tempoNode -> value);
        // Move to next link
        tempoNode = tempoNode -> next;
    }
    //done writing to Reduce_reducerID.txt

    //close output stream
    fclose(oStream);

    freeFinalDS(start);
}

int main(int argc, char *argv[]) {

    if(argc < 2)
    {
        printf("Less number of arguments.\n");
        printf("./reducer reducerID");
		exit(PROCESS_ERROR);
    }

    // ###### DO NOT REMOVE ######
    // initialize
    reducerID = strtol(argv[1], NULL, 10);

    // ###### DO NOT REMOVE ######
    // master will continuously send the word.txt files
    // alloted to the reducer
    char key[MAXKEYSZ];
    tempoNode = NULL;
    
    while(getInterData(key, reducerID))
    {
        reduce(key);
    }

    // You may write this logic. You can somehow store the
    // <key, value> count and write to Reduce_reducerID.txt file
    // So you may delete this function and add your logic
    writeFinalDS(reducerID);

    return 0;
}
