#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../../include/sqlite3.h"

#define ptr_assert(_ptr, ...) if(!(_ptr)) {fprintf(stderr, __VA_ARGS__); exit(1);}



#define NUM_LINES_TO_SETUP 100




static int callback(void *xxx, int argc, char **argv, char **col_names) {
    for (int i=0; i<argc; i++) {
        printf("%s;", argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

static int table_info_callback(void *xxx, int argc, char **argv, char **col_names) {
    for (int i=0; i<argc; i++) {
        printf("%s: %s ", col_names[i], argv[i]);
    }
    printf("\n\n");
    return 0;
}





char *fix_date(char *original_date_str);
char *fix_scramble(char *scramble);

static void setup(FILE *times_txt, sqlite3 *db);



int main(int argc, char *argv[]) {
/* ERROR CHECKING */

    if (argc < 2) {
        fprintf(stderr, "usage: .\\sql.exe <option> <args>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "\t-create: creates a new table (row names already determined)\n");
        fprintf(stderr, "\t-setup: adds records from a text file in CubeTimer format\n");
        fprintf(stderr, "\t-print_table_info: prints out info about the table: column names, etc.\n");
        fprintf(stderr, "\t-insert: inserts an entry into the table with values passed into <args>\n");
        fprintf(stderr, "\t\targs: <arg1:CUBE_TYPE> <arg2: SCRAMBLE> <arg3:TIME> <arg4:DATE>\n");
        fprintf(stderr, "\t-select_all: selects all elements in the table (DANGEROUS!)\n");
        fprintf(stderr, "\t-select_recent_entry: selects the most recent table entry\n");
        fprintf(stderr, "\t-select_recent_time: selects the most recent time\n");
        fprintf(stderr, "\t-clear: wipes the table\n");
        fprintf(stderr, "\t-select_best: selects the best solve\n");
        fprintf(stderr, "\t-delete: deletes the table (VERY DANGEROUS!)\n\n\n");
        exit(1);
    }

    // test the insert command
    if (strcmp(argv[1], "-insert") == 0 && argc != 6) { // program, -insert, 4 args = 6 total
        fprintf(stderr, "Incorrect number of arguments to -insert:\n");
        fprintf(stderr, "Expected 4, got %d\n", argc - 2);
        fprintf(stderr, "Usage: .\\sql.exe -insert <arg1:CUBE_TYPE> <arg2: SCRAMBLE> <arg3:TIME> <arg4:DATE>\n");
        exit(1);
    }

    // test the setup command, this one is kinda separate
    if (strcmp(argv[1], "-setup") == 0) {
        char *txt_file_path = "..\\times.txt";
        char *db_file_path = "solves.db";

        FILE *times_txt = times_txt = fopen(txt_file_path, "r");
        ptr_assert(times_txt, "setup(): error opening file\n");

        sqlite3 *db;
        sqlite3_open(db_file_path, &db);


        setup(times_txt, db);


        if (times_txt != NULL) fclose(times_txt);
        sqlite3_close(db);
        exit(0);
    }


    char option[16];
    strcpy(option, argv[1]);
    char cube_type[16];
    char scramble[256];
    char time[16];
    char date[64];
    const size_t insert_stmt_size = 16 + 256 + 16 + 64;
    char insert_stmt[insert_stmt_size];



    /* DEFINING SQL STATEMENTS*/

    char *create = "CREATE TABLE SOLVES(\
        CUBE_TYPE   varchar(16)     NOT NULL,\
        SCRAMBLE    varchar(256)    NOT NULL,\
        TIME        REAL            NOT NULL,\
        DATE        varchar(64)     NOT NULL\
    );"; 
    char *print_table_info = "PRAGMA table_info(SOLVES);";
    char *insert = "INSERT into SOLVES (CUBE_TYPE,SCRAMBLE,TIME,DATE) \
                    VALUES "; // need to add shit to this one
    char *selectall = "SELECT * from SOLVES;";
    char *select1 = "SELECT * from SOLVES \
                    ORDER BY ROWID DESC \
                    LIMIT 1;";
    char *select_most_recent_time = "SELECT TIME from SOLVES \
                                    ORDER BY ROWID DESC \
                                    LIMIT 1;";
    char *clear = "DELETE from SOLVES;";
    char *select_best_time =    "SELECT TIME from SOLVES \
                                ORDER BY TIME ASC \
                                LIMIT 1;";
    char *select_5_recent_times =   "SELECT TIME FROM SOLVES \
                                    ORDER BY DATE DESC \
                                    LIMIT 5;";
    char *delete = "DROP TABLE SOLVES;";


    char *options[] = {create, print_table_info, insert, selectall, select1, select_most_recent_time, clear, select_best_time, select_5_recent_times, delete};

    char *options_str[] = {"-create", "-print_table_info", "-insert", "-select_all", "-select_recent_entry", "-select_recent_time", "-clear", "-select_best", "-select_5_recent", "-delete"};

    char *success_msgs[] = {"table created\n", "\n", "element inserted\n", "\n", "\n", "\n", "table cleared\n", "\n", "\n", "table deleted\n"};


    /* INSERT STATEMENT IS SEPARATE */

    if (strcmp(option, "-insert") == 0 && argc == 6) {
        strcpy(cube_type, argv[2]);
        strcpy(scramble, argv[3]);
        strcpy(time, argv[4]);
        strcpy(date, argv[5]);


        // POWERSHELL I WILL KILL YOU WHY DON'T YOU LIKE ANYTHING THAT I DO 
        // TODO: fix this i guess

        sprintf(insert_stmt, "(%s, %s, %s, %s);", cube_type, scramble, time, date);
        strcat(insert, insert_stmt);

        printf("%s\n\n", insert);
        exit(0);
    }





    /* INITIALIZATION STUFF */

    sqlite3 *db;
    int rc = sqlite3_open("solves.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "error opening database file");
        exit(1);
    }
    
    char sql[256];
    char success_msg[32];
    int (*callback_func)(void *, int, char **, char **) = callback;

    bool is_command = false;

    for (int i=0; i<(sizeof(options)/sizeof(*options)); i++) {
        if (strcmp(options_str[i], option) == 0) {
            strcpy(sql, options[i]);
            strcpy(success_msg, success_msgs[i]);
            is_command = true;
        }
    }
    if (strcmp(option, "-print_table_info") == 0) callback_func = table_info_callback;

    if (!is_command) {
        fprintf(stderr, "invalid command\n");
        sqlite3_close(db);
        exit(1);
    }


    char *errmsg;

    rc = sqlite3_exec(db, sql, callback_func, 0, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "error running command: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        exit(1);
    }
    printf(success_msg);

    sqlite3_close(db);
    return 0;
}



char *fix_date(char *original_date_str) {
    char *original_date_saveptr;

    // DATE PORTION
    char *date = strtok_r(original_date_str, " ", &original_date_saveptr);

    char *date_saveptr;
    char *year = strtok_r(date, "-", &date_saveptr);
    char *new_year = year + 2;
    char *month = strtok_r(NULL, "-", &date_saveptr);
    char *day = strtok_r(NULL, "-", &date_saveptr);


    char *time_str = strtok_r(NULL, " ", &original_date_saveptr);

    char *time_saveptr;
    char *hour = strtok_r(time_str, ":", &time_saveptr);
    char *min = strtok_r(NULL, ":", &time_saveptr);
    char *sec = "00";

    char new_date[64];
    sprintf(new_date, "%s/%s/%s %s:%s:%s", month, day, new_year, hour, min, sec);
    char *new_date_ptr = malloc(sizeof(char) * (strlen(new_date) + 1));
    strcpy(new_date_ptr, new_date);

    return new_date_ptr;
}


char *fix_scramble (char *scramble) {
    char new_scramble[256] = "";
    char *scramble_saveptr;

    int offset = 2;
    if (scramble[strlen(scramble) - 1] == '\'' ) offset = 0;

    char *token = strtok_r(scramble, "'", &scramble_saveptr);
    while (token != NULL) {
        strcat(new_scramble, token);
        strcat(new_scramble, "''");

        token = strtok_r(NULL, "'", &scramble_saveptr);
    }
    new_scramble[strlen(new_scramble) - offset] = 0;

    char *new_scramble_ptr = malloc(sizeof(char) * (strlen(new_scramble) + 1));
    strcpy(new_scramble_ptr, new_scramble);
    
    return new_scramble_ptr;
}


void setup(FILE *times_txt, sqlite3 *db) {
    const size_t buf_size = 500; //7x7x7 solves are wild

    int rc;
    char *errmsg;

    char buffer[buf_size];
    char *buffer_saveptr;
    fgets(buffer, buf_size, times_txt);

    for (int i=0; i<NUM_LINES_TO_SETUP; i++) {
        char insert[500] = "INSERT into SOLVES (CUBE_TYPE,SCRAMBLE,TIME,DATE) \
                    VALUES ";
        fgets(buffer, buf_size, times_txt);

        char *token = strtok_r(buffer, ";", &buffer_saveptr); // cube_type
        char cube_type[16];
        strcpy(cube_type, token);


        token = strtok_r(NULL, ";", &buffer_saveptr); // time
        char time[16];
        strcpy(time, token);


        token = strtok_r(NULL, ";", &buffer_saveptr); // scramble
        char scramble[64];
        strcpy(scramble, token);
        char *new_scramble = fix_scramble(scramble);


        token = strtok_r(NULL, ";", &buffer_saveptr); // date
        char date[64];
        strcpy(date, token);
        char *new_date = fix_date(date);

        char insert_stmt[450];
        sprintf(insert_stmt, "('%s', '%s', '%s', '%s');", cube_type, new_scramble, time, new_date);
        strcat(insert, insert_stmt);

        
        rc = sqlite3_exec(db, insert, NULL, NULL, &errmsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "setup(): error executing sql statement: %s\n", errmsg);
            sqlite3_close(db);
            sqlite3_free(errmsg);
            fclose(times_txt);
            exit(1);
        }
        free(new_scramble);
        free(new_date);
    }


    if (times_txt != NULL) fclose(times_txt);
    sqlite3_close(db);
}