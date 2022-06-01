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

    void printRecords(Record record);

    void readRecord(Record* Rec);
    void copyRecord(Record* target, Record* base);
#endif
