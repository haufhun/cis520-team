#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

void getLCS( char *X, char *Y, int lengthX, int lengthY, int lineNum1, int lineNum2 );
void clearArray(char *array, int lengthArray);
void ReadFileIntoArray(int startPos, int endPos);
void DoLCS(int);
void PrintArray(void);
void CountLines();

#define NUM_THREADS 2
#define ARRAY_SIZE 1000000
#define LINE_SIZE 2005
char _linesOfFile[ARRAY_SIZE][LINE_SIZE];
char _resultLCS[ARRAY_SIZE][LINE_SIZE];
unsigned int line_count = 0;
char * filename = "../wiki_dump.txt";

int main(int argc, char ** argv)
{
    int i;

    omp_set_num_threads(NUM_THREADS);

    // if (argc > 1)
    // {
        // filename = argv[0];
        // line_count = atoi(argv[1]);
    // }

	//ReadFileIntoArray();
    CountLines();
    #pragma omp parallel 
	{
        DoLCS(omp_get_thread_num());
    }
    
    PrintArray();

  	return 0;
}

void PrintArray()
{
    int i;
    for(i = 0; i < line_count; i ++)
    {
         printf("%d-%d: %s\n", i, i+1, _resultLCS[i]);
    }
}

void DoLCS(int myID)
{
    int lengthX, lengthY, i,j, startPos, endPos;
    
    #pragma omp private(myID, i, j, lengthX, lengthY, startPos, endPos)
    {
        
        startPos = myID * (line_count / NUM_THREADS);
		endPos = startPos + (line_count / NUM_THREADS);
        printf("%d startPos: %d\n", myID,startPos);
        printf("endPos: %d\n", endPos);
        
       ReadFileIntoArray(startPos, endPos);
        
      
       for(i = startPos, j = 0; i < endPos; i++, j++)
        {
          lengthX = strlen(_linesOfFile[i]);
          lengthY = strlen(_linesOfFile[i+1]);
          getLCS(_linesOfFile[i], _linesOfFile[i+1], lengthX, lengthY, i, i+1);
        }

    }
}
/* Returns the longest common substring of two strings.
	EX. String1 = "333ABCD455"
		String2 = "55652ABCD44456"
		LCS = "ABCD" 							*/
void getLCS( char *X, char *Y, int lengthX, int lengthY, int lineNum1, int lineNum2 )
{
	/* Common string cant be longer than the shortest string, 
		therefore it does not matter which length you use to 
		initilize the array. 							*/
	char LCS[lengthX];
	char tempLCS[lengthX];
	int i, j, t, tempJ, line_count = 0;

	for(i = 0; i < lengthX; i ++)
	{
		for(j = 0; j < lengthY; j++)
		{
			if(X[i] == Y[j] && X[i+1] == Y[j+1])
			{
				clearArray(tempLCS, lengthX);
				
				tempJ = j;
				line_count = 0;				
				while(X[i] == Y[tempJ] && i < lengthX && tempJ < lengthY)
				{
                  	tempLCS[line_count++] =  X[i++];
				    tempJ++;
				}

				if(strlen(tempLCS) > strlen(LCS))
				{
					for(t = 0; t < lengthX; t++)
					{
						LCS[t] = tempLCS[t];
					}
				}
			}
		}	
	}
    int lengthLCS = strlen(LCS)-1;
    if (LCS[lengthLCS] == '\n' || LCS[lengthLCS] == '\r')
        LCS[lengthLCS] = '\0';    
     
    #pragma omp critical
    { 
       strcpy(_resultLCS[lineNum1], LCS);
    }
}

void ReadFileIntoArray(int startPos, int endPos)
{
	FILE *fp = fopen(filename, "r");
    int index = 0;
    int lineInFile = startPos;
    
    
    if (fp == NULL) {
        printf("Could not open file %s",filename);
        return;
    }
    //printf("ReadFunc1\n");
    

    char line[LINE_SIZE];
    // fgets(*(_linesOfFile + startPos * ARRAY_SIZE), ARRAY_SIZE, fp);
    // **_linesOfFile = 'c';
       // printf("ReadFuncWhile\n");
    #pragma omp critical
    {
        while (fgets(line, LINE_SIZE, fp) != NULL && lineInFile <= endPos)
        {		
            if(lineInFile >= startPos)
            {    
                strcpy(_linesOfFile[lineInFile], line);
                printf("line %d: %s\n", lineInFile, _linesOfFile[lineInFile]);
                //index++;
            }
            lineInFile++;      
        }
    }
        //printf("ReadFuncEnd\n");
	fclose(fp);
    
}
void CountLines()
{
	FILE *fp = fopen(filename, "r");
    
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return;
    }

    char line[LINE_SIZE];
    while (fgets(line, LINE_SIZE, fp) != NULL)
	{		
		 line_count++;
         
	}  
    line_count = 10;
    printf("line_count: %d\n\n\n", line_count);
	fclose(fp);
}

/* Clears the given array */
void clearArray(char *array, int lengthArray)
{
	int t;
	for(t = 0; t < lengthArray; t++)
	{
		array[t] = '\0';	
	}
}