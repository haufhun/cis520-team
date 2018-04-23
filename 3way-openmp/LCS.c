#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>

void getLCS( char *X, char *Y, int lengthX, int lengthY, int lineNum1, int lineNum2 );
void clearArray(char *array, int lengthArray);
void ReadFileIntoArray(int startPos, int endPos);
void DoLCS(int);
void PrintArray(void);
void CountLines();

#define NUM_THREADS 1
#define ARRAY_SIZE 1000000
#define LINE_SIZE 2005
char _linesOfFile[ARRAY_SIZE][LINE_SIZE];
char _resultLCS[ARRAY_SIZE][LINE_SIZE];
unsigned int line_count = 0;
char * filename = "../../wiki_dump.txt";

int main(int argc, char ** argv)
{
    struct timeval t1, t2;
    double elapsedTime;
    
	gettimeofday(&t1, NULL);
    
    printf("DEBUG: starting CLI parsing on %s\n", getenv("HOSTNAME"));

    omp_set_num_threads(NUM_THREADS);
    
    if (argc > 1)
    {
        if(atoi(argv[1]) <= 0)
        {
            printf("Invalid line count given.\n"); 
            return -1;
        }
        
        line_count = atoi(argv[1]);
    }
    else
        CountLines();
  

    printf("DEBUG: starting DoLCS.\n");
    
    #pragma omp parallel 
	{
        DoLCS(omp_get_thread_num());
    }
    
    printf("DEBUG: starting printing LCS.\n");
    
    PrintArray();
    
    gettimeofday(&t2, NULL);
    
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms

    printf("DATA, %d, %s, %f\n", line_count, getenv("SLURM_CPUS_ON_NODE"),  elapsedTime);
    
  	return 0;
}

void PrintArray()
{
    int i;
    for(i = 0; i < line_count-1; i ++)
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
        endPos = (myID == NUM_THREADS-1) ? line_count : startPos + (line_count / NUM_THREADS);
        
        ReadFileIntoArray(startPos, endPos);
        
       for(i = startPos, j = 0; i < endPos && i+1 < line_count; i++, j++)
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
    
    clearArray(LCS,lengthX);

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
                
                int lengthLCS = strlen(tempLCS)-1;
                if (tempLCS[lengthLCS] == '\n' || tempLCS[lengthLCS] == '\r')
                    tempLCS[lengthLCS] = '\0';   
                
				if(strlen(tempLCS) > strlen(LCS))
				{
					for(t = 0; t < strlen(tempLCS); t++)
					{
						LCS[t] = tempLCS[t];
					}
				}
			}
		}	
	}
    
    #pragma omp critical
    { 
       strcpy(_resultLCS[lineNum1], LCS);                     
    }
}

void ReadFileIntoArray(int startPos, int endPos)
{
	FILE *fp = fopen(filename, "r");
    int lineInFile = 0;
    
    
    if (fp == NULL) {
        printf("Could not open file %s",filename);
        return;
    }
    #pragma omp critical
    {
        char line[LINE_SIZE];
        while (fgets(line, LINE_SIZE, fp) != NULL && lineInFile < endPos)
        {
            if(lineInFile >= startPos)
            {    
                strcpy(_linesOfFile[lineInFile], line);
            }
            lineInFile++;      
        }
    }
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