#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



#define MAX_LINE 256

void welcome(){
  printf("\e[35m>>>Welcome to my shell\e[0m\n");
  printf("\e[35m>>>This is a simaple UNIX shell\e[0m\n");
  printf("\e[35m>>>For more information, plz contact WHX. Have a good time! :)\e[0m\n");
}


void get_command(char *buf){
  int len = 0;
  int ch;
  ch = getchar();
  while (len < 256 && ch != '\n'){
    buf[len++] = ch;
    ch = getchar();
  }
  if (len == 256){
    printf("%s\n", "Command is too long");
    exit(-1);
  }

  buf[len] = '\n';
  len++;
  buf[len] = '\0';
}

void explain_command(char* buf, int *argcount, char arglist[100][256]){
  char *p = buf;
  char *q = buf;
  int number = 0;
  while (1){
    if (p[0] == '\n')
      break;
    if (p[0] == ' ')
      p++;
    else{
      q = p;
      number = 0;
      while (q[0] != ' ' && q[0] != '\n'){
        number++;
        q++;
      }
      strncpy(arglist[*argcount], p, number+1);
      arglist[*argcount][number] = '\0';
      *argcount = *argcount+1;
      p = q;
    }
  }
}

void my_redirect(int argcount, char* arg[]){
  int i, j;
  pid_t pid;
  int fd;
  char filename[MAX_LINE];
  for (i = 1; i < argcount; ++i){
    if (strcmp(arg[i], ">") == 0 || strcmp(arg[i], "<") == 0){
      if (arg[i+1] == NULL){
        perror("Syntax error: File name expected\n");
      }
      else{
        strcpy(filename, arg[i+1]);
      }
      if (strcmp(arg[i], ">") == 0){
        arg[i] = NULL;
        pid = fork();
        if (pid == 0){
          fd = open(filename, O_RDWR|O_CREAT|O_TRUNC, 0644);
          if (fd < 0){
            perror("File opening failed.\n");
            exit(1);
          }
          dup2(fd, 1);
          execvp(arg[0], arg);
        }
        else if (pid > 0){
          if (strcmp(arg[argcount-1], "&") != 0)
            waitpid(pid, NULL, 0);
        }
        else{
          perror("Fork failed.\n");
          exit(1);
        }
        close(fd);
      }
      else{
        arg[i] = NULL;
        pid = fork();
        if (pid == 0){
          fd = open(filename, O_RDONLY);
          if (fd < 0){
            perror("File opening failed.\n");
            exit(1);
          }
          dup2(fd, 0);
          execvp(arg[0], arg);
        }
        else if (pid > 0){
          if (strcmp(arg[argcount-1], "&") != 0)
            waitpid(pid, NULL, 0);
        }
        else{
          perror("Fork failed.\n");
          exit(1);
        }
        close(fd);
      }
      return;
    }
  }
}


void my_pipe(int argcount, char* arg[]){
  int i, j;
  pid_t pid;

  char* argnext[argcount+1];
  for (i = 1; i < argcount; ++i){
    if (strcmp(arg[i], "|") == 0){
      arg[i] = NULL;
      for (j = i + 1; arg[j] != NULL; ++j){
        argnext[j-i-1] = arg[j];
      }
      argnext[j-i-1] = arg[j];
      break;
    }
  }

  pid = fork();
  int pipefd[2];
  int result = pipe(pipefd);
  if (result == -1){
    perror("Pipe failed.\n");
    exit(-1);
  }

  if (pid < 0){
    perror("Fork failed.\n");
    exit(-1);
  }
  else if (pid == 0){
    pid_t pid2;
    pid2 = fork();
    if (pid2 < 0){
      perror("Fork failed.\n");
      exit(-1);
    }
    else if (pid2 == 0){
      close(pipefd[0]);
      dup2(pipefd[1], 1);
      execvp(arg[0], arg);
    }

    waitpid(pid2, NULL, 0);
    close(pipefd[1]);
    dup2(pipefd[0], 0);
    execvp(argnext[0], argnext);

  }
  else{
    if (strcmp(arg[argcount-1], "&") != 0)
      waitpid(pid, NULL, 0);
  }
}


char arglist[100][256];
char arg_tmp[100][256];
char *buf = NULL;
int argcount = 0, argcount_previous = 0;

int main(void){
  // Initialize the interface
  welcome();
  // Initialize some parameters
  int should_run = 1;
  int i;


  buf = (char *)malloc(256);
  if (buf == NULL){
    perror("malloc failed");
    exit(-1);
  }
  char *arg_previous[MAX_LINE/2+1];
  arg_previous[0] = NULL;
  pid_t pid;

  while (should_run) {
    // Get command input
    memset(buf, 0, 256);
    printf("osh> ");
    //fflush(stdout);
    get_command(buf);
    char* arg[argcount+1];
    // Exit from the shell
    if (strcmp(buf, "exit\n") == 0)
      break;
    for (i = 0; i < 100; ++i)
      arglist[i][0] = '\0';
    argcount = 0;
    explain_command(buf, &argcount, arglist);

    for (i = 0; i < argcount; ++i)
      arg[i] = (char *) arglist[i];
    arg[argcount] = NULL;

    // Implement the history feature
    if (strcmp(arg[0], "!!") == 0){
      int k = 0;
      //for (k; k < argcount_previous; ++k)
      //  printf("%s\n", arg_previous[k]);
      //printf("%d\n", argcount_previous);
      pid = fork();
      if (pid < 0){
        perror("Fork failed.\n");
      }
      else if (pid == 0){
        if (arg_previous[0] == NULL){
          perror("No previous command.\n");
          break;
        }
        int flag = 0;
        for (i = 1; i < argcount_previous; ++i)
          if (strcmp(arg_previous[i], ">") == 0 || strcmp(arg_previous[i], "<") == 0){
            my_redirect(argcount_previous, arg_previous);
            flag++;
          }
        for (i = 1; i < argcount_previous; ++i)
          if (strcmp(arg_previous[i], "|") == 0){
            my_pipe(argcount_previous, arg_previous);
            flag++;
          }
        if (flag == 0)
          execvp(arg_previous[0], arg_previous);
      }
      else{
        if (strcmp(arg_previous[argcount_previous-1], "&") != 0)
          waitpid(pid, NULL, 0);
      }
    }
    else{
      int number = 0;
      if (strcmp(arg[0], "!!") != 0){
        for (i = 0; i < argcount; ++i){
          int length = strlen(arg[i]);
          strncpy(arg_tmp[i], arg[i], length+1);
          arg_previous[i] = (char*) arg_tmp[i];
        }
        argcount_previous = argcount;
        for (i = argcount; i < MAX_LINE/2 + 1; ++i)
          arg_previous[i] = NULL;
      }

      pid = fork();
      if (pid < 0){
        perror("Fork failed.\n");
      }
      else if (pid == 0){
        int flag = 0;
        for (i = 1; i < argcount; ++i)
          if (strcmp(arg[i], ">") == 0 || strcmp(arg[i], "<") == 0){
            my_redirect(argcount, arg);
            flag++;
          }
        for (i = 1; i < argcount; ++i)
          if (strcmp(arg[i], "|") == 0){
            my_pipe(argcount, arg);
            flag++;
          }
        if (flag == 0){
          if (strcmp(arg[argcount-1], "&") == 0)
            arg[argcount-1] = NULL;
          execvp(arg[0], arg);
          arg[argcount-1] = "&";
        }
      }
      else {
        if (strcmp(arg[argcount-1], "&") != 0)
          waitpid(pid, NULL, 0);
      }
    }

  }
  return 0;
}
