#include "functions.h"

int running_foreground_process = 0;
void sigint_handler(int sig)
{
    if (running_foreground_process != 0)
    {
        int d = kill(running_foreground_process, SIGINT);

        // printf("%d\n",d);
    }
    // fflush(stdout);
}
void sigtstp_handler(int sig)
{
    if (running_foreground_process != 0)
    {
        // send_ping(running_foreground_process,19);
        int d = kill(running_foreground_process, SIGTSTP);
        // printf("%d\n", d);
        // fflush(stdout);
        // printf("%d\n", running_foreground_process);
    }
}
void sigkill_handler(int sig)
{
    kill(0, SIGTERM);
    exit(0);
}
void sigpipe_handler(int sig)
{
    printf("oops\n");
}
int main()
{
    int savestdin = dup(STDIN_FILENO);

    int activities_saved_count = 0;
    char **activities_saved;
    activities_saved = (char **)malloc(sizeof(char *) * 100);
    for (int i = 0; i < 100; i++)
    {
        activities_saved[i] = (char *)malloc(sizeof(char) * MAX_INPUT_LENGTH);
    }
    char input[MAX_INPUT_LENGTH];
    char current_dir[PATH_MAX];
    char home_dir[PATH_MAX];
    char prev_dir[PATH_MAX];
    getcwd(home_dir, sizeof(home_dir));
    char *history[15];
    for (int i = 0; i < 15; i++)
    {
        history[i] = (char *)malloc(4096 * sizeof(char));
    }
    int history_size = 15;

    // FILE* fptr;
    // fptr = fopen("HISTORY.txt","w");
    // char historyfile[PATH_MAX];
    // getcwd(historyfile)
    // fclose(fptr);

    char historyfile[PATH_MAX];
    strcpy(historyfile, home_dir);
    strcat(historyfile, "/HISTORY.txt");

    char seek_save_file[PATH_MAX];
    strcpy(seek_save_file, home_dir);
    strcat(seek_save_file, "/seekFile.txt");

    int history_count = 0;
    load_history(historyfile, history, history_size, &history_count);

    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror("Error getting current directory");
        return 1;
    }
    strcpy(home_dir, current_dir);
    strcpy(prev_dir, current_dir);
    int bgprocessids[10000];
    char *bgcommands[10000];
    int bgcount = 0;
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    signal(SIGPIPE, sigpipe_handler);
    // signal(SIGKILL,sigkill_handler);
    while (1)
    {
        for (int i = 0; i < bgcount; i++)
        {
            // printf("%d\n",bgprocessids[i]);
            waitpid(bgprocessids[i], NULL, WNOHANG);
        }
        prompt(home_dir);
        // for (int i=0;i<4096;i++) input[i]='\0';
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            kill(-getpid(), SIGKILL);
        }
        // Remove newline character from input
        input[strcspn(input, "\n")] = 0;
        // for (int i=0;i<4096;i++)
        //     printf("%c", input[i]);
        // printf("\n");

        // Tokenize input using strtok
        char *token;
        char *tokens[MAX_ARGS];
        char *alltokens[MAX_ARGS];
        int token_count = 0, tkc2 = 0;
        ;
        int token_delimiting_operators = 0, is_delimitor = 0;
        token = strtok(input, " \t");

        while (token != NULL)
        {

            alltokens[tkc2++] = strdup(token);
            token = strtok(NULL, " \t");
        }
        for (int iii = 0; iii < tkc2; iii++)
        {
            tokens[token_count++] = strdup(alltokens[iii]);
            if (strcmp(alltokens[iii], ";") == 0 || strcmp(alltokens[iii], "&") == 0 || strcmp(alltokens[iii], "|") == 0)
            {
                if (strcmp(alltokens[iii], ";") == 0)
                    token_delimiting_operators = 1;
                else if (strcmp(alltokens[iii], "&") == 0)
                    token_delimiting_operators = 2;
                else if (strcmp(alltokens[iii], "|") == 0)
                    token_delimiting_operators = 3;
                is_delimitor = 1;
                // func();
                token_count--;
                tokens[token_count] = NULL;
                goto doeverything;
            tokenize_continue:;
                token_count = 0;
                token_delimiting_operators = 0;
                continue;
            }
        }
    doeverything:;
        int savestdout = dup(STDOUT_FILENO);
        tokens[token_count] = NULL;
        int commandtokens = token_count;
        int pipe_count = 0;
        int was_pipe;

        for (int i = 0; i < token_count - 1; i++)
        {
            // printf("x%sx\n",tokens[i]);
            int fd = 1;
            if (strcmp(tokens[i], ">>") == 0)
            {
                if (i < commandtokens)
                    commandtokens = i;
                fd = open(tokens[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                dup2(fd, STDOUT_FILENO);
            }
            else if (strcmp(tokens[i], ">") == 0)
            {
                if (i < commandtokens)
                    commandtokens = i;
                fd = open(tokens[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd, STDOUT_FILENO);
            }
            else if (strcmp(tokens[i], "<") == 0)
            {
                if (i < commandtokens)
                    commandtokens = i;
                fd = open(tokens[i + 1], O_RDONLY);
                dup2(fd, STDIN_FILENO);
            }
            if (fd == -1)
            {
                printf("Cannot open file\n");
                goto end;
            }
            // close(fd);
        }
        int ret = -1;
        token_count = commandtokens;
        tokens[token_count] = NULL;
        if (token_delimiting_operators == 3)
        {
            int piping[2];
            int a = pipe(piping);
            ret = fork();
            if (ret == 0)
            {
                dup2(piping[1], STDOUT_FILENO);
                close(savestdin);
                close(savestdout);
                close(piping[1]);
                close(piping[0]);
            }
            else
            {
                waitpid(ret, NULL, 1);

                close(piping[1]);
                dup2(piping[0], STDIN_FILENO);
                close(piping[0]);
                goto end;
            }
            // token_delimiting_operators=1;
        }
        if (token_delimiting_operators == 2)
        {
            pid_t ppid = fork();

            if (ppid == 0)
            {
                if (execvp(tokens[0], tokens) != 0)
                {
                    perror("Invalid Command\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                sprintf(activities_saved[activities_saved_count], "%d ", ppid);
                strcat(activities_saved[activities_saved_count], ": ");
                for (int i = 0; i < token_count; i++)
                {
                    printf("%s\n", tokens[i]);
                    strcat(activities_saved[activities_saved_count], tokens[i]);
                    strcat(activities_saved[activities_saved_count], " ");
                }
                activities_saved_count++;
            }
            bgprocessids[bgcount] = ppid;
            bgcommands[bgcount++] = strdup(tokens[0]);
            printf("[%d] %d\n", bgcount, ppid);
            goto tokenize_continue;
            // int status;
            // int completed=waitpid(bgpid[i],&status,WNOHANG);
            // if(completed)
            // {
            //     printf("sleep with pid %d exited normally\n", ppid);
            // }
        }
        // if(token_delimiting_operators==2)
        //     printf("---%s---",tokens[0]);
        else if (token_count > 0)
        {
            if (strcmp(tokens[0], "exit") == 0)
            {
                // Exit the shell
                break;
            }
            else if (strcmp(tokens[0], "fg") == 0 || strcmp(tokens[0], "bg") == 0)
            {
                if (token_count < 2 || token_count > 2)
                    printf("Incorrect Arguments\n");
                else
                {
                    pid_t pid = atoi(tokens[1]);
                    int t = kill(pid, SIGCONT);
                    if (strcmp(tokens[0], "fg") == 0)
                    {
                        running_foreground_process = pid;
                        waitpid(pid, NULL, WUNTRACED);
                        running_foreground_process = 0;
                    }
                }
            }
            else if ((strcmp(tokens[0], "iMan") == 0))
            {
                if (token_count > 1)
                {
                    fetchManPage(tokens[1]);
                }
            }
            else if (strcmp(tokens[0], "neonate") == 0)
            {
                if (token_count > 1 && (tokens[1], "-n"))
                {
                    if (token_count > 2)
                    {
                        int time_interval = atoi(tokens[2]);
                        int t = run_neonate(time_interval);
                    }
                    else
                    {
                        printf("Not enough arguments\n");
                    }
                }
                else
                {
                    printf("Flag not provided\n");
                }
            }
            else if (strcmp(tokens[0], "ping") == 0)
            {
                if (token_count == 3)
                {
                    send_ping(atoi(tokens[1]), atoi(tokens[2]));
                }
                else
                {
                    printf("Wrong number of arguments\n");
                }
            }
            else if (strcmp(tokens[0], "activities") == 0)
            {
                // printf("%s", activities_saved[0]);
                print_activities(activities_saved, &activities_saved_count);
            }
            else if (strcmp(tokens[0], "warp") == 0)
            {
                {
                    // Execute warp command
                    execute_warp(tokens, prev_dir, current_dir, home_dir, token_count);

                    // Update current directory
                    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
                    {
                        perror("Error getting current directory");
                        return 1;
                    }
                }
            }
            else if (strcmp(tokens[0], "peek") == 0)
            {
                bool show_hidden = false;
                bool show_details = false;
                char directory_path[PATH_MAX];
                strcpy(directory_path, current_dir); // Default path is current directory

                // Check for flags
                for (int i = 1; i < token_count; ++i)
                {
                    if (strcmp(tokens[i], "-a") == 0)
                    {
                        show_hidden = true;
                    }
                    else if (strcmp(tokens[i], "-l") == 0)
                    {
                        show_details = true;
                    }
                    else if (strcmp(tokens[i], ".") == 0)
                    {
                        ;
                    }
                    else if (strcmp(tokens[i], "..") == 0)
                    {
                        char *expanded_path = strdup(tokens[i]);
                        snprintf(directory_path, PATH_MAX, "%s/%s", current_dir, expanded_path);
                    }
                    else if (strcmp(tokens[i], "~") == 0)
                    {
                        strcpy(directory_path, home_dir);
                    }
                    else
                    {
                        show_details = true;
                        show_hidden = true;
                    }

                    // else if (tokens[i][0] != '-')
                    // {
                    //     // Assume this is the directory path
                    //     directory_path = expand_tilde(tokens[i]);
                    // }
                }

                // Execute peek command
                peek_directory(directory_path, show_hidden, show_details);
            }
            else if (strcmp(tokens[0], "proclore") == 0)
            {
                if (token_count == 1)
                    proclore_process(getpid());
                else
                    proclore_process(atoi(tokens[1]));
            }
            else if (strcmp(tokens[0], "seek") == 0)
            {
                int dironly = 0, fileonly = 0, execute_flag = 0;
                char target_name[PATH_MAX];
                char target_dir[PATH_MAX];
                strcpy(target_dir, current_dir);
                int flag = 0;
                for (int i = 1; i < token_count; i++)
                {
                    if (strcmp(tokens[i], "-d") == 0)
                        dironly = 1;
                    else if (strcmp(tokens[i], "-f") == 0)
                        fileonly = 1;
                    else if (strcmp(tokens[i], "-e") == 0)
                        execute_flag = 1;
                    else if (flag == 0)
                    {
                        flag = 1;
                        strcpy(target_name, tokens[i]);
                    }
                    else
                    {
                        strcpy(target_dir, tokens[i]);
                    }
                }
                int found = 0, founddir = 0;
                seek_directory(target_name, target_dir, dironly, fileonly, execute_flag, &found, &founddir, seek_save_file);
                if (!found)
                    printf("NOT FOUND\n");
                else if (execute_flag)
                {
                    if (found == 1)
                    {
                        if (founddir)
                        {
                            // change dir
                            FILE *nfile = fopen(seek_save_file, "r");
                            char buffer98[4096];
                            int nytyt = fread(buffer98, 1, 4095, nfile);
                            buffer98[nytyt] = '\0';
                            fclose(nfile);

                            chdir(buffer98);
                        }
                        else
                        {
                            FILE *nfile = fopen(seek_save_file, "r");
                            char buffer98[4096];
                            int nytyt = fread(buffer98, 1, 4095, nfile);
                            buffer98[nytyt] = '\0';
                            fclose(nfile);

                            buffer98[strlen(buffer98) - 1] = '\0';
                            nfile = fopen(buffer98, "r");
                            nytyt = fread(buffer98, 1, 4095, nfile);
                            buffer98[nytyt] = '\0';
                            printf("%s\n", buffer98);
                            fclose(nfile);
                            // remove(seek_save_file);
                        }
                    }
                    else
                    {
                        FILE *nfile = fopen(seek_save_file, "r");
                        char buffer98[4096];
                        int nytyt = fread(buffer98, 1, 4095, nfile);
                        buffer98[nytyt] = '\0';
                        fclose(nfile);

                        printf("%s\n", buffer98);
                    }
                }
                else
                {
                    // print everything
                    FILE *nfile = fopen(seek_save_file, "r");
                    char buffer98[4096];
                    int nytyt = fread(buffer98, 1, 4095, nfile);
                    buffer98[nytyt] = '\0';
                    fclose(nfile);
                    char line[4096];

                    printf("%s\n", buffer98);
                }

                found = 0;
            }
            //---------------------------------------------------------------
            else if (strncmp(input, "pastevents", 10) == 0)
            {
                if (token_count > 1 && strcmp(tokens[1], "execute") == 0)
                {
                    // Parse 'pastevents execute <index>'
                    int index;
                    if (token_count > 2)
                        (index = atoi(tokens[2]));
                    else
                    {
                        printf("Invalid command\n");
                        break;
                    }
                }
                else if (token_count > 1 && strcmp(tokens[1], "purge") == 0)
                {
                    // Clear the history
                    history_count = 0;
                    printf("History cleared.\n");
                    save_history(historyfile, history, history_count);
                    continue;
                }
                else
                {
                    // Print the history
                    print_history(history, history_count);
                    continue;
                }
            }
            //---------------------------------------------------------------------------
            else
            {
                // Execute other commands
                execute_command(tokens, 0, activities_saved, &activities_saved_count, bgprocessids, bgcommands, &bgcount);
            }
            //----------------------------------------------------------------------------
            if (strncmp(input, "pastevents", 10) != 0 && strncmp(input, "pastevents purge", 16) != 0)
            {
                // Don't store repeated commands
                if (history_count == 0 || strcmp(input, history[history_count - 1]) != 0)
                {
                    // Shift history to make room for the new command
                    if (history_count == history_size)
                    {
                        for (int i = 0; i < history_size - 1; ++i)
                        {
                            strcpy(history[i], history[i + 1]);
                        }
                    }
                    else
                    {
                        history_count++;
                    }
                    // history[history_count-1]="\0";
                    int cc = 0;
                    for (int i = 0; i < 4096; i++)
                        history[history_count - 1][i] == ' ';
                    for (int i = 0; i < token_count; i++)
                    {
                        for (int j = 0; j < strlen(tokens[i]); j++)
                        {
                            history[history_count - 1][cc++] = tokens[i][j];
                        }
                        history[history_count - 1][cc++] = ' ';
                    }
                    // for (int i=0;i<4096;i++){
                    //     // printf("%c",input[i]);
                    //     history[history_count-1][i]=input[i];
                    // }
                    save_history(historyfile, history, history_count);
                }
            }
            getcwd(current_dir, sizeof(current_dir));
            if (token_delimiting_operators == 3)
            {
                was_pipe = 1;
            }
        }
        if (ret == 0)
        {
            exit(0);
            ret = -1;
        }
    end:;
        if (was_pipe==1&&token_delimiting_operators!=3)
        {
            dup2(savestdout,STDOUT_FILENO);
            dup2(savestdin,STDIN_FILENO);
            close(savestdin);
            close(savestdout);
            // printf("a");
            // fflush(stdout);
            savestdin=dup(STDIN_FILENO);
            savestdout=dup(STDOUT_FILENO);
        }
        if (is_delimitor)
        {
            // dup2(savestdout, STDOUT_FILENO);
            // dup2(savestdin, STDIN_FILENO);
            is_delimitor = 0;
            goto tokenize_continue;
        }

        dup2(savestdout, STDOUT_FILENO);
        dup2(savestdin, STDIN_FILENO);
        close(savestdin);
        close(savestdout);
        fflush(stdout);
    }
    return 0;
}
