#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
void getLCS( char *X, char *Y, int lengthX, int lengthY, int lineNum1, int lineNum2 );
void clearArray(char *array, int lengthArray);
void ReadFileIntoArray();
void DoLCS(int);

#define NUM_THREADS 1
#define ARRAY_SIZE 1000000
char char_array[ARRAY_SIZE][2005];

unsigned int count = 0;

int main(void)
{
    omp_set_num_threads(NUM_THREADS);

	ReadFileIntoArray();
    #pragma omp parallel 
	{
        //DoLCS(omp_get_thread_num());
    }
  	return 0;
}

void DoLCS(int myID)
{
    int lengthX, lengthY, i, startPos, endPos;
    
    #pragma omp private(myID,theChar,charLoc,local_char_count,startPos,endPos,i,j)
    {
        startPos = myID * (count / NUM_THREADS);
		endPos = startPos + (count / NUM_THREADS);
        char char2[endPos-startPos +1];
        for(i = startPos; i < endPos; i++)
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
	int i, j, t, tempJ, count = 0;

	for(i = 0; i < lengthX; i ++)
	{
		for(j = 0; j < lengthY; j++)
		{
			if(X[i] == Y[j] && X[i+1] == Y[j+1])
			{
				clearArray(tempLCS, lengthX);
				
				tempJ = j;
				count = 0;				
				while(X[i] == Y[tempJ] && i < lengthX && tempJ < lengthY)
				{
                  	tempLCS[count++] =  X[i++];
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
        
    printf("%d-%d: %s\n",lineNum1,lineNum2, LCS);
}

void ReadFileIntoArray()
{
    char* filename = "../wiki_dump.txt";
	
	
	FILE *fp;
    
 
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return;
    }
	
    while (fgets(char_array[count], ARRAY_SIZE, fp) != NULL)
	{		
		 count++;
         
	}    
    printf("count: %d\n\n\n\n", count);
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
