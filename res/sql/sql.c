#include <stdio.h>
#include <stdlib.h>

#include "../../include/sqlite3.h"

static int callback(void *xxx, int argc, char **argv, char **col_names) {
    for (int i=0; i<argc; i++) {
        printf("%s: %s\n", col_names[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n\n");
    return 0;
}

int main(void) {

    sqlite3 *db;
    int rc = sqlite3_open("solves.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "error opening database file");
        exit(1);
    }

    char *create = "CREATE TABLE SOLVES(\
        CUBE_TYPE   varchar(16)     NOT NULL,\
        SCRAMBLE    varchar(127)    NOT NULL,\
        TIME        REAL            NOT NULL,\
        DATE        varchar(63)     NOT NULL\
    );"; 
    char *insert = "INSERT into SOLVES (CUBE_TYPE,SCRAMBLE,TIME,DATE) \
                    VALUES('3x3x3', 'F U D R'' B R2', 8.89, 'Thursday');";
    char *selectall = "SELECT * from SOLVES;";
    char *select1 = "SELECT * from SOLVES \
                    ORDER BY ROWID DESC \
                    LIMIT 1;";
    char *delete = "DELETE from SOLVES;";
    char *errmsg;

    rc = sqlite3_exec(db, select1, callback, 0, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "error running command: %s", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        exit(1);
    }

    sqlite3_close(db);
    printf("all good");
    return 0;
}