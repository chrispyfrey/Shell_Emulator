// Chris Frey
// 03/08/20

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_CMMND_LEN 512

void shell_path(char *[], const int, char *);
void strip_trailing(char *, const int);
void dispatch_command(char *, char *, char *[]);
int term_count(const char *, const int, const char);
void split(char *, const int, char *[], const char);

/////////////////////////////////
//            Main
/////////////////////////////////

int main() {
    const char *envp_ptr = getenv("PATH");
    
    if (envp_ptr != NULL) {
        int envp_len = strlen(envp_ptr);
        char envp_cpy[envp_len];
        int dir_num = term_count(envp_ptr, envp_len, ':');
        char *dirs[dir_num];
        char sh_pth[envp_len];
        
        strcpy(envp_cpy, envp_ptr);
        split(envp_cpy, envp_len, dirs, ':');
        shell_path(dirs, dir_num, sh_pth);

        if (sh_pth != NULL) {
            char envp[envp_len + 5];
            char cmmnd[MAX_CMMND_LEN];

            strcpy(envp, "PATH=");
            strcat(envp, envp_ptr);
            
            char *envp_arg[] = { envp, NULL };

            printf("Enter 'exit' to quit shell.\n");

            while (1) {
                printf("shell_emu > ");
                fgets(cmmnd, MAX_CMMND_LEN, stdin);

                int cmmnd_len = strlen(cmmnd);

                strip_trailing(cmmnd, cmmnd_len);

                if (strcmp(cmmnd, "exit") != 0) {
                    cmmnd_len = strlen(cmmnd);
                    int cmmnd_num = term_count(cmmnd, cmmnd_len, ' ');
                    char cmmnd_cpy[cmmnd_len];
                    char *cmmnd_array[cmmnd_num];
                    
                    strcpy(cmmnd_cpy, cmmnd);
                    split(cmmnd_cpy, cmmnd_len, cmmnd_array, ' ');

                    if (strcmp(cmmnd_array[0], "cd") == 0) {     
                        chdir(cmmnd_array[cmmnd_num-1]);
                    }
                    else if (fork() == 0) {
                        dispatch_command(sh_pth, cmmnd, envp_arg);
                    }
                }
                else {
                    break;
                }
                
                wait(NULL);
            }
        }
        else {
            printf("Error: Could not find shell path.\n");
        }
    }
    else {
        printf("Error: Could not read PATH variable.\n");
    } 
}

/////////////////////////////////
//     Function Definitions
/////////////////////////////////

void shell_path(char *dirs[], const int dir_num, char *sh_pth) {
    int fnd = 0;

    for (int i = 0; i < dir_num; ++i) {
        strcpy(sh_pth, dirs[i]);
        strcat(sh_pth, "/sh");

        if (access(sh_pth, F_OK) == 0) {
            fnd = 1;
            break;
        }
    }

    if (!fnd) {
        sh_pth = NULL;
    }
}

void strip_trailing(char *strng, const int strng_len) {
    for (int i = 0; i < strng_len; ++i) {
        if (strng[i] == '\n') {
            strng[i] = '\0';

            if (i-1 > 0 && strng[i-1] == ' ') {
                --i;

                while (strng[i] == ' ' && i-1 > -1) {
                    strng[i] = '\0';
                    --i;
                }
            }

            break;
        }
        else if (strng[i] == '\0') {
            break;
        }
    }
}

void dispatch_command(char *sh_pth, char *cmmnd, char *envp_arg[]) {
    char *args[] = { sh_pth, "-c", cmmnd, NULL };
    execve(sh_pth, args, envp_arg);
}

int term_count(const char *strng, const int strng_len, const char delim) {
    int term_num = 0;

    for (int i = 0; i < strng_len; ++i) {
        if (strng[i] == delim) {
            ++term_num;
        }
    }

    return term_num + 1;
}

void split(char *strng, const int strng_len, char *ptr_arr[], const char delim) {
    int j = 0;

    ptr_arr[j] = &strng[0];

    ++j;

    for (int i = 0; i < strng_len; ++i) {
        if (strng[i] == delim) {
            strng[i] = '\0';

            if (i+1 < strng_len) {
                ptr_arr[j] = &strng[i+1];
                ++j;
            }
        }
    }
}
