//****************************************************************************
//psearch.c 	Batuhan Tüter / 21200624
//
//This program takes number of input files from command-line and
//takes a parameter. If a line in any of the files includes the parameter,
//that line is added to the output file in sorted order
//****************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


typedef struct node NODE;
struct node {
    int lineNum;
    char * token;
    char * name;
    NODE * next;
};

void add(NODE ** list,char * name, char * token, int lineNum);
void write(NODE ** list, FILE * file);
int findIndexofLargest(char * theArray [], int size);
 
int main(int argc, char *argv[]) 
{
	//Timer begins
	clock_t begin, end;
	double time_spent;
	begin = clock();

	char * keyWord = argv[1];
	int inputCount = atoi(argv[2]);

	FILE *inputFile;
	NODE *lists[inputCount];
	FILE *tempFile;

	char name[80];
	size_t nbytes = 0;
	char * myString = (char *) malloc(nbytes);
	int bytes_read;
	int i;

	//Creation of temporary files
	for(i = 0;i<inputCount;++i)
	{
		sprintf(name, "%s%d","Temp", i);
		tempFile = fopen(name,"w+");
		fclose(tempFile);
	}
	
	//Reading input files and creating temporary files
        for (i = 0; i < inputCount; i++)
        {
		int n;
		n = fork();
		if(n==0)
		{
			inputFile = fopen(argv[i+3], "r");
			lists[i]=NULL;
			int lineNumber =0;
			
			while((bytes_read = getline(&myString, &nbytes, inputFile)) > 0) 
			{   
			    lineNumber++;
			    char * token = malloc((bytes_read) * (sizeof(char)));
			    if(myString[bytes_read - 1]=='\n')//Reading the file lines
			    {
			    	    strncpy(token, myString, ((bytes_read - 1) * (sizeof(char))));//Discard the \n character
			    	    token[bytes_read - 1] = '\0';
			    }
			    else//Last line to be read in the file
				strncpy(token, myString, ((bytes_read) * (sizeof(char))));
			    char *ret = strstr(token, keyWord);
			    if(ret)
			    {
				add(&lists[i],argv[i+3], token, lineNumber);
			    }
			    else
				free(token);
			}
			//Write the list in to temp file
			sprintf(name, "%s%d","Temp", i);
			tempFile = fopen(name,"w+");
		        write(&lists[i],tempFile);

			//Deallocate memory from created lists
		        NODE * temp = lists[i];
		        while(temp != NULL) 
		        {
				NODE * nextItem = temp->next;
				free(temp->token);
				free(temp);
				temp = nextItem;
		        }
		        fclose(inputFile);
		        fclose(tempFile);
		        exit(0);
		}
    	 }

	FILE * output = fopen(argv[argc-1], "w");
	char * sorted[1024];
	int sizeSorted=0;
	//Wait child processes
	for(i=0; i<inputCount;i++)
	{
		wait();
	}
	//Read temp files and save their content
	for(i=0;i<inputCount;i++)	
	{
		sprintf(name, "%s%d","Temp", i);
		tempFile = fopen(name,"r");
		if(!tempFile)
			printf("Error opening temp file!");
		while((bytes_read = getline(&myString, &nbytes, tempFile)) > 0)
		{	
			char * token = malloc((bytes_read) * (sizeof(char)));
			if(myString[bytes_read - 1]=='\n')//Reading the file lines
			{
			    strncpy(token, myString, ((bytes_read - 1) * (sizeof(char))));//Discard the \n character
			    token[bytes_read - 1] = '\0';
			}
			else//Last line to be read in the file
				strncpy(token, myString, ((bytes_read) * (sizeof(char))));
			
			strcat(token,"\n");
			sorted[sizeSorted]=token;
			++sizeSorted;
		}
		fclose(tempFile);
		remove(name);
		
	}
	//Sorting the content array
	int last;
	for(last = sizeSorted-1; last>=1; last--)
	{
		int largest=findIndexofLargest(sorted,last+1);
		//SWAP
		char * temp=sorted[largest];
		sorted[largest]=sorted[last];
		sorted[last]=temp;
	}

	//After sorting the list
	for( i = 0 ; i < sizeSorted; i++ ) 
	{   
		fputs(sorted[i], output);
		free(sorted[i]);
	}
	
	if(myString) 
	{
		free(myString);
	}

	fclose(output);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	//printf("Execution time of the program is : %f seconds.\n",time_spent);
	return 0;
} 
//This function is inherited from the book "Data Abstraction & Problem Solving with C++ 6th edition "  
int findIndexofLargest(char * theArray [], int size)
{
	int indexSoFar=0;
	int currentIndex;
	for(currentIndex=1; currentIndex < size; currentIndex++)
	{
		if(strcmp(theArray[currentIndex],theArray[indexSoFar])>0)
		{
			indexSoFar = currentIndex;
		}
	}
	return indexSoFar;
}
void add(NODE ** list,char * name, char * token, int lineNum) 
{
        NODE * item = (NODE *) malloc(sizeof(NODE));
        item->token = token;
        item->name = name;
        item->lineNum = lineNum;
        item->next = NULL;
        if((*list) == NULL) 
        {
                *list = item;
        } 
	else 
    	{	//Find the insertion place
                while(((*list)->next) != NULL) 
	        {
		        int result = strcmp(((*list)->next)->token, token);
		        if(result > 0) 
		            	break;
		        else 
				list = &((*list)->next);
                }
                int result = strcmp((*list)->token, token);
                if(result > 0) //Insert at the beginning
	        {
                        item->next = (*list);
                        (*list) = item;
                } 
		        else //insert between or to the end
		        { 
                        NODE * temp = (*list)->next;
                        (*list)->next = item;
                        item->next = temp;
                }
        }
}
void write(NODE ** list, FILE * file) 
{
	NODE * temp = (*list);
        while(temp != NULL) 
	{
                fprintf(file, "%s%s%s%s%d%s\n",temp->name,": ", temp->token, " (",temp->lineNum,")");
                temp = temp->next;
        }
}
