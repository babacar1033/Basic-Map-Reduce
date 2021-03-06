#include "mapper.h"

// Pointer pointing to the root of the intermediate data
static intermediateDS* interDSRoot = NULL;

// combined value list corresponding to a word <1,1,1,1....>
valueList *createNewValueListNode(char *value){
	valueList *newNode = (valueList *)malloc (sizeof(valueList));
	strcpy(newNode -> value, value);
	newNode -> next = NULL;
	return newNode;
}

// insert new count to value list
valueList *insertNewValueToList(valueList *root, char *count){
	valueList *tempNode = root;
	if(root == NULL)
		return createNewValueListNode(count);
	while(tempNode -> next != NULL)
		tempNode = tempNode -> next;
	tempNode -> next = createNewValueListNode(count);
	return root;
}

// free value list
void freeValueList(valueList *root) {
	if(root == NULL) return;

	valueList *tempNode = NULL;
	while (root != NULL){
		tempNode = root;
		root = root -> next;
		free(tempNode);
	}
}

// create <word, value list>
intermediateDS *createNewInterDSNode(char *word, char *count){
	intermediateDS *newNode = (intermediateDS *)malloc (sizeof(intermediateDS));
	strcpy(newNode -> key, word);
	newNode -> value = NULL;
	newNode -> value = insertNewValueToList(newNode -> value, count);
	newNode -> next = NULL;
	return newNode;
}

// insert or update a <word, value> to intermediate DS
intermediateDS *insertPairToInterDS(intermediateDS *root, char *word, char *count){
	intermediateDS *tempNode = root;
	if(root == NULL)
		return createNewInterDSNode(word, count);
	while(tempNode->next != NULL) {
		if(strcmp(tempNode -> key, word) == 0){
			tempNode -> value = insertNewValueToList(tempNode -> value, count);
			return root;
		}
		tempNode = tempNode -> next;		
	}
	if(strcmp(tempNode -> key, word) == 0){
		tempNode -> value = insertNewValueToList(tempNode -> value, count);
	} else {
		tempNode -> next = createNewInterDSNode(word, count);
	}
	return root;
}

// free the DS after usage. Call this once you are done with the writing of DS into file
void freeInterDS(intermediateDS *root) {
	if(root == NULL) return;

	intermediateDS *tempNode = NULL;
	while (root != NULL){
		tempNode = root;
		root = root -> next;
		freeValueList(tempNode -> value);
		free(tempNode);
	}
}

// emit the <key, value> into intermediate DS 
void emit(char *key, char *value) {
	interDSRoot = insertPairToInterDS(interDSRoot, key, value);
}

// map function
void map(char *chunkData){
	// Retriving word from chunk data
	int i = 0;
	char *buffer;
	while ((buffer = getWord(chunkData, &i)) != NULL){
		// Emit the <key, value> into intermediate DS 
		emit(buffer, "1");
		// Free the buffer to prevent memory leak
		free(buffer);
	}
}

// write intermediate data to separate word.txt files
// Each file will have only one line : word 1 1 1 1 1 ...
void writeIntermediateDS() {
	intermediateDS* itInterDS = interDSRoot;	// Iterator to traverse the intermediate data structure
	valueList* itValue = NULL;	// Iterator to traverse the second level linked list of each word
	FILE * wordTXT;	// File pointer to the word.txt of each key

	while(itInterDS != NULL)
	{
		// Get the root of the value list of this key
		itValue = itInterDS->value;
		// Allocate space to save the path to the word.txt path
		char* keyOutPath = (char*) calloc(strlen(mapOutDir) + strlen(itInterDS->key) + 5, sizeof(char)); // +5 = strlen(".txt\0")
		// Form the path to output key file
		sprintf(keyOutPath, "%s/%s.txt", mapOutDir, itInterDS->key);
		// Open the word.txt to edit
		if((wordTXT = fopen(keyOutPath, "w")) == NULL)
		{
			// Error happen
			printf("Mapper_%d: Error openning file %s\n", mapperID, keyOutPath);
			// Close as a good practice
			fclose(wordTXT);
			// Exit with error status code
			exit(PROCESS_ERROR);
		}
		// Write the word first "word"
		fprintf(wordTXT, "%s", itInterDS->key);
		// Then write the 1 1 1 1 1 ... list
		while(itValue != NULL)
		{
			fprintf(wordTXT, " %s", itValue->value);
			itValue = itValue->next;
		}
		// Add newline at the end of file
		fprintf(wordTXT,"\n");
		// Close file to save every thing to file
		fclose(wordTXT);
		// Free allocated memory to prevent memeory leakage
		free(keyOutPath);
		// Move on to the next key
		itInterDS = itInterDS->next;
	}
}

int main(int argc, char *argv[]) {
	
	if (argc < 2) {
		printf("Less number of arguments.\n");
		printf("./mapper mapperID\n");
		exit(PROCESS_ERROR);
	}
	// ###### DO NOT REMOVE ######
	mapperID = strtol(argv[1], NULL, 10);
	// ###### DO NOT REMOVE ######
	// create folder specifically for this mapper in output/MapOut
	// mapOutDir has the path to the folder where the outputs of 
	// this mapper should be stored
	mapOutDir = createMapDir(mapperID);

	// ###### DO NOT REMOVE ######
	while(1) {
		// create an array of chunkSize=1024B and intialize all 
		// elements with '\0'
		char chunkData[chunkSize + 1]; // +1 for '\0'
		memset(chunkData, '\0', chunkSize + 1);

		char *retChunk = getChunkData(mapperID);
		if(retChunk == NULL) {
			break;
		}

		strcpy(chunkData, retChunk);
		free(retChunk);

		map(chunkData);
	}

	// ###### DO NOT REMOVE ######
	writeIntermediateDS();

	// Free intermidate DS to prevent memory leakage
	freeInterDS(interDSRoot);

	return 0;
}