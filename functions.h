#ifndef FUNCCH
#define FUNCCH

#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h> // Add this line to include the necessary header
#include <limits.h>
#include <fcntl.h>
#include <termios.h>
#include <ctype.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX_INPUT_LENGTH 4096
#define MAX_ARGS 4096
#define PATH_MAX 4096
#define MAX_INPUT_LENGTH 4096
#define MAX_RESPONSE_SIZE 4096


#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_BLUE "\033[34m"
#define COLOR_WHITE "\033[37m"


extern int running_foreground_process;

void prompt(char *initial);
void fetchManPage(const char* command);
void execute_command(char **args, int background, char** activities_saved, int* activities_saved_count, int* bgprocessids, char** bgcommands, int* bgcount);
void peek_directory(char *path, bool show_hidden, bool show_details);
void execute_warp(char **args, char *prev_dir, char *current_dir, char *home_dir, int token_count);
void proclore_process(pid_t pid);
void print_colored(const char *text, const char *color);
void seek_directory(const char *target_name, const char *target_directory, int directories_only, int files_only, int execute_flag, int *found, int *founddir, char *save_file);
void save_history(char *filename, char **history, int history_size);
void load_history(char *filename, char *history[], int history_size, int *history_count);
void print_history(char *history[], int history_size);
void print_activities(char **activities_saved, int* activities_saved_count);
void send_ping(int pid, int signal);
int run_neonate(int time_interval);
void enableRawMode();
void disableRawMode();
void die(const char *s);

#endif 
