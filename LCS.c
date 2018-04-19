#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
void getLCS( char *X, char *Y, int lengthX, int lengthY, int lineNum1, int lineNum2 );
void clearArray(char *array, int lengthArray);
#define MAXCHAR 1000
int main(void)
{
	char* filename = "../wiki_dump.txt";
	char X[MAXCHAR];
	char Y[MAXCHAR];
	int lengthX, lengthY, count = 0;
	FILE *fp;
    
 
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }



	fgets(X, MAXCHAR, fp);
	
    while (fgets(Y, MAXCHAR, fp) != NULL && count <10000)
	{		
		lengthX = strlen(X);
	 	lengthY = strlen(Y);
		
	 	getLCS(X, Y, lengthX, lengthY, count, count+1);
		// printf("\n%s\n", X);
		strcpy(X,Y);
		// printf("\n%s\n", X);
		count++;
	}
	fclose(fp);
  	return 0;
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
    printf("%d-%d: %s\n",lineNum1,lineNum2, LCS);
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
