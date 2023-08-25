#include "functions.h"

#define MAX_INPUT_LENGTH 1024
// #define MAX_ARGS 64

int main()
{
    char input[MAX_INPUT_LENGTH];
    char current_dir[PATH_MAX];
    char home_dir[PATH_MAX];
    char prev_dir[PATH_MAX];

    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror("Error getting current directory");
        return 1;
    }
    strcpy(home_dir, current_dir);
    strcpy(prev_dir, current_dir);

    while (1)
    {
        prompt(home_dir);
        fgets(input, sizeof(input), stdin);

        // Remove newline character from input
        input[strcspn(input, "\n")] = 0;

        // Tokenize input using strtok
        char *token;
        char *tokens[MAX_ARGS];
        int token_count = 0;

        token = strtok(input, " \t");
        while (token != NULL)
        {
            tokens[token_count++] = token;
            token = strtok(NULL, " \t");
        }
        tokens[token_count] = NULL;

        if (token_count > 0)
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
                    execute_warp(tokens,prev_dir, current_dir, home_dir, token_count);

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
                char directory_path[PATH_MAX] ;
                strcpy(directory_path,current_dir); // Default path is current directory

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
                    else if (strcmp(tokens[i],".")==0){
                        ;
                    }
                    else if (strcmp(tokens[i],"..")==0){
                        char *expanded_path = strdup(tokens[i]);
                        snprintf(directory_path, PATH_MAX, "%s/%s", current_dir, expanded_path);
                    }
                    else if (strcmp(tokens[i],"~")==0){
                        strcpy(directory_path,home_dir);
                    }
                    else {
                        show_details=true;
                        show_hidden=true;
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
            else if (strcmp(tokens[0],"proclore")==0){
                if (token_count==1) proclore_process(getpid());
                else proclore_process(atoi(tokens[1]));
            }
            else if (strcmp(tokens[0],"seek")==0){
                int dironly=0,fileonly=0,execute_flag=0;
                char target_name[PATH_MAX];
                char target_dir[PATH_MAX];
                strcpy(target_dir,current_dir);
                int flag=0;
                for (int i=1;i<token_count;i++){
                    if (strcmp(tokens[i],"-d")==0) dironly=1;
                    else if (strcmp(tokens[i],"-f")==0) fileonly=1;
                    else if (strcmp(tokens[i],"-e")==0) execute_flag=1;
                    else if (flag==0){
                        flag=1;
                        strcpy(target_name,tokens[i]);
                    }
                    else {
                        strcpy(target_dir,tokens[i]);
                    }
                }
                seek_directory(target_name,target_dir,dironly,fileonly,execute_flag);
            }
            else
            {
                // Execute other commands
                execute_command(tokens, 0);
            }
            getcwd(current_dir, sizeof(current_dir));
        }

    }
        return 0;
}
