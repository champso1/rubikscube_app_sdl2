#include "utils.h"

char *time_float_to_str(float time) {
    int mins = time / 60; 
    int secs = floorf(time); // should give what's before decimal (6.789 -> 6)
    int ms = floorf(1000.0f * (time - secs)); // should give everything past the decimal (6.789 -> .789) 789 / 100 7

    char *time_str = NULL;
    time_str = malloc(sizeof(char) * 16);
    ptr_assert(time_str, "time_float_to_str(): error allocating memory for time_str\n");

    sprintf(time_str, "%d%d:%d%d.%d%d%d", mins/10, mins % 10, secs/10, secs % 10, ms / 100, (ms - (ms/100)*100) / 10, ms % 10);

    return time_str;
}