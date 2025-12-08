#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>


#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 100


int main() {
    char *message = "worker";
    int pipefd_in[2]; // in_parent
    int pipefd_out[2]; // out_parent
    int status;
    pid_t pid;

    pipe(pipefd_in);
    pipe(pipefd_out);

    pid = fork();
    if (pid == 0) {
        // child
        char buf[256];
        ssize_t n;
        close(pipefd_out[WRITE_END]);
        close(pipefd_in[READ_END]);

        dup2(pipefd_out[READ_END], STDIN_FILENO);
        dup2(pipefd_in[WRITE_END], STDOUT_FILENO);

        close(pipefd_out[READ_END]);
        close(pipefd_in[WRITE_END]);

        while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
            write(STDOUT_FILENO, buf, n);
        }
        
        exit(17);

    } else {
        // parent
        close(pipefd_out[READ_END]);
        close(pipefd_in[WRITE_END]);
        
        write(pipefd_out[WRITE_END], message, strlen(message));
        close(pipefd_out[WRITE_END]);
        char buf[256];
        ssize_t n;
        while ((n = read(pipefd_in[READ_END], buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            printf("%s\n", buf);
        }


        close(pipefd_in[READ_END]);

        waitpid(-1, &status, WNOHANG);
    }

}