void getWord(char* line, char* word);
int equal(char* s1, char* s2);
int exists(char* file);
void getNumber(int n);
int MOD(int x, int y);
int DIV(int x, int y);

int main()
{
	while(1)
	{
		char input[200];
		char command[30];
		char fileName[10];
		interrupt (0x21, 0, "SHELL>\0", 0, 0);
		interrupt (0x21, 1, input, 0, 0);

		getWord(input, command);
		interrupt(0x21, 0, "\n\r\0", 0, 0);

		if (equal(command, "view\0"))
		{
			char fileName[10];
			char data[13312];
			getWord(input+5, fileName);

			data[0] = '\0';
			interrupt(0x21, 3, fileName, data, 0);
			if (data[0] == '\0')
				interrupt(0x21, 0, "file not found!\n\0", 0, 0);
			else
				interrupt(0x21, 0, data, 0, 0);
		}
		else if (equal(command, "execute\0"))
		{
			char fileName[10];
			getWord(input+8, fileName);
			interrupt(0x21, 4, fileName, 0x2000, 0);
		}
		else if (equal(command, "delete\0"))
		{
			char fileName[10];
			getWord(input+7, fileName);

			if (exists(fileName))
				interrupt(0x21, 7, fileName, 0, 0);
			else
				interrupt(0x21, 0, "file not found!\n\0", 0, 0);
		}
		else if (equal(command, "copy\0"))
		{
			int size1 = 0;
			int i = 0;
			char file1[10];
			char file2[10];
			char data[13312];
			getWord(input+5, file1);

			for (; i < 10; i++)
			{
				if (file1[i] == '\0') break;

				size1++;
			}

			getWord(input+5+size1+1, file2);

			data[0] = '\0';
			interrupt(0x21, 3, file1, data, 0);
			if (data[0] != '\0')
			{
				int sectors = DIV(size1, 512) + 1;
				if (MOD(size1, 512) == 0)
					sectors = sectors - 1;

				interrupt(0x21, 8, file2, data, sectors, 0);
			}
			else
				interrupt(0x21, 0, "file not found!\n\0", 0, 0);

		}
		else if (equal(command, "dir\0"))
		{
			int i = 0;
			char directory[512];
			interrupt(0x21, 2, directory, 2, 0);

			while (i < 16)
			{
				int j;
				int startIndex = i*32;

				char fileName[10];
				char numSec = 0;

				for (j = 0; j < 6; j++)
				{
					if (directory[j + startIndex] == 0)
						break;

					fileName[j] = directory[j + startIndex];
				}

				fileName[j] = '\0';

				for (j = 0; j < 26; j++)
				{
					if (directory[j + 6 + startIndex] == 0)
						break;
					numSec++;
				}

				if (fileName[0] != '\0')
				{
					char c[3];
					getNumber(numSec, c);
					c[2] = '\0';
					interrupt(0x21, 0, fileName, 0, 0);
					interrupt(0x21, 0, "       ", 0, 0);
					interrupt(0x21, 0, c, 0, 0);
					interrupt(0x21, 0, "\n\r\0", 0, 0);
				}

				i++;
			}
		}
		else if (equal(command, "create\0"))
		{
			int i = 0;
			int sectors = 0;
			char fileName[10];
			char data[13312];
			getWord(input+7, fileName);

			while(i < 13311)
			{
				int j;
				char line[80];
				interrupt(0x21, 0, ">\0", 0, 0);
				interrupt (0x21, 1, line, 0, 0);
				if (line[0] == '\0')
					break;

				data[i] = '\r';
				i++;
				for (j = 0; j < 80 && i < 13311; j++, i++)
				{
					if (line[j] == '\0')
					{
						data[i] = '\n';
						i++;
						break;
					}

					data[i] = line[j];
				}


				interrupt(0x21, 0, "\n\r\0", 0, 0);
			}

			sectors = DIV(i, 512) + 1;
			if (MOD(i, 512) == 0)
				sectors = sectors - 1;

			data[i] = '\0';

			interrupt(0x21, 8, fileName, data, sectors, 0);
			interrupt(0x21, 0, "\n\r\0", 0, 0);
		}
		else if (!equal(command, "\0"))
		{
			interrupt(0x21, 0, "NMSK: command not found: \0", 0, 0);
			interrupt(0x21, 0, command + '\0', 0, 0);
			interrupt(0x21, 0, "\n\0", 0, 0);
		}

		interrupt(0x21, 0, "\r\0", 0, 0);
	}
}

void getWord(char* line, char* word)
{
	int index = 0;
	while(line[index] != '\0' && line[index] != ' ')
	{
		word[index] = line[index];
		index = index + 1;
	}

	word[index] = '\0';
}

int equal(char* s1, char* s2)
{
	int index = 0;

	while(s1[index] != '\0' && s2[index] != '\0')
	{
		if (s1[index] != s2[index])
			return 0;
		index = index + 1;
	}

	if (s1[index] == '\0' && s2[index] == '\0')
		return 1;
	else
		return 0;
}

int exists(char* file)
{
	char data[13312];

	data[0] = '\0';
	interrupt(0x21, 3, file, data, 0);
	if (data[0] == '\0')
		return 0;
	return 1;
}

void getNumber(int n, char* c)
{
	switch(n)
	{
		case 1: c[0] = '1'; c[1] = '\0'; break;
		case 2: c[0] =  '2'; c[1] = '\0'; break;
		case 3: c[0] =  '3'; c[1] = '\0'; break;
		case 4: c[0] =  '4'; c[1] = '\0'; break;
		case 5: c[0] =  '5'; c[1] = '\0'; break;
		case 6: c[0] =  '6'; c[1] = '\0'; break;
		case 7: c[0] =  '7'; c[1] = '\0'; break;
		case 8: c[0] =  '8'; c[1] = '\0'; break;
		case 9: c[0] =  '9'; c[1] = '\0'; break;
		case 10: c[0] =  '1'; c[1] = '0'; break;
		case 11: c[0] =  '1'; c[1] = '1'; break;
		case 12: c[0] =  '1'; c[1] = '2'; break;
		case 13: c[0] =  '1'; c[1] = '3'; break;
		case 14: c[0] =  '1'; c[1] = '4'; break;
		case 15: c[0] =  '1'; c[1] = '5'; break;
		case 16: c[0] =  '1'; c[1] = '6'; break;
		case 17: c[0] =  '1'; c[1] = '7'; break;
		case 18: c[0] =  '1'; c[1] = '8'; break;
		case 19: c[0] =  '1'; c[1] = '9'; break;
		case 20: c[0] =  '2'; c[1] = '1'; break;
		case 21: c[0] =  '2'; c[1] = '2'; break;
		case 22: c[0] =  '2'; c[1] = '2'; break;
		case 23: c[0] =  '2'; c[1] = '3'; break;
		case 24: c[0] =  '2'; c[1] = '4'; break;
		case 25: c[0] =  '2'; c[1] = '5'; break;
		case 26: c[0] =  '2'; c[1] = '6'; break;
	}
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
