#include <stdio.h>

struct Header
{
	char * fileName;
	struct Header* pNext;
};

struct Header* s_included = 0;

//lista de ja incluido

bool Write(const char* name, FILE* out)
{
	bool found = false;
	//colocar em uma lista de ja incluido

	FILE * input = fopen(name, "r");
	if (input)
	{
		found = true;

		while (!feof(input))
		{
			char c = fgetc(input);
			if (c == '#')
			{
				c = fgetc(input);

				char buffer[500];
				int i = 0;
				buffer[i] = '#';
				i++;
				while (c != '\n' && c != EOF)
				{
					buffer[i] = c;
					c = fgetc(input);
					i++;
				} 
				buffer[i] = '\0';
				if (buffer[1] == 'i' &&
					buffer[2] == 'n' &&
					buffer[3] == 'c' &&
					buffer[4] == 'l' &&
					buffer[5] == 'u' &&
					buffer[6] == 'd' &&
					buffer[7] == 'e' &&
					buffer[8] == ' ' &&
					buffer[9] == '"')
				{
					int k = 10;
					while (buffer[k])
					{
						if (buffer[k] == '"')
						{
							buffer[k] = 0;
							break;
						}
						k++;
					}
					if (!Write(&buffer[10], out))
					{
						//imprimir linha
					}
					//marcar como ja incluido
				}
				else
				{
					fputs(buffer, out);
				}
			}
			else
			{
				if (c != EOF)
				{
					fputc(c, out);
				}
			}
		}
	}
	//returnar false se nao incluiu
	return found;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("usage ");
		return 1;
	}
	
	FILE * out = fopen("out.txt", "w");
	if (out)
	{
		Write(argv[1], out);
	}


	return 0;
}
