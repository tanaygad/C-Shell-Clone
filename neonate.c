#include "functions.h"

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

/**
 * Enable row mode for the terminal
 * The ECHO feature causes each key you type to be printed to the terminal, so you can see what you’re typing.
 * Terminal attributes can be read into a termios struct by tcgetattr().
 * After modifying them, you can then apply them to the terminal using tcsetattr().
 * The TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read.
 * The c_lflag field is for “local flags”
 */
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

/**
 * stdout and stdin are buffered we disable buffering on that
 * After entering in raw mode we read characters one by one
 * Up arrow keys and down arrow keys are represented by 3 byte escape codes
 * starting with ascii number 27 i.e. ESC key
 * This way we interpret arrow keys
 * Tabs are usually handled by the term, but here we are simulating tabs for the sake of simplicity
 * Backspace move the cursor one control character to the left
 * @return
 */
int run_neonate(int time_interval)
{
    //while (1) printf("%d\n", time_interval);
    char *inp = malloc(sizeof(char) * 100);
    char c;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    pid_t pid = fork();
    if (pid == 0)
    {
        int k=1;
        while (1)
        {
            gettimeofday(&end, NULL);
            int elapsed = (end.tv_sec - start.tv_sec) ;
            if (elapsed == k*time_interval)
            {
                k++;
                // printf("%d\n", elapsed);
                // printf("%d\n", k);
                FILE *f = fopen("/proc/loadavg", "r");
                char buffer[1024];
                fgets(buffer, 1024, f);
                int j = 0, count = 0;
                while (j < 1024)
                {
                    if (buffer[j] == ' ')
                        count++;
                    j++;
                    if (count == 4)
                        break;
                }
                while (j < 1024)
                {
                    if (buffer[j] == ' ' || buffer[j] == '\n')
                        break;
                    printf("%c", buffer[j]);
                    j++;
                }
                printf("\n");
            }
        }
    }
    else
    {
        while (1)
        {
            setbuf(stdout, NULL);
            enableRawMode();
            // printf("Prompt>");
            memset(inp, '\0', 100);
            int pt = 0;
            while (read(STDIN_FILENO, &c, 1) == 1)
            {
                if (iscntrl(c))
                {
                    if (c == 10)
                        break;
                    else if (c == 27)
                    {
                        char buf[3];
                        buf[2] = 0;
                        if (read(STDIN_FILENO, buf, 2) == 2)
                        { // length of escape code
                            printf("\rarrow key: %s", buf);
                        }
                    }
                    else if (c == 127)
                    { // backspace
                        if (pt > 0)
                        {
                            if (inp[pt - 1] == 9)
                            {
                                for (int i = 0; i < 7; i++)
                                {
                                    printf("\b");
                                }
                            }
                            inp[--pt] = '\0';
                            printf("\b \b");
                        }
                    }
                    else if (c == 9)
                    { // TAB character
                        inp[pt++] = c;
                        for (int i = 0; i < 8; i++)
                        { // TABS should be 8 spaces
                            printf(" ");
                        }
                    }
                    else if (c == 4)
                    {
                        exit(0);
                    }
                    else
                    {
                        printf("%d\n", c);
                    }
                }
                else if (c == 'x'){
                    printf("\n");    
                    kill(pid,SIGKILL);
                    disableRawMode();
                    return 0;
                }
                else
                {
                    inp[pt++] = c;
                    printf("%c", c);
                }
            }
            disableRawMode();

            // printf("\nInput Read: [%s]\n", inp);
        }
    }
    return 0;
}