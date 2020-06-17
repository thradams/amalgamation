#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include  	<ctype.h>
#include <assert.h>

int strcicmp(char const* a, char const* b)
{
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}


struct Header
{
    char* fileName;
    struct Header* pNext;
};


void MarkAsIncludedCore(const char* filename, struct Header** s_included)
{

    struct Header* pNew = malloc(sizeof * pNew);
    pNew->fileName = _strdup(filename);
    pNew->pNext = 0;

    if (*s_included == NULL)
    {
        *s_included = pNew;
    }
    else
    {
        pNew->pNext = *s_included;
        *s_included = pNew;
    }
}


void MarkAsIncluded(const char* filename0, struct Header** s_included)
{
    char filename[200];
    _fullpath(
        filename,
        filename0,
        200
    );
    printf("filename %s \n", filename);
    MarkAsIncludedCore(filename, s_included);
}

bool IsAlreadyIncluded(const char* filename0, struct Header** s_included)
{
    char filename[200];
    _fullpath(
        filename,
        filename0,
        200
    );

    bool result = false;
    struct Header* pCurrent = *s_included;
    while (pCurrent)
    {
        if (strcicmp(pCurrent->fileName, filename) == 0)
        {
            result = true;
            break;
        }
        pCurrent = pCurrent->pNext;
    }
    return result;
}

void FreeList(struct Header** s_included)
{
    struct Header* pCurrent = *s_included;
    while (pCurrent)
    {
        struct Header* pNext = pCurrent->pNext;
        free(pCurrent->fileName);
        free(pCurrent);
        pCurrent = pNext;
    }

}

int EndsWith(const char* str, const char* suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strcicmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

bool Write(char* name, FILE* out, struct Header** s_included)
{

    bool cppfile = EndsWith(name, ".c");

    fputs("\n", out);
    bool found = false;

    assert(!IsAlreadyIncluded(name, s_included));
    MarkAsIncluded(name, s_included);

    struct Header* defineList = 0;


    char previous = '\0';

    FILE* input = fopen(name, "r");
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
                const char* pChar = linebuffer;

                pChar++; //skip #

                while (*pChar == ' ') //skip spaces
                    pChar++;

                if (strncmp(pChar, "line", sizeof("line") - 1) == 0)
                {
                    fputs("//", out);
                    fputs(linebuffer, out);
                    fputs("\n", out);
                }
                else if (strncmp(pChar, "define", sizeof("define") - 1) == 0)
                {
                    
                    fputs(linebuffer, out);
                    fputs("\n", out);

                    if (cppfile)
                    {
                        //guardar todos defines feitos em ccpp e fazer undef
                        pChar += sizeof("define") - 1; //skip pragma

                        while (*pChar == ' ') //skip spaces
                            pChar++;

                        char defineName[200] = { 0 };
                        int k = 0;
                        while (*pChar != '\0' && *pChar != ' ' && *pChar != '(' && *pChar != '\n')
                        {
                            defineName[k] = *pChar;
                            k++;
                            pChar++;
                        }
                        MarkAsIncludedCore(defineName, &defineList);
                    }
                }
                else if (strncmp(pChar, "pragma", sizeof("pragma") - 1) == 0)
                {
                    pChar += sizeof("pragma") - 1; //skip pragma

                    while (*pChar == ' ') //skip spaces
                        pChar++;

                    if (strncmp(pChar, "once", sizeof("once") - 1) == 0)
                    {
                        fputs("//", out);
                        fputs(linebuffer, out);
                        fputs("\n", out);
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

                        if (!IsAlreadyIncluded(fileName, s_included))
                        {
                            fputs("\n", out);
                            if (!Write(fileName, out, s_included))
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

        fprintf(out, "\n");
        struct Header* pCurrent = defineList;
        while (pCurrent)
        {
            struct Header* pNext = pCurrent->pNext;
            fprintf(out, "#undef %s \n", pCurrent->fileName);
            

            free(pCurrent->fileName);
            free(pCurrent);
            pCurrent = pNext;
        }
        
        fclose(input);
    }

    
    return found;
}


void almagamate(const char* file_name_out, const char* files[], int count)
{
    struct Header* s_included = 0;


    FILE* out = fopen(file_name_out, "w");
    if (out)
    {
        for (int i = 0; i < count; i++)
        {
            
            if (!IsAlreadyIncluded(files[i], &s_included))
            {
                Write(files[i], out, &s_included);
            }
            
        }
        fclose(out);
    }

    FreeList(&s_included);

}

int main()
{
    const char* files[] = {
        "duktape.c",
        
        "console.c",
        "Error.c",
        "StrBuilder.c",
        "fs.c",
        "Stream.c",
        "JsonScanner.c",
        "TaskQueue.c",
        "ThreadPool.c",
        "Board.c",
        "BoardCore.c",        
        "Socket.c",
        "HttpConnection.c",
        "HttpServer.c",
        "UITask.c",
        "Actor.c"
        "tinycthread.c",
    };

    almagamate("out.c", files, (sizeof(files) / sizeof(files[0])));

}
