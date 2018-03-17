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
	//colocar em uma lista de ja incluido

	FILE * input = fopen(name, "r");
	if (input)
	{
		while (!feof(input))
		{
			char c = fgetc(input);
			if (c == '#')
			{
				char buffer[500];
				c = fgetc(input);
				if (c == 'i')
				{
					if (!Write("input2.txt", out))
					{
						//imprimir linha
					}
				}
				else if (c != EOF)
				{
					fputc(c, out);
				}
			}
			else if (c != EOF)
			{
				fputc(c, out);
			}
		}
	}
	else
	{

	}
	//returnar false se nao incluiu
	return true;
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
