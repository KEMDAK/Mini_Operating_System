void printString(char*);
void readString(char*);

int main()
{
	char* line[70];
	printString("Enter a line: \0");
	readString(line);
	printString("  ");
	printString(line);
	printString("\n\r\0");
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
	// interrupt(0x10,ah*256+nl,0,0,0);
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

    // line[index] = 0;
}
