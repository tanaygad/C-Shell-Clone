#include "functions.h"


int main()
{
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

    while (1)
    {
        prompt(home_dir);
        // for (int i=0;i<4096;i++) input[i]='\0';
        fgets(input, sizeof(input), stdin);
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
            if (strcmp(alltokens[iii], ";") == 0 || strcmp(alltokens[iii], "&") == 0)
            {
                if (strcmp(alltokens[iii], ";") == 0)
                    token_delimiting_operators = 1;
                else if (strcmp(alltokens[iii], "&") == 0)
                    token_delimiting_operators = 2;
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
        tokens[token_count] = NULL;
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
                execute_command(tokens, 0);
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
            if (is_delimitor)
            {
                is_delimitor = 0;
                goto tokenize_continue;
            }
        }
    }
    return 0;
}
