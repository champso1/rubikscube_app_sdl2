#ifndef SOLVE_H
#define SOLVE_H

#include "utils.h"
#include "Window.h"
#include <time.h>

#define SCRAMBLE_SIZE 25

extern char *moves[3][6];

/**
 * Generates a random 25-move long scramble
 * 
 * \returns A string containing the scramble
*/
extern char *generate_scramble();




extern sqlite3 *db;

/*
CUBE_TYPE   CHAR(16)    NOT NULL
SCRAMBLE    CHAR(64)    NOT NULL
TIME        REAL        NOT NULL
DATE        CHAR(64)    NOT NULL
*/


/**
 * The callback function passed to `sqlite3_exec()`.
 * This will spit to stdout, each on a new line, <COL_NAME>: <COL_DATA> 
 * for a row in the database.
 * 
 * \param xxx Not used.
 * \param argc Number of columns in the row
 * \param argv List of the data in the columns
 * \param col_names String representations of the names of the columns
 * 
 * \returns 0 on success. 1 on failure.
*/
extern int callback(void *xxx, int argc, char **argv, char **col_names);

/**
 * Initializes an sqlite3 database connection used to read and store times. Assumes the database
 * is already set up in terms of tables and whatnot
 * 
 * \param filepath The path to the database file
 * 
 * \returns Nothing
*/
extern void init_sql_db(const char *filepath);

/**
 * Closes an opened sqlite3 database connection.
*/
extern void close_sql_db();

/**
 * Saves the solve time, the scramble, and the date to an SQL database. Will free 
 * the scramble pointer.
 * 
 * \param time The scramble time, in seconds
 * \param scramble The scramble string. Will be freed.
 * 
 * \returns Nothing.
*/
extern void solve_save(uint8_t cube_type, float time, char *scramble);

extern void solve_print();

#endif // SOLVE_H