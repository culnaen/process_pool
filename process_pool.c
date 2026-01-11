#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ_END 0
#define WRITE_END 1
#define WORKERS 5

int main() {
    int pipe_in[WORKERS][2]; 
    int pipe_out[WORKERS][2];
    pid_t pid;

    for (int i = 0; i < WORKERS; i++) {
        pipe(pipe_in[i]);
        pipe(pipe_out[i]);

        pid = fork();
        if (pid == 0) {
            // child
            char buf[256];
            ssize_t n;

            for (int j = 0; j < WORKERS; j++) {
                if (j != i) {
                    close(pipe_in[j][READ_END]);
                    close(pipe_in[j][WRITE_END]);
                    close(pipe_out[j][READ_END]);
                    close(pipe_out[j][WRITE_END]);
                }
            }

            close(pipe_in[i][WRITE_END]);
            close(pipe_out[i][READ_END]);

            dup2(pipe_in[i][READ_END], STDIN_FILENO);
            dup2(pipe_out[i][WRITE_END], STDOUT_FILENO);

            close(pipe_in[i][READ_END]);
            close(pipe_out[i][WRITE_END]);

            while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
                write(STDOUT_FILENO, buf, n);
            }

            exit(10 + i);
        }
    }

    // parent
    for (int i = 0; i < WORKERS; i++) {
        close(pipe_in[i][READ_END]);
        close(pipe_out[i][WRITE_END]);
    }

    for (int i = 0; i < WORKERS; i++) {
        char msg[64];
        snprintf(msg, sizeof(msg), "worker %d says hello", i);
        write(pipe_in[i][WRITE_END], msg, strlen(msg));
        close(pipe_in[i][WRITE_END]);
    }

    for (int i = 0; i < WORKERS; i++) {
        char buf[256];
        ssize_t n;

        while ((n = read(pipe_out[i][READ_END], buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            printf("from worker %d: %s\n", i, buf);
        }
        close(pipe_out[i][READ_END]);
    }

    // ждём всех
    while (wait(NULL) > 0);

    return 0;
}
