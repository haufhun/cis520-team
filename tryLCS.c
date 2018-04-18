#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void getLCS( char *X, char *Y, int lengthX, int lengthY );
void clearArray(char *array, int lengthArray);

int main(void)
{
	char X[] = "AGGHEOLLOOTABbbAGGHEOLLOOTABbbAGGHEOLLOOTABbbAGGHEOLLOOTABbbAGGHEOLLOOTABbbAGGHEOLLOOTABbbAGGHEOLLOOTABbbAGGHEOLLOOTABbbAGGHEOLLOOTABbbAGGHEOLLOOTABbbAGGHEOLLOOTABbbAGGHEOLLOOTABbbAGGHEOLLOOTABbb";
  	char Y[] = "GXTXHEOLLOOAYBbbGXTXHEOLLOOAYBbbGXTXHEOLLOOAYBbbGXTXHEOLLOOAYBbbGXTXHEOLLOOGGHEOLLOOTABbbAGGHEOLLOOTABbbAGGHEOBbbGXTXHEOLLOOAYBbbGXTXHEOLLOOAYBbbGXTXHEOLLOOAYBbbGXTXHEOLLOOAYBbbGXTXHEOLLOOAYBbbGXTXHEOLLOOAYBbb";
  	int lengthX = strlen(X);
 	int lengthY = strlen(Y);
  	getLCS(X, Y, lengthX, lengthY);
  	return 0;
}


/* Returns the longest common substring of two strings.
	EX. String1 = "333ABCD455"
		String2 = "55652ABCD44456"
		LCS = "ABCD" 							*/
void getLCS( char *X, char *Y, int lengthX, int lengthY )
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
    printf("%s\n", LCS);
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
