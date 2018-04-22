#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
void getLCS( char *X, char *Y, int lengthX, int lengthY, int lineNum1, int lineNum2 );
void clearArray(char *array, int lengthArray);
void ReadFileIntoArray(int startPos, int endPos, char** local_array);
void DoLCS(int);
void PrintArray(void);
void CountLines();

#define NUM_THREADS 4
#define ARRAY_SIZE 1000000
//char char_array[ARRAY_SIZE][2005];
char char_array2[ARRAY_SIZE][2005];
unsigned int line_count = 0;

int main(void)
{
    omp_set_num_threads(NUM_THREADS);

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
         printf("%d-%d: %s\n", i, i+1, char_array2[i]);
    }
}

void DoLCS(int myID)
{
    int lengthX, lengthY, i,j, startPos, endPos;
    
    #pragma omp private(myID, i, j, lengthX, lengthY, startPos, endPos)
    {
        startPos = myID * (line_count / NUM_THREADS);
		endPos = startPos + (line_count / NUM_THREADS);
        
        char char_array[endPos-startPos][2005];
        ReadFileIntoArray(startPos, endPos, (char**)char_array);
        
        for(i = startPos, j = 0; i < endPos; i++, j++)
        {
           lengthX = strlen(char_array[i]);
           lengthY = strlen(char_array[i+1]);
           getLCS(char_array[i], char_array[i+1], lengthX, lengthY, i, i+1);
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
    
    strcpy(char_array2[lineNum1], LCS);
}

void ReadFileIntoArray(int startPos, int endPos, char** local_array)
{
    char* filename = "../wiki_dump.txt";
	FILE *fp;
    int local_count=0;
    int another_count = startPos;
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return;
    }
    char line[2005];
    while (fgets(line, ARRAY_SIZE, fp) != NULL)
	{		
        if(another_count <=endPos)
        {    
            strcpy(local_array[local_count],line);
            another_count++;
        }
		local_count++;      
	}    
	fclose(fp);
    return;
}
void CountLines()
{
    char* filename = "../wiki_dump.txt";
	FILE *fp;
    
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return;
    }
    char line[2005];
    while (fgets(line, ARRAY_SIZE, fp) != NULL)
	{		
		 line_count++;
         
	}    
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
