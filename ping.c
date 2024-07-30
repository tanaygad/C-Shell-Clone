#include "functions.h"

void send_ping(int pid, int signal)
{
    
    signal %= 32;

    // Check if the process with the given PID exists
    if (kill(pid, 0) == -1) {
        perror("No such process found");
        exit(EXIT_FAILURE);
    }

    // Send the specified signal to the process
    if (kill(pid, signal ) == -1) {
        perror("Error sending signal");
        exit(EXIT_FAILURE);
    }

    printf("Sent signal %d to process with pid %d\n", signal, pid);
    
    return ;
}

