#ifndef DB_H
#define DB_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Record { 

   char handle[32];
   long unsigned int followers;
   char comment[64];
   long unsigned int timestamp;

} Record;

typedef struct Database { 

   int size;
   int capacity;
   Record *records;

} Database;

Database db_create();

char* timestampString(time_t timestamp);

void print_record(Record const *r);

void print_database(Database const *db);

void db_append(Database * db, Record const * item);

Record * db_index(Database * db, int index);

Record * db_lookup(Database * db, char const * handle);

void db_free(Database * db);

void db_load_csv(Database * db, char const * path);

void db_write_csv(Database * db, char const * path);


#endif
