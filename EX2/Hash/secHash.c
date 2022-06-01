#include "secHash.h"

int SHT_CreateSecondaryIndex( char *sfileName,char* attrName, int attrLength,int buckets,char* fileName){
    int i,Desc;
    SHT_info* sec_header_info;
    sec_blockNode* sec_block;

    if (BF_CreateFile(sfileName)<0){
        BF_PrintError("Error creating file (SHT_CreateIndex)");
        return -1;
    }
    Desc= BF_OpenFile(sfileName);
    if (Desc < 0) {
        BF_PrintError("Error opening file  (SHT_CreateIndex)");
        return -2;
    }

    for(i=0; i<=buckets; i++ ){
        if(BF_AllocateBlock(Desc)<0){
            BF_PrintError("Error allocating block  (SHT_CreateIndex)");
            return -3;
        }
    }

    if(BF_ReadBlock(Desc, 0 ,(void*)& sec_header_info)<0){
        BF_PrintError("Error reading SHT_info  (SHT_CreateIndex)");
        return -4;
    }

    sec_header_info->fileDesc=Desc;
    strcpy(sec_header_info->attrName,attrName);
    sec_header_info->attrLength=attrLength;
    sec_header_info->numBuckets=buckets;
    strcpy(sec_header_info->fileName,fileName);

    if(BF_WriteBlock(sec_header_info->fileDesc , 0 )<0){
        BF_PrintError("Error writing SHT_info  (SHT_CreateIndex)");
        return -5;
    }


    for(i=0; i<buckets; i++ ){            // allocating blocks * buckets
        if(BF_ReadBlock(Desc, i+1 ,(void*)& sec_block)<0){
            BF_PrintError("Error reading node  (SHT_CreateIndex)");
            return -4;
        }
        sec_block->numOfRecs=0;            // no records yet
        sec_block->nextBlock=-1;            // no next block
        if(BF_WriteBlock(Desc , i+1 )<0){
            BF_PrintError("Error writing node  (SHT_CreateIndex)");
            return -5;
        }
    }

    BF_CloseFile(Desc);
    return 0;
}

SHT_info* SHT_OpenSecondaryIndex( char *sfileName ){

    int Desc= BF_OpenFile(sfileName);

    if (Desc < 0) {
        BF_PrintError("Error opening file  (SHT_OpenIndex)");
        return NULL;
    }
    SHT_info* sec_header_info;
    if(BF_ReadBlock(Desc, 0 ,(void*)& sec_header_info)<0){
        BF_PrintError("Error reading node  (SHT_OpenIndex)");
        return NULL;
    }
    return sec_header_info;
}

int SHT_CloseIndex( SHT_info* sec_header_info ){

    BF_CloseFile(sec_header_info->fileDesc);
    return 0;
}

int SHT_SecondaryInsertEntry(  SHT_info sec_header_info,  SecondaryRecord sec_record ){

    if(sec_record.block_id==-1){
        printf("surname not in first hash, cannot insert\n" );
        return -1;
    }

    HT_info* header_info=malloc(sizeof(HT_info));
    header_info->fileDesc=-9999; //checking if the node (surname, block_id) already exists in secondary heap
    if (SHT_SecondaryGetAllEntries( sec_header_info,*header_info,  (void*)&sec_record)>0){
        printf("Element with same surname and block_id already inside\n" );
        free(header_info);
        return -1;
    }
    free(header_info);
    int hashValue= sec_hash(sec_record.surname, sec_header_info);

    sec_blockNode* block;
    if(BF_ReadBlock(sec_header_info.fileDesc, hashValue+1 ,(void*)& block)<0){
        BF_PrintError("Error reading node (SHT_InsertEntry)");
        return -4;
    }
    int blockNumber=hashValue+1;
    while(block->numOfRecs==MAX_RECORDS2){ //block is foul
        if(block->nextBlock!=-1){ //next block is already allocated
            blockNumber=block->nextBlock;
            if(BF_ReadBlock(sec_header_info.fileDesc, block->nextBlock ,(void*)& block)<0){
                BF_PrintError("Error reading next node (SHT_InsertEntry)");
                return -4;
            }
        }else{ // next block is empty

            if(BF_AllocateBlock(sec_header_info.fileDesc)<0){
                BF_PrintError("Error allocating block  (SHT_InsertEntry)");
                return -3;
            }
            block->nextBlock= BF_GetBlockCounter(sec_header_info.fileDesc)-1; //setting current's block next
            if(BF_WriteBlock(sec_header_info.fileDesc , blockNumber )<0){
                BF_PrintError("Error writing new next node  (SHT_InsertEntry)");
                return -5;
            }
            blockNumber=block->nextBlock; // saving block's num
            if(BF_ReadBlock(sec_header_info.fileDesc, block->nextBlock ,(void*)& block)<0){
                BF_PrintError("Error reading new next node (SHT_InsertEntry)");
                return -4;
            }
            block->numOfRecs=0;  // setting new block
            block->nextBlock=-1;
            if(BF_WriteBlock(sec_header_info.fileDesc , BF_GetBlockCounter(sec_header_info.fileDesc)-1 )<0){
                BF_PrintError("Error writing new next node  (SHT_InsertEntry)");
                return -5;
            }
        }// } of if nextBlock!=1
    }//} of while
    strcpy((block->records[block->numOfRecs]).surname,sec_record.surname);
    (block->records[block->numOfRecs]).block_id=sec_record.block_id;
    block->numOfRecs++;
    if(BF_WriteBlock(sec_header_info.fileDesc , blockNumber )<0){
        BF_PrintError("Error writing new next node  (SHT_InsertEntry)");
        return -5;
    }
    return blockNumber;
}


int SPrint_all(SHT_info sec_header_info){
    int i,j;
    sec_blockNode* block;

    for(i=1; i<BF_GetBlockCounter(sec_header_info.fileDesc); i++){
        if(BF_ReadBlock(sec_header_info.fileDesc, i ,(void*)& block)<0){
            BF_PrintError("Error reading new next node (SPrint_all)");
            return -4;
        }
        printf("block %d || num of recs =%d || nextnum= %d  \n",i,block->numOfRecs,block->nextBlock );
        for(j=0; j<block->numOfRecs; j++){
            printf("Block's %d record n%d\n",i,j );
            printSRecords(block->records[j]);
        }
    }
    return 0;
}

int SHT_SecondaryGetAllEntries(SHT_info sec_header_info,HT_info header_info,  void *value){
    if(value==NULL){
        printf("no value given to SHT_SecondaryGetAllEntries\n" );
        return  -1;
    }

    char* surname= malloc(25*sizeof(char));
    int i,j,key,flag=-1, blocksRead=0 ,next,action,found=0;
    HT_info* BackUp;
    blockNode* block;
    SecondaryRecord* sec_record;

    if (header_info.fileDesc!=-9999){
        action=1;
        BackUp=malloc(sizeof(HT_info));
        copyHeader(BackUp,&header_info);
        strcpy(surname,(char*) value);
    }else{
        action=-1;
        sec_record=malloc(sizeof(SecondaryRecord));
        copySRecord(sec_record,(SecondaryRecord*)value);
        strcpy(surname,sec_record->surname);

    }
    SHT_info* BackUp2;
    sec_blockNode* sec_block;
    BackUp2=malloc(sizeof(SHT_info));
    copySHeader(BackUp2,&sec_header_info);


    key= sec_hash(surname, sec_header_info);


    if(BF_ReadBlock(BackUp2->fileDesc, key+1 ,(void*)& sec_block)<0){
        BF_PrintError("Error reading first first node (SHT_SecondaryGetAllEntries)");
        return -4;
    }
    blocksRead++;
    while(flag==-1){
        if(action==1){
            for(i=0; i<sec_block->numOfRecs; i++){

                if (strcmp(sec_block->records[i].surname,surname)==0){
                    found=1;
                    if(BF_ReadBlock(BackUp->fileDesc, sec_block->records[i].block_id ,(void*)& block)<0){
                        BF_PrintError("Error reading first node (SHT_SecondaryGetAllEntries)");
                        return -4;
                    }
                    for(j=0;j<block->numOfRecs; j++){
                        if (strcmp(block->records[j].surname,surname)==0){
                            printRecords(block->records[j]);
                        }
                    }

                }
            }
        }else{
            for(i=0; i<sec_block->numOfRecs; i++){
                if (strcmp(sec_block->records[i].surname,sec_record->surname)==0 && sec_block->records[i].block_id==sec_record->block_id ){
                    flag=1;
                    break;
                }//if
            }//for
        }//else
        if (flag!=-1) break; //break from while
        if(sec_block->nextBlock!=-1){
            blocksRead++;
            next=sec_block->nextBlock;
            if(BF_ReadBlock(BackUp2->fileDesc, next,(void*)& sec_block)<0){
                BF_PrintError("Error reading next node (HT_GetAllEntries)");
                return -4;
            }
        }else{
            break;
        }
    }
    free(BackUp2);
    free(surname);
    if(action==1){
        free(BackUp);
    }else{
        free(sec_record);
    }
    if(flag==1 || found==1){
        return blocksRead;
    }else{
        return -1;
    }

}


int SecondaryHashStatistics( char* filename  ){


        SHT_info* header_info;
        SHT_info* BackUp;
        header_info=SHT_OpenSecondaryIndex( filename);
        BackUp=malloc(sizeof(SHT_info));
        copySHeader(BackUp,header_info);

        int total_blocks_counter=BF_GetBlockCounter(BackUp->fileDesc)-1;
        int recs_per_bucket,sumofBlocks=0, sumOfBuckOverflow=0;
        int minRec=999999,maxRec=-999999,sumOfRecs=0;
        double averageRecs,averageBlocksPerBuck;
        int i,j,next;
        int* blocks_per_bucket= malloc(sizeof(int)*BackUp->numBuckets);
        sec_blockNode* block;


        for(i=0; i<BackUp->numBuckets; i++){
            recs_per_bucket=0;
            blocks_per_bucket[i]=1;
            if(BF_ReadBlock(BackUp->fileDesc, i+1 ,(void*)& block)<0){
                BF_PrintError("Error reading  node (SHashStatistics)");
                return -1;
            }
            while(1){
                for(j=0; j<block->numOfRecs; j++)  recs_per_bucket++;
                if(block->nextBlock!=-1){
                    blocks_per_bucket[i]+=1;
                    next=block->nextBlock;
                    if(BF_ReadBlock(BackUp->fileDesc, next,(void*)& block)<0){
                        BF_PrintError("Error reading next node (SHashStatistics)");
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

        SHT_CloseIndex(BackUp );
        free(BackUp);
        free(blocks_per_bucket);
        return 0;

}

void copySHeader(SHT_info* target, SHT_info* base){

    target->fileDesc=base->fileDesc;
    strcpy(target->attrName,base->attrName);
    strcpy(target->fileName,base->fileName);
    target->attrLength=base->attrLength;
    target->numBuckets=base->numBuckets;

}

int sec_hash(unsigned char *str , SHT_info sec_header_info){
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;
    return (hash%(sec_header_info.numBuckets));
}
