void printString(char*);
void readString(char*);
void readSector(char*, int);

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
	char line[80];
	makeInterrupt21();
	interrupt(0x21, 1, line, 0, 0);
	printString("\n\r\0");
	interrupt(0x21, 0, line, 0, 0);
	printString("\n\r\0");


	// testing readSector
	// char line[512];
	// makeInterrupt21();
	// interrupt(0x21, 2, line, 30, 0);
	// interrupt(0x21, 0, line, 0, 0);


	// testing error
	// char line[80];
	// makeInterrupt21();
	// interrupt(0x21, 3, line, 0, 0);

	while(1);
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

void handleInterrupt21(int ax, int bx, int cx, int dx) {
	// Was for Task 4
	// printString("Hello World!");
	//-->

	switch(ax) {
		case 0:printString(bx); break;

		case 1:readString(bx); break;

		case 2:readSector(bx, cx); break;

		default: printString("You have entered an AX value greater than 2, don't do that!"); break;			
	}
}