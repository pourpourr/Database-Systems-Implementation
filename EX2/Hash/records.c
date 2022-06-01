#include "records.h"

void printRecords(Record record){
    printf("id =%d\n",record.id );
    printf("name= %s\n",record.name );
    printf("surname= %s\n",record.surname );
    printf("address= %s\n",record.address );

}
void printSRecords(SecondaryRecord record){
    printf("surname= %s\n",record.surname );
    printf("block_id  =%d\n",record.block_id );

}
static int i=0;
void readRecord(Record* Rec){

    printf("Give id\n" );
    scanf("%d",&(Rec->id)) ;
    printf("Give name\n" );
    scanf("%s",Rec->name );
    printf("Give surname\n" );
    scanf("%s",Rec->surname );
    printf("Give address\n" );
    scanf("%s",Rec->address );


}
void readSRecord(SecondaryRecord* Rec){


    printf("Give surname\n" );
    scanf("%s",Rec->surname );
    printf("Give block_id\n" );
    scanf("%d",&(Rec->block_id)) ;



}

void copyRecord(Record* target, Record* base){

    target->id=base->id;
    strcpy(target->surname,base->surname);
    strcpy(target->name,base->name);
    strcpy(target->address,base->address);



}
void copySRecord(SecondaryRecord* target, SecondaryRecord* base){

    target->block_id=base->block_id;
    strcpy(target->surname,base->surname);

}
