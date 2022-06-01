#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"

/* sdi1800065 Karakolis  Ioannis */

int HP_CreateFile(char *fileName, char attrType,char* attrName, int attrLength)
{
    HP_info info;
    void* headblock;
    int i;
    void*block;
    int counter=0;

    BF_Init();

    if (BF_CreateFile(fileName) < 0)
    {
        BF_PrintError("Error creating file in HP_CreateFile ");
        return -1;
    }
    if ((info.fileDesc = BF_OpenFile(fileName)) < 0)
    {
        BF_PrintError("Error opening file in HP_CreateFile ");
        return -1;
    }

    info.attrLength = attrLength;
    info.attrName = attrName;
    info.attrType = attrType;
    info.printflag=0;

    if (BF_AllocateBlock(info.fileDesc) < 0)
    {
        BF_PrintError("Error allocating block in HP_CreateFile ");
        return -1;
    }

    if (BF_ReadBlock(info.fileDesc, 0, &headblock) < 0)
    {
        BF_PrintError("Error reading block in HP_CreateFile ");
        return -1;
    }
    			
    memcpy(headblock, &info, sizeof(HP_info)+sizeof(attrName));

    if (BF_WriteBlock(info.fileDesc, 0) < 0)
    {
        BF_PrintError("Error writing block in HP_CreateFile ");
        return -1;
    }


    if (BF_AllocateBlock(info.fileDesc) < 0)
    {
        BF_PrintError("Error allocating block in HP_CreateFile ");
        return -1;
    }
    
    if (BF_ReadBlock(info.fileDesc, 1, &block) < 0)
    {
        BF_PrintError("Error reading block in HP_CreateFile ");
        return -1;
    }
    
    memcpy(block,&counter, sizeof(int));

    if (BF_WriteBlock(info.fileDesc, 1) < 0)
    {
        BF_PrintError("Error writing block in HP_CreateFile ");
        return -1;
    }

     if (BF_CloseFile(info.fileDesc) < 0)
    {
        BF_PrintError("Error closing file in HP_CreateFile ");
        return -1;
    }
}

HP_info*HP_OpenFile(char*Name)
{
    void *block;
    int fileDesc, blockNumber;
    BF_Init();
    if ((fileDesc = BF_OpenFile(Name)) < 0) 
    {

        BF_PrintError("Error opening file in HP_OpenFile");
        return NULL;
    }
    
    if (BF_ReadBlock(fileDesc, 0, &block) < 0) {
        BF_PrintError("Error reading block in HP_OpenFile");
        return NULL;
    }
    return block;
}

int HP_CloseFile(HP_info* header_info)
{
    if (BF_CloseFile(header_info->fileDesc) == 0)
    {
        return 0;
    }
    
    BF_PrintError("Error closing file in HP_CloseFile ");

    return -1;
}

int HP_InsertEntry(HP_info* info, Record record) 
{
    void *block;
    int blockNumber;
    
    if ((blockNumber = BF_GetBlockCounter(info->fileDesc)) < 0) {
        BF_PrintError("Error getting block counter in HP_InsertEntry");
        return -1;
    }
    //Use the code bellow to stop the funciton from inserting already inserted records in the heap file.
 /*   HP_info doublescheck=*info;
    doublescheck.printflag=1;

        if(HP_GetAllEntries(doublescheck,&record)>0)
        {
            printf("Aleady inserted a record with this id\n");
            return blockNumber;
        }
*/
   if(BF_ReadBlock(info->fileDesc,blockNumber-1,&block)<0)
    {
        BF_PrintError("Error reading block counter in HP_InsertEntry");
        return -1;
    }

    int k=sizeof(Record)*(*(int*)(block))+sizeof(int); //The space where the new record must be inserted in the block if is has the needed space left.

    if(k<=(BLOCK_SIZE-sizeof(Record)))
    {
        memcpy(block+k,&record,sizeof(Record));
        ++(*(int*)block); //recordcounter=recordcounter+1 

        if(BF_WriteBlock(info->fileDesc,blockNumber-1)<0)
        {
            BF_PrintError("Error writing block counter in HP_InsertEntry");
            return -1;
        }
    }
    else //After that a new block is being allocated to insert the record couse there is no space left in the already allocated records
    {   
        if(BF_ReadBlock(info->fileDesc,0,&block)<0){//Solves the problem of deleteting header block the from ram after 20 loaded blocks by loading the first one so it will never be the longest to be used and wont get deleted from the ram
               BF_PrintError("Error reading block counter in HP_InsertEntry");
               return -1;
            }
    
        int counter=1;

        if (BF_AllocateBlock(info->fileDesc) < 0)
        {
           BF_PrintError("ERROR allocating block in HP_InsertEntry ");
           return -1;
        }

        if (BF_ReadBlock(info->fileDesc, blockNumber, &block) < 0)
        {
            BF_PrintError("Error reading block counter in HP_InsertEntry ");
            return -1;
        }
    
        memcpy(block,&counter, sizeof(int));
        memcpy(block+sizeof(int),&record,sizeof(Record));

        if (BF_WriteBlock(info->fileDesc, blockNumber) < 0)
        {
            BF_PrintError("ERROR writing block in HP_InsertEntry ");
            return -1;
        }
     
    }
    return blockNumber;
}

int HP_DeleteEntry(HP_info header_info ,void *value)
{
    header_info.printflag=1;
    int blocknumber=0;
    void *block;
    Record record;
    int i=1,k,index;

    if((blocknumber=HP_GetAllEntries(header_info,value))<0) //Gets the block on witch the record we want to delete is at.
    {
        return -1;
    }

    if(BF_ReadBlock(header_info.fileDesc,blocknumber,&block)<0)
    {
        BF_PrintError("ERROR reading block in HP_DeleteEntry ");
        return -1;
    }


    for(k=sizeof(int);k<sizeof(Record)*((*(int*)(block)));k=k+sizeof(Record))
    {
        record=*(Record*)(block+k); //Gets the pointer for each record of the block 

        if(record.id== *(int*)value)
        {

                (*(Record*)(block+k))=*(Record*)(block+sizeof(int)+sizeof(record)*((*(int*)block)-1)); // Replaces the record we want to delete with last record
                --(*(int*)block);//recordcounter=recordcounter-1 ,Removes last record

                if(BF_WriteBlock(header_info.fileDesc,blocknumber)<0)
                    {
                        BF_PrintError("ERROR reading block in HP_DeleteEntry ");
                        return -1;
                    }
                return 0;
        }
    }

}

int HP_GetAllEntries(HP_info info,void *value)
{
    
    void *block;
    int numOfBlocks,n,k,flag=0;
    Record record;
    
    if ((numOfBlocks = BF_GetBlockCounter(info.fileDesc)) < 0)
    {
        BF_PrintError("Error getting block counter in HP_GetAllEntries");
        return-1;
    }

    for (n = 1; n < numOfBlocks; n++) 
    {

        if (BF_ReadBlock(info.fileDesc, n, &block) < 0) 
        {
            BF_PrintError("Error getting block in HP_GetAllEntries");
            return -1;
        }

        for(k=sizeof(int);k<sizeof(Record)*((*(int*)(block)));k=k+sizeof(Record))
        {
           
            record=*(Record*)(block+k);

            if(value!=NULL)
            {
                if(record.id== *(int*)value)
                {
                    if(info.printflag==0)
                        printf("%d,\n%s,\n%s,\n%s\n\n", record.id, record.name, record.surname, record.address);
                    return n;
                }
            }
            else
            {
                printf("%d,\n%s,\n%s,\n%s\n\n", record.id, record.name, record.surname, record.address);
                flag=1;   
            }           
        }
        
        if(BF_ReadBlock(info.fileDesc,0,&block)<0) //Solves the problem of deleteting header block the from ram after 20 loaded blocks by loading the first one so it will never be the longest to be used and wont get deleted from the ram
        {
            BF_PrintError("Error getting block in HP_GetAllEntries");
            return -1;
        }
   }
   if(flag==1)
       return numOfBlocks;
   return -2;
}
