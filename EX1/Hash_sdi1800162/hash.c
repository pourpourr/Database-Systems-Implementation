#include "hash.h"


int HT_CreateIndex( char *fileName, char atType, char* atName, int atLength, int buckets){

    int i,Desc;
    HT_info* header_info;
    blockNode* block;

    if (BF_CreateFile(fileName)<0){
        BF_PrintError("Error creating file (HT_CreateIndex)");
        return -1;
    }
    Desc= BF_OpenFile(fileName);
    if (Desc < 0) {
        BF_PrintError("Error opening file  (HT_CreateIndex)");
        return -2;
    }

    for(i=0; i<=buckets; i++ ){
        if(BF_AllocateBlock(Desc)<0){
            BF_PrintError("Error allocating block  (HT_CreateIndex)");
            return -3;
        }
    }

    if(BF_ReadBlock(Desc, 0 ,(void*)& header_info)<0){
        BF_PrintError("Error reading HT_info  (HT_CreateIndex)");
        return -4;
    }

    header_info->fileDesc=Desc;
    header_info->attrType=atType;
    strcpy(header_info->attrName,atName);
    header_info->attrLength=atLength;
    header_info->numBuckets=buckets;

    if(BF_WriteBlock(header_info->fileDesc , 0 )<0){
        BF_PrintError("Error writing HT_info  (HT_CreateIndex)");
        return -5;
    }

    for(i=0; i<buckets; i++ ){            // allocating blocks * buckets
        if(BF_ReadBlock(Desc, i+1 ,(void*)& block)<0){
            BF_PrintError("Error reading node  (HT_CreateIndex)");
            return -4;
        }
        block->numOfRecs=0;            // no records yet
        block->nextBlock=-1;            // no next block
        if(BF_WriteBlock(Desc , i+1 )<0){
            BF_PrintError("Error writing node  (HT_CreateIndex)");
            return -5;
        }
    }


    BF_CloseFile(Desc);
    return 0;
}

HT_info* HT_OpenIndex( char *fileName  ){
    int Desc= BF_OpenFile(fileName);

    if (Desc < 0) {
        BF_PrintError("Error opening file  (HT_OpenIndex)");
        return NULL;
    }
    HT_info* header_info;
    if(BF_ReadBlock(Desc, 0 ,(void*)& header_info)<0){
        BF_PrintError("Error reading node  (HT_OpenIndex)");
        return NULL;
    }
    return header_info;
}

int HT_CloseIndex( HT_info* header_info ){

    BF_CloseFile(header_info->fileDesc);
}

int HT_InsertEntry( HT_info header_info,  Record record ){

    char id[12];
    sprintf(id, "%d", record.id);  //making id into string
    char key[13];
    sprintf(key, "n%s", id);     //sending HT_GetAllEntries the message find id but not print

    if (HT_GetAllEntries( header_info,  (void*)key)>0){
        printf("Element with id %d already inside\n",record.id );
        return -1;
    }
    int hashValue= hash(id, header_info);

    blockNode* block;
    if(BF_ReadBlock(header_info.fileDesc, hashValue+1 ,(void*)& block)<0){
        BF_PrintError("Error reading node (HT_InsertEntry)");
        return -4;
    }
    int blockNumber=hashValue+1;
    while(block->numOfRecs==MAX_RECORDS){ //block is foul
        if(block->nextBlock!=-1){ //next block is already allocated
            blockNumber=block->nextBlock;
            if(BF_ReadBlock(header_info.fileDesc, block->nextBlock ,(void*)& block)<0){
                BF_PrintError("Error reading next node (HT_InsertEntry)");
                return -4;
            }
        }else{ // next block is empty

            if(BF_AllocateBlock(header_info.fileDesc)<0){
                BF_PrintError("Error allocating block  (HT_InsertEntry)");
                return -3;
            }
            block->nextBlock= BF_GetBlockCounter(header_info.fileDesc)-1; //setting current's block next
            if(BF_WriteBlock(header_info.fileDesc , blockNumber )<0){
                BF_PrintError("Error writing new next node  (HT_InsertEntry)");
                return -5;
            }
            blockNumber=block->nextBlock; // saving block's num
            if(BF_ReadBlock(header_info.fileDesc, block->nextBlock ,(void*)& block)<0){
                BF_PrintError("Error reading new next node (HT_InsertEntry)");
                return -4;
            }
            block->numOfRecs=0;  // setting new block
            block->nextBlock=-1;
            if(BF_WriteBlock(header_info.fileDesc , BF_GetBlockCounter(header_info.fileDesc)-1 )<0){
                BF_PrintError("Error writing new next node  (HT_InsertEntry)");
                return -5;
            }
        }// } of if nextBlock!=1
    }//} of while
    copyRecord(&(block->records[block->numOfRecs]),&record);
    block->numOfRecs++;
    if(BF_WriteBlock(header_info.fileDesc , blockNumber )<0){
        BF_PrintError("Error writing new next node  (HT_InsertEntry)");
        return -5;
    }
    return 0;
}

int Print_all(HT_info header_info){
    int i,j;
    blockNode* block;

    for(i=1; i<BF_GetBlockCounter(header_info.fileDesc); i++){
        if(BF_ReadBlock(header_info.fileDesc, i ,(void*)& block)<0){
            BF_PrintError("Error reading new next node (HT_InsertEntry)");
            return -4;
        }
        printf("block %d || num of recs =%d || nextnum= %d  \n",i,block->numOfRecs,block->nextBlock );
        for(j=0; j<block->numOfRecs; j++){
            printf("Block's %d record n%d\n",i,j );
            printRecords(block->records[j]);
        }
    }
    return 0;
}

int HT_DeleteEntry( HT_info header_info, void *value ){


        int id,key,i,flag=-1,next=-1;
        char* num= (char*)value;
        blockNode* block;
        HT_info* BackUp;

        id= atoi(num);
        key= hash(num, header_info);

        BackUp=malloc(sizeof(HT_info));
        copyHeader(BackUp,&header_info);

        if(BF_ReadBlock(BackUp->fileDesc, key+1 ,(void*)& block)<0){
            BF_PrintError("Error reading first node (HT_DeleteEntry)");
            return -4;
        }
        while(flag==-1){
            for(i=0; i<block->numOfRecs; i++){
                if (block->records[i].id==id){
                    flag=1;        // if record isn't last we move the last in its position and adjust numOfRecs
                    if (i!=block->numOfRecs -1) copyRecord(&(block->records[i]),&(block->records[block->numOfRecs -1]));
                    block->numOfRecs--;
                    break; //break from for
                }
            }
            if (flag!=-1) break; //break from while
            if(block->nextBlock!=-1){
                next=block->nextBlock;

                if(BF_ReadBlock(BackUp->fileDesc, next,(void*)& block)<0){
                    BF_PrintError("Error reading next node (HT_DeleteEntry)");
                    return -4;
                }
            }else{
                break;
            }
        }
        if(flag==1){
            if (next==-1){
                if(BF_WriteBlock(BackUp->fileDesc , key+1 )<0){  //rec was in bucket
                    BF_PrintError("Error writing new next node  (HT_DeleteEntry)");
                    return -5;
                }
            }else{
                if(BF_WriteBlock(BackUp->fileDesc , next )<0){ //rec was in overflow bucket
                    BF_PrintError("Error writing new next node  (HT_DeleteEntry)");
                    return -5;
                }
            }
            free(BackUp);
            return 0;
        }else
            free(BackUp);
            return -1;
}

int HT_GetAllEntries( HT_info header_info,  void *value ){

    if(value==NULL){
        Print_all(header_info);
        return  BF_GetBlockCounter(header_info.fileDesc)-1;
    }


    char* temp= (char*)value;
    char print=temp[0];  //if y we print else we do not
    int i,key,id,flag=-1, blocksRead=0 ,next;
    char num[12];

    HT_info* BackUp;
    blockNode* block;
    BackUp=malloc(sizeof(HT_info));
    copyHeader(BackUp,&header_info);

    for ( i = 0; i < strlen(temp); i++) {
        num[i]=temp[i+1];   // getting num from the agrument
    }
    id= atoi(num);
    key= hash(num, header_info);

    if(BF_ReadBlock(BackUp->fileDesc, key+1 ,(void*)& block)<0){
        BF_PrintError("Error reading first node (HT_GetAllEntries)");
        return -4;
    }
    blocksRead++;
    while(flag==-1){
        for(i=0; i<block->numOfRecs; i++){
            if (block->records[i].id==id){
                flag=1;
                if(print=='y'){
                    printRecords(block->records[i]);
                }
                break; //break from for
            }
        }
        if (flag!=-1) break; //break from while
        if(block->nextBlock!=-1){
            blocksRead++;
            next=block->nextBlock;
            if(BF_ReadBlock(BackUp->fileDesc, next,(void*)& block)<0){
                BF_PrintError("Error reading next node (HT_GetAllEntries)");
                return -4;
            }
        }else{
            break;
        }
    }
    free(BackUp);
    if(flag==1){
        return blocksRead;
    }else
        return -1;
}

int HashStatistics( char* filename  ){

    HT_info* header_info;
    HT_info* BackUp;
    header_info=HT_OpenIndex( filename);
    BackUp=malloc(sizeof(HT_info));
    copyHeader(BackUp,header_info);


    int total_blocks_counter=BF_GetBlockCounter(BackUp->fileDesc)-1;
    int recs_per_bucket,sumofBlocks=0, sumOfBuckOverflow=0;
    int minRec=999999,maxRec=-999999,sumOfRecs=0;
    double averageRecs,averageBlocksPerBuck;
    int i,j,next;
    int* blocks_per_bucket= malloc(sizeof(int)*BackUp->numBuckets);
    blockNode* block;


    for(i=0; i<BackUp->numBuckets; i++){
        recs_per_bucket=0;
        blocks_per_bucket[i]=1;
        if(BF_ReadBlock(BackUp->fileDesc, i+1 ,(void*)& block)<0){
            BF_PrintError("Error reading  node (HashStatistics)");
            return -1;
        }
        while(1){
            for(j=0; j<block->numOfRecs; j++)  recs_per_bucket++;
            if(block->nextBlock!=-1){
                blocks_per_bucket[i]+=1;
                next=block->nextBlock;
                if(BF_ReadBlock(BackUp->fileDesc, next,(void*)& block)<0){
                    BF_PrintError("Error reading next node (HashStatistics)");
                    return -1;
                }
            }else break;
        }
        sumOfRecs+=recs_per_bucket;
        sumofBlocks+=blocks_per_bucket[i];
        if(recs_per_bucket>maxRec){
            maxRec=recs_per_bucket;
        }
        if(recs_per_bucket<minRec){
            minRec=recs_per_bucket;
        }
    }
    averageRecs=(double)sumOfRecs/(double)(BackUp->numBuckets);
    averageBlocksPerBuck=(double)sumofBlocks/(double)(BackUp->numBuckets);
    printf("===========HASH STATISTICS===========\n" );
    printf("Sum of blocks in file =%d\n",total_blocks_counter );
    printf("Max num of records in a bucket = %d\n",maxRec );
    printf("min num of records in a bucket = %d\n",minRec );
    printf("average num of records in a bucket = %f\n",averageRecs);
    printf("average num of blocs per bucket = %f\n",averageBlocksPerBuck);
    sumOfBuckOverflow=0;
    for ( i = 0; i < BackUp->numBuckets; i++) {
        if(blocks_per_bucket[i]>1){
            sumOfBuckOverflow++;
            printf("Bucket %d has %d overflow blocks\n",i+1,blocks_per_bucket[i]-1  );
        }
    }
    printf("Sum of buckets with overflow buckets =%d\n",sumOfBuckOverflow );

    HT_CloseIndex(BackUp );
    free(BackUp);
    free(blocks_per_bucket);
    return 0;
}

int hash(unsigned char *str , HT_info header_info){
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return (hash%(header_info.numBuckets));
}

void copyHeader(HT_info* target, HT_info* base){

    int i;
    target->fileDesc=base->fileDesc;
    target->attrType=base->attrType;
    strcpy(target->attrName,base->attrName);
    target->attrLength=base->attrLength;
    target->numBuckets=base->numBuckets;

}
