#include "Solve.h"


char *moves[3][6] = {
    {"U", "D", "F", "B", "R", "L"},
    {"U'", "D'", "F'", "B'", "R'", "L'"},
    {"U2", "D2", "F2", "B2", "R2", "L2"}
};

char *generate_scramble() {  

    char scramble_arr[SCRAMBLE_SIZE + 1][4]; //plus 1 for newline character; helps with word wrapping when rendering it
    char cache[2][4];
    int num_cache[2] = {0};

    int i=0;
    for ( ; i<SCRAMBLE_SIZE; i++) {
        int move_type = rand() % 3;
        int move_num = rand() % 6;
        char *move = moves[move_type][move_num];

        // do the first iteration separately since there will be null/uninitialized stuff
        if (i == 0) {
            // nothing to compare on the first iteration, just stick this in cache[0]
            strcpy(cache[i], move);
            num_cache[i] = move_num;

            char move_str[4];
            sprintf(move_str, "%s ", move);
            strcpy(scramble_arr[i], move_str);
            continue;
        }

        // insert a newline character roughly 2/3 or 3/4 of the way in
        // to help with rendering
        if (i == 15) {
            strcpy(scramble_arr[i], "\n");
            continue;
        }


        // we should just be able to check if the move_num is the same
        // this will be a problem regardless of the move_type
        // any 2 U moves, for example, are not good
        // instead of having a ton of if elses to fix each type of problem individually
        // we will just redo the loop to get something that works
        if (move_num == num_cache[0]) {
            i -= 1;
            continue;
        }

        // now we need to test for if we get something like U D U or L R' L2
        // this is the same as U2 D (or D U2) and L' R' (or R' L') respectively
        // since the opposite faces (which is where this becomes a problem) are only 1 apart in the arrays
        // so I think if we take the difference between the move_num and if the different between each is 1 (or -1)
        // then we have a problem, and we should restart from one step back (rather than redoing the current loop)
        if ((move_num == num_cache[1]) && (move_num / 2 == num_cache[0] / 2) && i > 1) {
            num_cache[0] = num_cache[1];
            i -= 2;
            continue;
        }
        
        // after this point, we should be good to continue, all tests will have passed
        strcpy(cache[1], cache[0]);
        strcpy(cache[0], move);
        num_cache[1] = num_cache[0];
        num_cache[0] = move_num;

        char move_str[4]; // 3 chars max (2 chars max in a move plus a space) plus a null terminator = 4 bytes
        sprintf(move_str, "%s ", move);
        memcpy(scramble_arr[i], move_str, (sizeof move_str));
    }

    // we convert the array of moves to single string, then allocate memory on the heap
    // then copy the string into that so it can be returned.
    char scramble[200] = ""; // tons of extra space for null terminators
    for (int i=0; i<SCRAMBLE_SIZE; i++) {
        strcat(scramble, scramble_arr[i]);
    }
    strcat(scramble, "\0");
    char *scramble_ptr = malloc((sizeof(char)) * strlen(scramble) + 1); // +1 for null terminator you stupid fucking dumbass
    ptr_assert(scramble_ptr, "generate_scramble(): error allocating memory for scramble_ptr\n");

    strcpy(scramble_ptr, scramble);

    return scramble_ptr;
}





/** SQLITE **/



sqlite3 *db = NULL;

int callback(void *data, int argc, char **argv, char **col_names) {
    for (int i=0; i<argc; i++) {
        printf("%s: %s\n", col_names[i], argv[i] ? argv[i] : "NULL");
    }
    return 0;
}


void init_sql_db(const char *filepath) {
    int err = sqlite3_open(filepath, &db);
    if (err != SQLITE_OK) {
        fprintf(stderr, "init_sql_db(): error opening database connection.\n");
        exit(1);
    }
}

void close_sql_db() {
    ptr_assert(db, "close_sql_db(): database is not initialized\n");
    sqlite3_close(db);
}

void solve_save(uint8_t cube_type, float solve_time, char *scramble) {
    ptr_assert(db, "solve_save(): database not initialized\n");

    //must make all apostrophes in the scramble double apostrophes for the sql insert

    char scramble_text[256];
    char *token = strtok(scramble, "'");
    while (token != NULL) {
        strcat(scramble_text, token);
        strcat(scramble_text, "''");

        token = strtok(NULL, "'");
    }
    scramble_text[strlen(scramble_text)-3] = 0; //-3 will remove the trailing double apostrophes and the trailing space

    time_t current_time;
    struct tm *time_info;
    char time_str[64];

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(time_str, 64, "%c", time_info);

    char sql[256];
    sprintf(sql, "INSERT INTO SOLVES (CUBE_TYPE, SCRAMBLE, TIME, DATE) \
        VALUES ('%s', '%s', %.4f, '%s');", 
        Cube_Types_str[cube_type], scramble_text, solve_time, time_str);
    char *errmsg;

    int rc = sqlite3_exec(db, sql, NULL, 0, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "solve_save(): error saving solve in database: %s", errmsg);
        close_sql_db();
        exit(1);
    }
    printf("Solve saved successfully.\n");
}





static int __time_callback(void *__time_str, int argc, char **argv, char **col_names) {
    strcpy((char *)__time_str, argv[0]);
    return 0;
}

char *get_best_time() {
    ptr_assert(db, ":get_most_recent_time(): database not initialized\n");

    char sql[128] = "";
    char time_type_str[32] = "";
    strcpy(sql, "SELECT TIME from SOLVES \
            ORDER BY TIME ASC \
            LIMIT 1;"
    );
    strcpy(time_type_str, "Best solve");

    char *time_str = NULL;
    time_str = malloc(sizeof(char) * (64)); // 64 should be enough
    ptr_assert(time_str, "get_time(): error allocating memory for time_str");
    char *errmsg = 0;

    int rc = sqlite3_exec(db, sql, __time_callback, time_str, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "get_time(): Error getting the most recent time in the database: %s\n", errmsg);
        sqlite3_free(errmsg);
        exit(1);
    }

    // if there are no rows, callback is not called at all!
    // therefore, the time_str will still just be the empty string
    // so we can test that

    float time = 0.0f;
    if (strlen(time_str) > 0) {
        // this means success, otherwise, it will just use the 0.0
        sscanf(time_str, "%f", &time);
    }
    
    char *formatted_time = time_float_to_str(time);
    strcpy(time_str, ""); // to initialize, not sure if important but
    sprintf(time_str, "%s: %s", time_type_str, formatted_time);
    free(formatted_time);

    return time_str;
}




void solve_print() {
    ptr_assert(db, "solve_print(): database not initialized\n");

    char *sql = "SELECT * from SOLVES";
    char *errmsg;

    int rc = sqlite3_exec(db, sql, callback, 0, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "solve_print(): error retrieving data from the database\n");
        close_sql_db();
        exit(1);
    }
}