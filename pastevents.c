#include "functions.h"

void save_history(char *filename, char **history, int history_size)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error saving history");
        return;
    }

    for (int i = 0; i < history_size; ++i)
    {
        // printf("%s\n", history[i]);
        for (int j = 0; j < 4096; j++)
            fprintf(file, "%c", history[i][j]);
        fprintf(file, "\n");
    }

    fclose(file);
}

void load_history(char *filename, char *history[], int history_size, int *history_count)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return; // No history file yet
    }

    char line[MAX_INPUT_LENGTH];
    int i = 0;

    while (fgets(line, sizeof(line), file))
    {
        // (*history_count)++;
        line[strcspn(line, "\n")] = 0; // Remove newline character
        strncpy(history[i++], line, MAX_INPUT_LENGTH);
        if (i == history_size)
        {
            break; // Read up to history_size lines
        }
    }

    fclose(file);
}

void print_history(char *history[], int history_size)
{
    
    for (int i = 0; i < history_size; ++i)
    {
        for (int j = 0; j < 4096; j++)
            printf("%c", history[i][j]);
        printf("\n");
    }
}
