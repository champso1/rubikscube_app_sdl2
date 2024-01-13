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
CUBE_TYPE   varchar(16)     NOT NULL,
SCRAMBLE    varchar(256)    NOT NULL,
TIME        varchar(16)     NOT NULL,
DATE        varchar(64)     NOT NULL
*/



extern int      callback(void *data, int argc, char **argv, char **col_names);
extern void     init_sql_db(const char *filepath);
extern void     close_sql_db();
extern void     solve_save(uint8_t cube_type, float time, char *scramble);


extern char     *get_best_time();
extern void     solve_print();

#endif // SOLVE_H