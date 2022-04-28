#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
void myPrint(char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}
int empty(char *cmd){
  for (int i = 0; i < strlen(cmd); i++){
    if (cmd[i] != ' ' && cmd[i] != '\t' && cmd[i] != '\n' && cmd[i] != '\0' && cmd[i] != '\r' && cmd[i] != '\v' && cmd[i] != '\f'){
      return 0;
    }
  }
  return 1;
}
void basicCommand(char *subtoken, char *savep2){  
  if (strcmp(subtoken, "exit") == 0){
    if (strtok_r(NULL, " \t\n\r\v\f", &savep2) != NULL){
      char error_message[30] = "An error has occurred\n";
      write(STDOUT_FILENO, error_message, strlen(error_message));
    }
    else{
      exit(0);
    }
  }
  else if (strcmp(subtoken, "cd") == 0){
    subtoken = strtok_r(NULL, " \t\n", &savep2);
    if (subtoken == NULL){
        chdir(getenv("HOME"));
    }
    else{
      char dir[512];
      strcpy(dir, subtoken);
      subtoken = strtok_r(NULL, " \t\n", &savep2);
      if (chdir(dir) != 0 || subtoken != NULL){
        char error_message[30] = "An error has occurred\n";
        write(STDOUT_FILENO, error_message, strlen(error_message));
      }
    }
  }
  else if (strcmp(subtoken, "pwd") == 0){
    char dir[512];
    subtoken = strtok_r(NULL, " \t\n", &savep2);
    if (getcwd(dir, sizeof(dir)) == NULL || subtoken != NULL){
      char error_message[30] = "An error has occurred\n";
      write(STDOUT_FILENO, error_message, strlen(error_message));
    }
    else{
      myPrint(dir);
      myPrint("\n");
    }
  }
  return;
}
void advredirect(char *token, char **execArgs){
  char *subtoken3, *subtoken4;
  char *savep3, *savep4;
  subtoken3 = strtok_r(token, ">", &savep3);
  if (subtoken3 == NULL || empty(subtoken3) == 1 || subtoken3[0] == '+'){
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
    return;
  }
  if (strcmp(subtoken3, "cd") == 0 || strcmp(subtoken3, "exit") == 0 || strcmp(subtoken3, "pwd") == 0){
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
    return;
  }
  subtoken4 = strtok_r(subtoken3, " \t\n\r\v\f", &savep4);
  int i = 0;
  while (subtoken4 != NULL){
    execArgs[i] = subtoken4;
    subtoken4 = strtok_r(NULL, " \t\n\r\v\f", &savep4);
    i++;
  }
  execArgs[i] = NULL;
  subtoken3 = strtok_r(NULL, ">", &savep3);
  subtoken3 = subtoken3 + 1;
  if (subtoken3 == NULL || strtok_r(NULL, ">", &savep3) != NULL){
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
    return;
  }
  subtoken4 = strtok_r(subtoken3, " \t\n\r\v\f", &savep4);
  if (subtoken4 == NULL || strtok_r(NULL, " \t\n", &savep4) != NULL){
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
  }
  else{
    int fd1 = open(subtoken4, O_RDWR | O_CREAT, 0777);
    if (fd1 == -1){
      char error_message[30] = "An error has occurred\n";
      write(STDOUT_FILENO, error_message, strlen(error_message));
      return;
    }
    int fd2 = open("temp.txt", O_RDWR | O_CREAT, 0777);
    if (fd2 == -1){
      char error_message[30] = "An error has occurred\n";
      write(STDOUT_FILENO, error_message, strlen(error_message));
      return;
    }
    char buff[512];
    int status = 0;
    int pid = fork();
    int count = 0;
    if (pid == 0){
      while ((count = read(fd1, buff, 512)) > 0){
      write(fd2, buff, count);
      }
      lseek(fd1, 0, SEEK_SET);
      dup2(fd1, STDOUT_FILENO);
      execvp(execArgs[0], execArgs);
      char error_message[30] = "An error has occurred\n";
      write(STDOUT_FILENO, error_message, strlen(error_message));
      exit(0);
    }
    else{
      while(wait(&status) > 0){
      }
    }
    lseek(fd2, 0, SEEK_SET);
    lseek(fd1, 0, SEEK_CUR);
    while ((count = read(fd2, buff, 512)) > 0){
      write(fd1, buff, count);
    }
    close(fd1);
    close(fd2);
    remove("temp.txt");
  }
  return;
}
void redirect(char *token, char **execArgs){
  char *subtoken3, *subtoken4;
  char *savep3, *savep4;
  subtoken3 = strtok_r(token, ">", &savep3);
  if (strcmp(subtoken3, "cd") == 0 || strcmp(subtoken3, "exit") == 0 || strcmp(subtoken3, "pwd") == 0){
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
    return;
  }
  subtoken4 = strtok_r(subtoken3, " \t\n\r\v\f", &savep4);
  int i = 0;
  while (subtoken4 != NULL){
    execArgs[i] = subtoken4;
    subtoken4 = strtok_r(NULL, " \t\n\r\v\f", &savep4);
    i++;
  }
  execArgs[i] = NULL;
  subtoken3 = strtok_r(NULL, ">", &savep3);
  if (subtoken3 == NULL || strtok_r(NULL, ">", &savep3) != NULL){
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
    return;
  }
  subtoken4 = strtok_r(subtoken3, " \t\n\r\v\f", &savep4);
  if (subtoken4 == NULL || strtok_r(NULL, " \t\n", &savep4) != NULL){
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
  }
  else if (access(subtoken4, F_OK) != -1){
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
  }
  else{
    int fd = open(subtoken4, O_RDWR | O_CREAT, 0777);
    if (fd == -1){
      char error_message[30] = "An error has occurred\n";
      write(STDOUT_FILENO, error_message, strlen(error_message));
    }
    else{
      dup2(fd, STDOUT_FILENO);
      execvp(execArgs[0], execArgs);
      char error_message[30] = "An error has occurred\n";
      write(STDOUT_FILENO, error_message, strlen(error_message));
      exit(0);
    }
    close(fd);
  }
  return;
}
void parse(char *commands){
  char *savep1, *savep2;
  char *token = strtok_r(commands, ";", &savep1);
  int redir = 0;
  char redirToken[512];
  while (token != NULL){
    if (empty(token) == 1){
    }
    else{
      if (strstr(token, ">+") != NULL){
        redir = 1;
        strcpy(redirToken, token);
      }
      else if (strchr(token, '>') != NULL){
        redir = 2;
        strcpy(redirToken, token);
      }
      char* subtoken = strtok_r(token, " \t\n\r\v\f", &savep2);
      if (strcmp(subtoken, "exit") == 0 || strcmp(subtoken, "cd") == 0 || strcmp(subtoken, "pwd") == 0){
        basicCommand(subtoken, savep2);
      }
      else{
        int status = 0;
        char **execArgs = malloc(sizeof(char *) * sizeof(commands));
        int i = 0;
        while (subtoken != NULL){
          execArgs[i] = malloc(sizeof(char) * sizeof(commands));
          execArgs[i] = subtoken;
          subtoken = strtok_r(NULL, " \t\n\r\v\f", &savep2);
          i++;
        }
        execArgs[i] = NULL;
        int pid = fork();
        if (pid == 0){
          if (redir == 1){
            advredirect(redirToken, execArgs);
            exit(0);
          }
          else if (redir == 2){
            redirect(redirToken, execArgs);
            exit(0);
          }
          else{
            execvp(execArgs[0], execArgs);
            char error_message[30] = "An error has occurred\n";
            write(STDOUT_FILENO, error_message, strlen(error_message));
            exit(0);
          }
        }
        else{
          while(wait(&status) > 0){
          }
        }
        free(execArgs);
      }
    }
    token = strtok_r(NULL, ";", &savep1);
  }
  return;
}
int main(int argc, char *argv[]) {
    char cmd_buff[514];
    char *pinput;
    char *file = argv[1];
    FILE *fp = fopen(file, "r");
    if (fp == NULL){
      char error_message[30] = "An error has occurred\n";
      write(STDOUT_FILENO, error_message, strlen(error_message));
      return 0;
    }
    if (argc > 2){
      char error_message[30] = "An error has occurred\n";
      write(STDOUT_FILENO, error_message, strlen(error_message));
      return 0;
    }
    while (1) {
      if (argc == 1){
        pinput = fgets(cmd_buff, 514, stdin);
      }
      else{
        pinput = fgets(cmd_buff, 514, fp);
      }
      if (!pinput) {
        exit(0);
      }
      if (pinput == NULL){
        break;
      }
      if (strlen(cmd_buff) > 512){
        myPrint(cmd_buff);
        while (cmd_buff[strlen(cmd_buff) -1] != '\n'){
          fgets(cmd_buff, 514, fp);
          myPrint(cmd_buff);
        }
        char error_message[30] = "An error has occurred\n";
        write(STDOUT_FILENO, error_message, strlen(error_message));
      }
      else if (empty(cmd_buff) == 1){
      }
      else{
        myPrint(cmd_buff);
        parse(cmd_buff);
      }
    }
    fclose(fp);
    return 0;
}
