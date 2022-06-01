#ifndef __RECORD__
#define __RECORD__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

    typedef struct{
        int id;
        char name[15];
        char surname[25];
        char address[50];
    }Record;

    typedef struct{
        char surname[25];
        int block_id;
    }SecondaryRecord;

    void printRecords(Record record);
    void printSRecords(SecondaryRecord record);
    void readSRecord(SecondaryRecord* Rec);
    void readRecord(Record* Rec);
    void copyRecord(Record* target, Record* base);
    void copySRecord(SecondaryRecord* target, SecondaryRecord* base);
#endif
