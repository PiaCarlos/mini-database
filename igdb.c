#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "database.h"


static long parseLong(const char * string)
{
    long value = -1;
    char * endPoint;
    value = strtol(string, &endPoint, 0);

    // if this is true, it means it wasn't correctly converted to a interger
    if(*endPoint != '\0') { 	
       value = -1;
    }

    return value;
}


char* timestampString(time_t timestamp) {
    
    char *correctTime = malloc(100 * sizeof(char));
    struct tm *montrealTime; // make use of the tm struc
    
    // convert timestamp to EST time
    montrealTime = localtime(&timestamp);

    // convert the EST time to the correct format in a string
    strftime(correctTime, 100, "%Y-%m-%d %H:%M:%S", montrealTime);
			  
    return correctTime;
}

void print_record(Record const *r) {

   printf("| %-20.20s | %-10lu | %-19s | %-30.30s \n",
          r -> handle, r -> followers, timestampString(r -> timestamp),r -> comment);
}

void print_database(Database const *db) {

   int i = 0;

   while(i < db -> size){
       print_record(&db->records[i]);
       i++;
   }
}


int validate_NumArguments(const char *handle, const char *followers, const char *thirdArgument) { 

     // Check if the user gave an handle
     if (handle == NULL) {
          printf("Error: the prompt needs an argument for handle\n");
          return 1;
     }

     // Check if the user gave the number of followers
     if (followers == NULL) {
          printf("Error: the prompt needs an argument for the number of followers\n");
          return 1;
     }

     // Check if the user gave more than two arguments
     if (thirdArgument != NULL) {
          printf("Error: the prompt needs exactly 2 arguments\n");
          return 1;
     }

     return 0;

}


int main_loop(Database * db){
   
   size_t size = 0;
   char *line = NULL;
   int changesNotSaved = 0; 


   while(1) {
        printf("> "); // so the user knows he stil can use commands 
        
	// handle crtl+d option
        if(getline(&line, &size , stdin) == -1) {
        break; //leave the loop
        }
	
	// get the operation and remove \n
	char * operation = strtok(line, " ");
	operation[strcspn(operation, "\n")] = 0;  

	
	// the user uses the prompt list
	if (strcmp(operation , "list") == 0) {
            
            char *argument = strtok(NULL, " "); 	    
            operation[strcspn(operation, "\n")] = 0;

            if (argument != NULL) {
                printf("Error: The list prompt does not have any arguments\n");
                continue; 
            }
            
	    printf(" HANDLE 	       | FOLLOWERS  | LAST MODIFIED       | COMMENT\n");
	    printf("- - - - - - - - - - - -| - - - - - -| - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	    print_database(db); // print the records 
				
	}

	// the user uses the prompt add
	else if (strcmp(operation , "add") == 0) {
            
            char *handle = strtok(NULL, " ");
            char *followers = strtok(NULL, " ");
            char *thirdArgument = strtok(NULL, " ");
	    
	    if (validate_NumArguments(handle, followers, thirdArgument)){
	        continue;
	    }
            
	    followers[strcspn(followers, "\n")] = 0;
	    long followersInt = parseLong(followers);
	     

             // Check if number of followers is an interger
            if(followersInt == -1){
                printf("Error: the argument for number of followers must be an interger\n");
                continue;
            }

	    // Check if the handle already exist in the database
            if (db_lookup(db, handle) != NULL) {
                printf("Error: the handle '%s' already exists in this database.\n", handle);
                continue;
            }
	
            printf("Comment > ");

           // get and check if the user has given an comment
           char *comment = NULL;
           size_t size = 0;
           if(getline(&comment, &size, stdin) == -1){
              printf("Error: nothing written for comment");
              continue;
           }

          comment[strcspn(comment, "\n")] = 0; 
          
          // create and add the new record
          Record recordAdd = {
	  .followers = followersInt,  
          .timestamp = time(NULL)
           };

          strcpy(recordAdd.handle, handle); 
	  strcpy(recordAdd.comment, comment);

	  changesNotSaved = 1;
          db_append(db, &recordAdd);	  
          

        }

	// the user uses the prompt update
	else if (strcmp(operation , "update") == 0) {

	   char *handle = strtok(NULL, " ");
           char *followers = strtok(NULL, " ");
           char *thirdArgument = strtok(NULL, " ");

           if (validate_NumArguments(handle, followers, thirdArgument)){
               continue;
           }

           followers[strcspn(followers, "\n")] = 0;
           long followersInt = parseLong(followers);

           // Check if number of followers is an interger
           if(followersInt == -1){
               printf("Error: the argument for number of followers must be an interger\n");
               continue;
           }

	   Record * recordSelected = db_lookup(db, handle);

           // Check if the handle don't exist in the database
           if (recordSelected == NULL) {
               printf("Error: the handle '%s' doesn't exist in this database, so it can't be updated.\n", handle);
               continue;
           }

	   printf("Comment > ");

           // get and check if the user has given an comment
           char *comment = NULL;
           size_t size = 0;
           if(getline(&comment, &size, stdin) == -1){
              printf("Error: nothing written for comment");
              continue;
           }

          // updating the handle with the new number of followers, comment and date
	   comment[strcspn(comment, "\n")] = 0;
	   recordSelected->followers = followersInt;
           strcpy(recordSelected->comment, comment);
	   recordSelected->timestamp = time(NULL);

	   changesNotSaved = 1;

        }

	// the user uses the prompt save
	else if (strcmp(operation , "save") == 0) {
	    db_write_csv(db, "database.csv");
	    changesNotSaved = 0;
        }

	// the user uses the prompt exit
	else if (strcmp(operation , "exit") == 0) {
	
	     char *force = strtok(NULL, " ");
	     if( force != NULL) {
		force[strcspn(force, "\n")] = 0;
	     	if(strcmp(force , "fr") == 0){
	  	      break;	     
	        }
	     }

	     if(changesNotSaved) {
                printf("you did not save your changes . Use `exit fr ` to force exiting anyway.\n");
	        continue;
	     }

	     break; 

        }
	// the user uses an invalid prompt
	else { 
	     printf("Error: the prompt %s is not supported\n", operation);
	}
   }
   db_free(db);
   free(line);
   return 0;

}

int main()
{
   Database db = db_create();
   db_load_csv(&db, "database.csv");
   return main_loop(&db);
   
    //testing part 1 and 2

    /* 	
    Database db = db_create();

    Record r = (Record) {
    .handle = "@spottedmcgill",
    .comment = "a bit cringe tbh",
    .followers = 14900,
    .timestamp = time(NULL),
     };

     Record r2 = (Record) {
    .handle = "@spottedmcgill",
    .comment = "a cringe tbh",
    .followers = 16900,
    .timestamp = time(NULL),
     };

     Record r3 = (Record) {
    .handle = "@spottedmcgill",
    .comment = "cringe tbh",
    .followers = 189900,
    .timestamp = time(NULL),
     };

     Record r4 = (Record) {
    .handle = "@spottedmcgill",
    .comment = "a bit cringe",
    .followers = 14999900,
    .timestamp = time(NULL),
     };

     Record r5 = (Record) {
    .handle = "@pia",
    .comment = "a bit cringe tbh",
    .followers = 14900,
    .timestamp = time(NULL),
     };

    db_append(&db, &r);
    db_append(&db, &r2);
    db_append(&db, &r3);
    db_append(&db, &r4);

    // testing print
    printf("Database after adding one record:\n");
    print_database(&db); 

    db_append(&db, &r5);

    // testing print
    printf("Database after adding one record:\n");
    print_database(&db);

    printf("Size of the database: %u\n", db.size);
    
    printf("testing other things \n \n \n \n");

     Record *record = db_index(&db, 2);
    if (record != NULL) {
        printf("Record index 2: Handle=%s, Followers=%lu\n", record->handle, record->followers);
    } 

    // Test db_lookup
    Record *lookup_record = db_lookup(&db, "@pia");
    if (lookup_record != NULL) {
        printf("Found record for handle1: Followers=%lu\n", lookup_record->followers);
    }
    printf("testing part 2 \n \n \n \n");


    Database db2;
    db_load_csv(&db2, "database.csv");
    print_database(&db2);

    Record new_record = {
        .handle = "@newuser",
        .comment = "This is a new user",
        .followers = 1000,
        .timestamp = time(NULL)
    };

    db_append(&db2, &new_record);
    print_database(&db2);

    db_write_csv(&db2, "modified_database.csv");


    

    // Free the memory used by the database
    db_free(&db);
    
    */

    return 0;
}
