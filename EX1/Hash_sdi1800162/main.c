#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"
#include "hash.h"
#include "records.h"

#define FILENAME "osfp"
#define NUM_OF_BUCKETS 1000

int main(int argc, char** argv) {


    if(argc<2)
	{
		printf("A records file hasnt been given\n");
		return -1;
	}

	int i, j,k;
	char id[12];
    char key[13];
    char line[256];
    HT_info* header_info;
    HT_info* BackUp;
    Record* record=malloc(sizeof(Record));
    FILE* file = fopen(argv[1], "r");

    srand(time(NULL));
	BF_Init();

    printf("Creating heap file\n");
    if ( HT_CreateIndex( FILENAME, 'i', "id", sizeof(int),NUM_OF_BUCKETS) < 0 ){
		printf("Error creating heap file \n");
		exit(EXIT_FAILURE);
	}

    printf("Opening heap file\n");
    if ((header_info = HT_OpenIndex(FILENAME)) == NULL){
        printf("Error opening heap file\n");
        exit(EXIT_FAILURE);
    }
    BackUp=malloc(sizeof(HT_info));
    copyHeader(BackUp,header_info);

    printf("Inserting all records from the file \n");
    char* scan;

    while (fgets(line, sizeof(line), file)) {

        scan=strtok(line,"{\",\"}\n");
		record->id=atoi(scan);
		scan=strtok(NULL,"{\",\"}\n");
		strcpy(record->name,scan);
		scan=strtok(NULL,"{\",\"}\n");
		strcpy(record->surname,scan);
		scan=strtok(NULL,"{\",\"}\n");
		strcpy(record->address,scan);
        HT_InsertEntry(*BackUp, *record );
    }

    printf("All records are inserted\n");

    printf("Printing all records\n");
    Print_all(*BackUp);

    printf("give id to find and print\n" );
    scanf("%s",id );
    sprintf(key, "y%s", id);
    j=HT_GetAllEntries(*BackUp, (void*)key);
    printf("blocks read==%d\n", j);

    printf("give id to delete\n" );
    scanf("%s",key );
    HT_DeleteEntry(*BackUp, (void*)key);

    printf("Printing all records\n");
    Print_all(*BackUp);

    printf("Manually inserting record\n" );
    readRecord(record);
    printRecords(*record);
    HT_InsertEntry(*BackUp, *record ) ;


    printf("Printing all records\n");
    Print_all(*BackUp);

    printf("give id to find and print\n" );
    scanf("%s",id );
    sprintf(key, "y%s", id);
    j=HT_GetAllEntries(*BackUp, (void*)key);
    printf("blocks read==%d\n", j);

    printf("give id to delete\n" );
    scanf("%s",key );
    HT_DeleteEntry(*BackUp, (void*)key);

    printf("Printing all records\n");
    Print_all(*BackUp);

    printf("Closing heap file\n");
    if (HT_CloseIndex(BackUp ) < 0) {
        printf("Error closing id hash index.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    HashStatistics(FILENAME);

    fclose(file);
    free(BackUp);
    free(record);

    return EXIT_SUCCESS;
}
