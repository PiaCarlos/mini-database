#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "database.h"

Database db_create() {
     
   Database db;
   db.capacity = 4;
   db.size = 0;

   db.records = malloc(db.capacity * sizeof(Record));

   return db;
}



void db_append(Database * db, Record const * item) {

   // if it's full, we need to change the resize the array
   if(db->size == db->capacity) {
       db->capacity = db->capacity * 2;
       db->records = realloc(db->records, db->capacity * sizeof(Record));
   }

   db->records[db->size] = *item; // add the item before changing the size
   db->size ++;

}

Record * db_index(Database * db, int index){

    // Check if the index is out of bounds, if it is, it gives an error
    if( index < 0 || index >= db->size) {
        fprintf(stderr, "Error: the index given by the user is out of bounds\n");
        return NULL;
    }

   return &(db->records[index]);
}

Record * db_lookup(Database * db, char const * handle) {
   
   int i = 0;

   // Check if there is a record with the given handle, if not , return null
   while(i < db->size){
       if(strcmp(db->records[i].handle, handle) == 0){
       return &(db->records[i]);
       }
       i++;
   }
   return NULL;

}

void db_free(Database * db) {

   free(db->records);
   // Make sure the pointers for this database don't cause undefined behavior
   db->capacity = 0;
   db->size = 0;
   db->records = NULL;

}


Record parse_record(char *line){

    Record r;
    char *value;

    // get the handle
    value = strtok(line, ",");
    strcpy(r.handle, value);
   
    // get the follower count
    value = strtok(NULL, ",");
    r.followers = strtoul(value, NULL, 10); // to convert the string to an unsigned long integer

    // get the comment
    value = strtok(NULL, ",");
    strcpy(r.comment, value);

    // get the date
    value = strtok(NULL, ",");
    r.timestamp = strtoul(value, NULL, 10);

    return r; 

}

void db_load_csv(Database *db, char const *path) {

   FILE *fl = fopen(path,"r");

   // Check if the file exist in the path
   if(fl == NULL){
      fprintf(stderr, "Error: the file doesn't exist");
      return;
   }

   size_t size = 0;
   char *line = NULL;
   size_t value;
   

   // if value is equal to -1, it means the while loop reached the end of the file
   while(value = getline(&line, &size , fl) != -1) {
       	   Record r = parse_record(line);
	   db_append(db, &r); // append the record to the database
        }
	     
   fclose(fl); // close the file after it's not needed anymore
   free(line);
}

void db_write_csv( Database *db, char const *path) {

   FILE *fl = fopen(path,"w");

   // Check if the file exist in the path
   if(fl == NULL){
      fprintf(stderr, "Error: the file already exist");
      return;
   }

   int i = 0;

   // Write the contents of the database in the csv format 
   while(i < db->size) {
   
       fprintf(fl, "%s,%lu,%s,%lu\n", db->records[i].handle, db->records[i].followers,
       db->records[i].comment, db->records[i].timestamp);
       i++;
   }

   fclose(fl);

}


