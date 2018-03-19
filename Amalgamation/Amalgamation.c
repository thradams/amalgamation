#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

struct Header
{
  char * fileName;
  struct Header* pNext;
};

struct Header* s_included = 0;

void MarkAsIncluded(const char* filename)
{
  struct Header* pNew = malloc(sizeof * pNew);
  pNew->fileName = _strdup(filename);
  pNew->pNext = 0;

  if (s_included == NULL)
  {
    s_included = pNew;
  }
  else
  {
    pNew->pNext = s_included;
    s_included = pNew;
  }
}

bool IsAlreadyIncluded(const char* filename)
{
  bool result = false;
  struct Header* pCurrent = s_included;
  while (pCurrent)
  {
    if (strcmp(pCurrent->fileName, filename) == 0)
    {
      result = true;
      break;
    }
    pCurrent = pCurrent->pNext;
  }
  return result;
}


bool Write(char* name, FILE* out)
{
  bool found = false;

  MarkAsIncluded(name);

  char previous = '\0';

  FILE * input = fopen(name, "r");
  if (input)
  {
    found = true;
    char c = '\0';

    while (!feof(input))
    {
      previous = c;
      c = fgetc(input);
      if ((previous == '\0' || previous == '\n') && c == '#')
      {
        c = fgetc(input);
        char linebuffer[500];
        int i = 0;
        linebuffer[i] = '#';
        i++;
        while (c != '\n' && c != EOF)
        {
          linebuffer[i] = c;
          c = fgetc(input);
          i++;
        }
        linebuffer[i] = '\0';
        const char * pChar = linebuffer;

        pChar++; //skip #

        while (*pChar == ' ') //skip spaces
          pChar++;

        if (strncmp(pChar, "pragma", sizeof("pragma") - 1) == 0)
        {
          pChar += sizeof("pragma") - 1; //skip pragma

          while (*pChar == ' ') //skip spaces
            pChar++;

          if (strncmp(pChar, "once", sizeof("once") - 1) == 0)
          {
            //nothing
          }
          else
          {
            fputs(linebuffer, out);
            fputs("\n", out);
          }
        }
        else if (strncmp(pChar, "include", sizeof("include") - 1) == 0)
        {
          pChar += sizeof("include") - 1; //match include

          while (*pChar == ' ') //skip spaces          
            pChar++;

          if (*pChar == '"' || *pChar == '<')
          {
            pChar++;//match " or <

            char fileName[200] = { 0 };
            int k = 0;
            while (*pChar)
            {
              if (*pChar == '\"' || *pChar == '>')
              {
                break;
              }
              fileName[k] = *pChar;
              pChar++;
              k++;
            }

            if (!IsAlreadyIncluded(fileName))
            {
              if (!Write(fileName, out))
              {
                fputs(linebuffer, out);
                fputs("\n", out);
              }
            }
          }
          else
          {
            fputs(linebuffer, out);
            fputs("\n", out);
          }
        }
        else
        {
          fputs(linebuffer, out);
          fputs("\n", out);
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

  return found;
}


int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("usage merge.txt out.c");
		return 1;
	}

	FILE * out = fopen(argv[2], "w");
	if (out)
	{
		Write(argv[1], out);
	}


	return 0;
}

