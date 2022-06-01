#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"
#include "heap.h"

#define FILENAME "file"

/*sdi180065*/
int main(int argc, char** argv)
{
	if(argc<2)
	{
		printf("A records file hasnt been given\n");
		return -1;
	}

	HP_info *info;
	Record record;


    FILE* file = fopen(argv[1], "r"); 
    char line[256];
	
	BF_Init();
	
	printf("Creating heap file\n");
    if ( HP_CreateFile(FILENAME, 'i', "id", sizeof(int)) < 0 )
	{
		printf("Error creating heap file \n");
		exit(EXIT_FAILURE);
	}
	
	printf("Opening heap file\n");
	if ((info = HP_OpenFile(FILENAME)) == NULL) 
	{
		printf("Error opening heap file\n");
		exit(EXIT_FAILURE);
	}
	int j,k,i;
	char idc[15];
	char name[15];
	char sur[25];
	char address[50];
	char* scan;
	printf("Inserting all records from the file \n");
	while (fgets(line, sizeof(line), file)) 
	{ 
		scan=strtok(line,"{\",\"}\n");
		record.id=atoi(scan);
		scan=strtok(NULL,"{\",\"}\n");
		strcpy(record.name,scan);
		scan=strtok(NULL,"{\",\"}\n");
		strcpy(record.surname,scan);
		scan=strtok(NULL,"{\",\"}\n");
		strcpy(record.address,scan);

		if (HP_InsertEntry(info, record) < 0) 
		{
			printf("Error inserting entry in heap file\n");
			HP_CloseFile(info);
			exit(EXIT_FAILURE);
		}
	}
	printf("All records are inserted\n");
	
	int id=997;
	printf("Running HP_GetAllEntries for record with id %d\n",id);
	if(HP_GetAllEntries(*info,&id)<0)
	{
		printf("Record is not in the database\n");
	}

	printf("Deleting record with id %d \n",id);
	if(HP_DeleteEntry(*info,&id)<0)
	{
		printf("Item was not in the database\n");
	}

	printf("Running HP_GetAllEntries for record with id %d\n",id);
	if(HP_GetAllEntries(*info,&id)<0)
	{
		printf("Record is not in the database\n");
	}
	id=16000;
	printf("Running HP_GetAllEntries for record with id %d\n",id);
	if(HP_GetAllEntries(*info,&id)<0)
	{
		printf("Record is not in the database\n");
	}
	id=999;
	printf("Running HP_GetAllEntries for record with id %d\n",id);
	if(HP_GetAllEntries(*info,&id)<0)
	{
		printf("Record is not in the database\n");
	}
	printf("Do you want to print all records in database (Y/N) \n ");
	char answer;
	
	scanf(" %c", &answer);
	if(answer == 'Y' || answer=='y')
	{
		HP_GetAllEntries(*info,NULL);
	}


	printf("Closing heap file\n");
	if (HP_CloseFile(info) < 0) 
	{
		printf("Error closing id hash index.\n");
		fclose(file);
		exit(EXIT_FAILURE);
	}
	fclose(file);
	return EXIT_SUCCESS;
}
