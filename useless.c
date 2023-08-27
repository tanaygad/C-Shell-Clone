#include "functions.h"

char *get_process_status(char *status)
{
    if (strcmp(status, "R") == 0)
    {
        return "Running";
    }
    else if (strcmp(status, "S") == 0)
    {
        return "Sleeping in an interruptible wait";
    }
    else if (strcmp(status, "Z") == 0)
    {
        return "Zombie";
    }
    else
    {
        return "Unknown";
    }
}
