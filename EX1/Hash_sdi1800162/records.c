#include "records.h"

void printRecords(Record record){
    printf("id =%d\n",record.id );
    printf("name= %s\n",record.name );
    printf("surname= %s\n",record.surname );
    printf("address= %s\n",record.address );

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

void copyRecord(Record* target, Record* base){

    target->id=base->id;
    strcpy(target->surname,base->surname);
    strcpy(target->name,base->name);
    strcpy(target->address,base->address);



}
