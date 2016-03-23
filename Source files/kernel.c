void printString(char*);
void readString(char*);
void readSector(char*, int);
void writeSector(char*, int);
void readFile(char* fileName, char* buffer);
void deleteFile(char* name);
void terminate();
void writeFile(char* name, char* buffer, int secNum);
int findFreeDirectory(char* directory);
int findFreeSector(char* map);
void terminate();

int main()
{
	// Task 1, Task 2
	// char line[80];
	// printString("Enter a line: \0");
	// readString(line);
	// printString("\n\r\0");
	// printString(line);
	// printString("\n\r\0");

	// Task 3
	// char buffer[512];
	// readSector(buffer, 30);
	// printString(buffer);

	// Task 4
	// makeInterrupt21();
	// interrupt(0x21, 0, 0, 0, 0);

	// Taks 5

	// testing printString and readString
	// char line[80];
	// makeInterrupt21();
	// interrupt(0x21, 1, line, 0, 0);
	// printString("\n\r\0");
	// interrupt(0x21, 0, line, 0, 0);
	// printString("\n\r\0");


	// testing readSector
	// char line[512];
	// makeInterrupt21();
	// interrupt(0x21, 2, line, 30, 0);
	// interrupt(0x21, 0, line, 0, 0);


	// testing error
	// char line[80];
	// makeInterrupt21();
	// interrupt(0x21, 3, line, 0, 0);

	char buffer1[13312]; /*this is the maximum size of a file*/
	makeInterrupt21();
	interrupt(0x21, 3, "messag\0", buffer1, 0); /*read the file into buffer*/
	interrupt(0x21, 0, buffer1, 0, 0); /*print out the file*/

	// char buffer[13312];
	// makeInterrupt21();
	// interrupt(0x21, 7, "messag\0", 0, 0); //delete messag
	// interrupt(0x21, 3, "messag\0", buffer, 0); // try to read messag
	// interrupt(0x21, 0, buffer, 0, 0); //print out the contents of buffer

	while(1); /*hang up*/
	// char buffer[13312]; /*this is the maximum size of a file*/
	// makeInterrupt21();
	// interrupt(0x21, 3, "messag\0", buffer, 0); /*read the file into buffer*/
	// interrupt(0x21, 0, buffer, 0, 0); /*print out the file*/
	// while(1); /*hang up*/

	// testing writeFile
	// int i=0;
	// char buffer1[13312];
	// char buffer2[13312];
	// buffer2[0]='h'; buffer2[1]='e'; buffer2[2]='l'; buffer2[3]='l'; buffer2[4]='o';
	// for(i=5; i<13312; i++) buffer2[i]=0x0;
	// makeInterrupt21();
	// interrupt(0x21,8, "testW\0", buffer2, 1); //write file testW
	// interrupt(0x21,3, "testW\0", buffer1, 0); //read file testW
	// interrupt(0x21,0, buffer1, 0, 0); // print out contents of testW
	// terminate();
}

void printString(char* chars)
{
	int i = 0;
	char ah = 0xE;
	while(chars[i] != '\0'){
		char al = chars[i];
		int ax = ah * 256 + al;
		interrupt(0x10, ax, 0,0,0);
		i = i + 1;
	}
}

void readString(char* line)
{
	int index = 0;
	char c = interrupt(0x16, 0, 0, 0, 0);

	while((c != 0xd)) {
		interrupt(0x10, 0xE * 256 + c, 0, 0, 0);

		/*0x8 = backspace*/
		if(c != 0x8) {
			line[index] = c;
			index++;
		}else {
			if(index > 0) {
				interrupt(0x10, 0xE * 256 + '\0', 0, 0, 0);
				interrupt(0x10, 0xE * 256 + c, 0, 0, 0);
				index--;
			}
		}

		c = interrupt(0x16, 0, 0, 0, 0);
	}

	if(c == 0xd){
		char lineFeed = 0xa;
		char endString = 0x0;
		line[index] = lineFeed;
		line[index+1] = endString;
	}
}

void readSector(char* buffer, int sector) {
	int relativeSector = MOD(sector, 18) + 1;
	int head = MOD(DIV(sector, 18), 2);
	int track = DIV(sector, 36);
	int AX = 2 * 256 + 1;
	int BX = buffer;
	int CX = track * 256 + relativeSector;
	int DX = head * 256 + 0;

	interrupt(0x13, AX, BX, CX, DX);
}

int DIV(int x, int y) {
	int res = 0;

	while (x > 0) {
		x = x - y;
		res = res + 1;
	}

	if(x < 0)
	res = res - 1;

	return res;
}

int MOD(int x, int y) {
	int res = 0;

	int tempX = x;
	while (tempX > 0) {
		tempX = tempX - y;
		res = res + 1;
	}

	if(tempX < 0)
	res = res - 1;

	return x - (res * y);
}

void readFile(char* fileName, char* buffer) {
	int i = 0;
	int found = 0;

	char directory[512];
	readSector(directory, 2);

	while (i < 16 && !found)
	{
		int cur = 1;
		int j;
		int startIndex = i*32;

		for (j = 0; j < 6 && cur; j++)
		{

			if (fileName[j] == '\0') break;

			if (fileName[j] != directory[j + startIndex])
			cur = 0;
		}

		if (cur)
		{
			int index = 0;
			char tempBuffer[512];
			found = 1;
			for (j = 6; j < 26; j++)
			{
				int k = 0;
				if (directory[j + startIndex] == 0) break;

				readSector(tempBuffer, directory[j + startIndex]);

				for (; k < 512; k++)
				buffer[index++] = tempBuffer[k];
			}
		}

		i++;
	}

	if (!found)
	return;
}

void writeFile(char* name, char* buffer, int secNum) {
	int freeDirectory, i, j, bufferIndex;
	char sectorBuffer[512];
	char map[512];
	char directory[512];

	readSector(map, 1);
	readSector(directory, 2);

	// searching for free directory
	freeDirectory = findFreeDirectory(directory);

	// writting the file name
	for(i = 0; i < 6; i++) {
		if(name[i] == '\0'){
			for(; i < 6; i++) {
				directory[freeDirectory + i] = 0;
			}

			break;
		}
		else {
			directory[freeDirectory + i] = name[i];
		}
	}

	// writing the file content
	bufferIndex = 0;
	for(i = 6; i < secNum + 6; i++) {
		int freeSector = findFreeSector(map);
		map[freeSector] = 1;

		directory[freeDirectory + i] = freeSector;

		// build the sector buffer
		for(j = 0; j < 512; j++) {
			sectorBuffer[j] = buffer[bufferIndex++];
		}

		// writting the sector content
		writeSector(sectorBuffer, freeSector);
	}

	// writting the map and directory back
	writeSector(map, 1);
	writeSector(directory, 2);
}

int findFreeDirectory(char* directory) {
	int freeDirectory;
	for(freeDirectory = 0; freeDirectory < 16; freeDirectory++) {
		if(directory[freeDirectory * 32] == 0){
			break;
		}
	}
	freeDirectory = freeDirectory * 32;

	return freeDirectory;
}

int findFreeSector(char* map) {
	int freeSector;
	for(freeSector = 0; freeSector < 512; freeSector++) {
		if(map[freeSector] == 0){
			break;
		}
	}

	return freeSector;
}

void deleteFile(char* name)
{
	int i = 0;
	int found = 0;

	char directory[512];
	readSector(directory, 2);

	while (i < 16 && !found)
	{
		int cur = 1;
		int j;
		int startIndex = i*32;

		for (j = 0; j < 6 && cur; j++)
		{

			if (name[j] == '\0') break;

			if (name[j] != directory[j + startIndex])
				cur = 0;
		}

		if (cur) 
		{
			int j = startIndex + 6;
			char map[512];
			readSector(map, 1);
			directory[startIndex] = 0;


			for (; j < startIndex + 32; j++)
			{
				if (directory[j] == 0) break;

				map[directory[j]] = 0;
			}

			writeSector(directory, 2);
			writeSector(map, 1);

			return;
		}

		i++;
	}

	if (!found)
		return;	
}

void terminate(){
	while(1);
}


void writeSector(char* buffer, int sector) {
	int relativeSector = MOD(sector, 18) + 1;
	int head = MOD(DIV(sector, 18), 2);
	int track = DIV(sector, 36);
	int AX = 3 * 256 + 1;
	int BX = buffer;
	int CX = track * 256 + relativeSector;
	int DX = head * 256 + 0;

	interrupt(0x13, AX, BX, CX, DX);
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
	// Was for Task 4
	// printString("Hello World!");
	//-->

	switch(ax) {
		case 0:printString(bx); break;

		case 1:readString(bx); break;

		case 2:readSector(bx, cx); break;

		case 3:readFile(bx, cx); break;

		case 5:terminate(); break;

		case 6:writeSector(bx, cx); break;

		case 7:deleteFile(bx); break;

		case 8:writeFile(bx, cx, dx); break;

		default: printString("You have entered an AX value that is not defined, don't do that!"); break;
	}
}

