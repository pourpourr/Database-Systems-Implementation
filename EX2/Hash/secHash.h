#ifndef __SEC_HASH__
#define __SEC_HASH__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "records.h"
#include "hash.h"
#include "BF.h"
#define MAX_RECORDS2 17//(int)((float)(512-2*sizeof(int))/(float)(25*sizeof(char)+sizeof(int)))


typedef struct {
    int fileDesc;
    char attrName[20];
    int attrLength;
    long int numBuckets;
    char fileName[5];
} SHT_info;

typedef struct {
    int numOfRecs;
    SecondaryRecord records[MAX_RECORDS2];
    int nextBlock;

}sec_blockNode;

int SHT_CreateSecondaryIndex( char *sfileName,char* attrName, int attrLength,int buckets,char* fileName);

SHT_info* SHT_OpenSecondaryIndex( char *sfileName );

int SHT_CloseIndex( SHT_info* sec_header_info );

int SHT_SecondaryInsertEntry(  SHT_info sec_header_info,  SecondaryRecord sec_record );

int SHT_SecondaryGetAllEntries(SHT_info sec_header_info,HT_info header_info,  void *value);

int SecondaryHashStatistics( char* filename  );

int SPrint_all(SHT_info sec_header_info);

int sec_hash(unsigned char *str , SHT_info sec_header_info);

void copySHeader(SHT_info* target, SHT_info* base);

#endif
