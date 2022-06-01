#ifndef __HASH__
#define __HASH__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "records.h"
#include "BF.h"
#define MAX_RECORDS 5  // max recs for block size =512

    typedef struct {
        int fileDesc;
        char attrType;
        char attrName[20];
        int attrLength;
        long int numBuckets;
    }HT_info;

    typedef struct {
        int numOfRecs;
        Record records[MAX_RECORDS];
        int nextBlock;

    }blockNode;




    int HT_CreateIndex( char *filename, char atType, char* atName, int atLength, int buckets);

    HT_info* HT_OpenIndex( char *fileName  );

    int HT_CloseIndex( HT_info* header_info );

    int HT_InsertEntry( HT_info header_info,  Record record );

    int HT_DeleteEntry( HT_info header_info, void *value );

    int HT_GetAllEntries( HT_info header_info,  void *value );

    int HashStatistics( char* filename  );

    int hash(unsigned char *str , HT_info header_info);

    int Print_all(HT_info header_info);

    void copyHeader(HT_info* target, HT_info* base);




#endif
