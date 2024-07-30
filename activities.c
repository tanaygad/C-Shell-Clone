#include "functions.h"

int cmpfnc(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

void print_activities(char** activities_saved, int* activities_saved_count)
{
    
    qsort(activities_saved,*activities_saved_count,sizeof(char *),cmpfnc);
    for (int j = 0; j < *activities_saved_count; j++)
    {
        int pid=0;
        for (int k=0;k<MAX_INPUT_LENGTH;k++){
            if(activities_saved[j][k]==' ') break;
            else {
                pid*=10;
                pid+=(activities_saved[j][k]-'0');
            }
        }
        char fileName[MAX_INPUT_LENGTH];
        sprintf(fileName, "/proc/%d/stat", pid);
        FILE *f = fopen(fileName, "r");
        if (f != NULL)
        {
            char statFile[MAX_INPUT_LENGTH];
            fgets(statFile, MAX_INPUT_LENGTH, f);
            int count = 0;
            int stopflag=0;
            for (int i = 0; i < strlen(statFile); i++)
            {
                if (statFile[i] == ' ')
                    count++;
                if (count == 2)
                {
                    if (statFile[i + 1] == 'T')
                    {
                        stopflag=1;
                    }

                    break;
                }
            }
            if (stopflag) printf("%s - Stopped\n",activities_saved[j]);
            else printf("%s - Running\n",activities_saved[j]);
            fclose(f);
        }
    }
}