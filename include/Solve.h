#ifndef SOLVE_H
#define SOLVE_H

#include "utils.h"
#include "Window.h"

#include <time.h>

#define SCRAMBLE_SIZE 25

extern char *moves[3][6];


extern char *generate_scramble();




extern sqlite3 *db;

/*
Cube_Type   varchar(16)     NOT NULL,
Scramble    varchar(256)    NOT NULL,
Time        REAL            NOT NULL,
Date        varchar(64)     NOT NULL
*/



extern int      callback(void *data, int argc, char **argv, char **col_names);
extern void     init_sql_db(const char *filepath);
extern void     close_sql_db();
extern void     solve_save(uint8_t cube_type, float time, char *scramble);


extern int      __time_callback(void *__time, int argc, char **argv, char **col_names);
extern char     *get_best_time();



typedef struct {
    float times[5];
    int ptr_loc;
} Avg5;

extern int      __get_avg5_callback(void *data, int argc, char **argv, char **col_names);
extern char     *get_avg5();


extern void     solve_print();

#endif // SOLVE_H