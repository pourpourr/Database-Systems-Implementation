#include "records.h"
#include "BF.h"
typedef struct {
    int fileDesc;
    char attrType;
    char* attrName;
    int attrLength;
    int printflag;
} HP_info;

int HP_CreateFile(char* fileName, char attrType, char* attrName, int attrLength);
HP_info*HP_OpenFile(char*Name);
int HP_GetAllEntries(HP_info, void *value) ;
int HP_CloseFile(HP_info* header_info);
int HP_InsertEntry (HP_info* header_info, Record record);
int HP_DeleteEntry(HP_info ,void *);
