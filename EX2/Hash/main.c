#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"
#include "hash.h"
#include "secHash.h"
#include "records.h"

#define FILENAME "osfp"
#define FILENAME2 "pao"
#define NUM_OF_BUCKETS 1000

int main(int argc, char** argv) {


    if(argc<2)
	{
		printf("A records file hasnt been given\n");
		return -1;
	}

	int i, j,k;
	char id[12];
    char key[25];
    char key2[13];
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

    SHT_CreateSecondaryIndex(FILENAME2,"surname",20,200,FILENAME);
    SHT_info* sec_header_info;
    sec_header_info=SHT_OpenSecondaryIndex( FILENAME2 );
    SHT_info* BackUp2=malloc(sizeof(SHT_info));
    copySHeader(BackUp2,sec_header_info);
    SecondaryRecord* rec=malloc(sizeof(SecondaryRecord));
    i=0;
    while (fgets(line, sizeof(line), file)) {
        scan=strtok(line,"{\",\"}\n");
		record->id=atoi(scan);
		scan=strtok(NULL,"{\",\"}\n");
		strcpy(record->name,scan);
		scan=strtok(NULL,"{\",\"}\n");
        strcpy(record->surname,scan);
        strcpy(rec->surname,scan);
		scan=strtok(NULL,"{\",\"}\n");
		strcpy(record->address,scan);
        rec->block_id=HT_InsertEntry(*BackUp, *record );
        SHT_SecondaryInsertEntry(*BackUp2,*rec);

    }

    printf("All records are inserted\n");

    printf("give id to find and print\n" );
    scanf("%s",id );
    sprintf(key2, "y%s", id);
    j=HT_GetAllEntries(*BackUp, (void*)key2);
    printf("blocks read==%d\n", j);

    printf("give surname to find and print\n" );
    scanf("%s",key );
    j=SHT_SecondaryGetAllEntries(*BackUp2,*BackUp, (void*)key);
    printf("Manually inserting record\n" );
    readRecord(record);
    strcpy(rec->surname,record->surname);
    rec->block_id=HT_InsertEntry(*BackUp, *record );
    j=SHT_SecondaryInsertEntry(*BackUp2,*rec);
    printf("give surname to find and print\n" );
    scanf("%s",key );
    j=SHT_SecondaryGetAllEntries(*BackUp2,*BackUp, (void*)key);printf("Manually inserting record\n" );
    readRecord(record);
    strcpy(rec->surname,record->surname);
    rec->block_id=HT_InsertEntry(*BackUp, *record );
    j=SHT_SecondaryInsertEntry(*BackUp2,*rec);

    printf("give id to find and print\n" );
    scanf("%s",id );
    sprintf(key2, "y%s", id);
    j=HT_GetAllEntries(*BackUp, (void*)key2);
    printf("blocks read==%d\n", j);

    printf("give surname to find and print\n" );
    scanf("%s",key );
    j=SHT_SecondaryGetAllEntries(*BackUp2,*BackUp, (void*)key);
    printf("===========================================\n" );

    SecondaryHashStatistics(FILENAME2);
    printf("Closing heap file\n");
    if (HT_CloseIndex(BackUp ) < 0) {
        printf("Error closing id hash index.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    printf("Closing sec heap file\n");
    if (SHT_CloseIndex(BackUp2 ) < 0) {
        printf("Error closing surname hash index.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }


    free(BackUp);
    free(BackUp2);
    free(rec);
    fclose(file);
    free(record);
    return EXIT_SUCCESS;
}
