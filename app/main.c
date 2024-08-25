#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

const char COMMANDS[][100] = {"echo","type","exit", "pwd"};

int checkCommandExists(char command[100]);
int findPath(char file[],char *result);
void programExecution(char *path, char *args[]);

int main(){
  
  char input[100];
  char result[100];

  while(true){
      printf("$ ");
      fflush(stdout);
      fgets(input, 100, stdin);
      input[strlen(input) - 1] = '\0';

    if (strcmp(input,"exit 0") == 0){
      exit(0);
    }

    if(strncmp(input,"echo",strlen("echo")) == 0){
      printf("%s\n",input + 5);
      continue;
    }
    if(strncmp(input,"type",strlen("type")) == 0){
      int numberOfcommands = sizeof(COMMANDS)/sizeof(COMMANDS[0]);
      int found = 0;
      
      for(int i = 0; i < numberOfcommands; i++){
        if(strcmp(input + 5, COMMANDS[i]) == 0){
          printf("%s is a shell builtin\n", input + 5);
          found = 1;
          break;
        }
      }
      if(!found){
        if(findPath(input +5,result) == 0){
          printf("%s is %s\n", input+5,result);
        }
        else if(findPath(input + 5, result) == 2){
          printf("PATH variable not set\n");
        }
        else{
          printf("%s: not found\n", input +5);
        }
      }
      continue;
    }
    if(strcmp(input, "pwd") == 0){
      char cwd[500];
      if(getcwd(cwd, sizeof(cwd)) != NULL){
        printf("%s\n", cwd);
      }
      else{
        printf("getcwd error");
      }
      continue;
    }
    if(strncmp(input, "cd", strlen("cd")) == 0){
      char *path[500];
      int count = 0;
      char *dir = strtok(input," ");
      
      while(dir != NULL){
        path[count++] = dir;
        dir = strtok(NULL, " ");
      }
      if(strcmp(path[1], "~") == 0){
        char *homeVari = getenv("HOME");

        if(homeVari == NULL){
          printf("HOME variable not set.");
        }
        else{
           chdir(homeVari) == 0 ?  : printf("cd: %s: No such file or directory\n", path[1]) ;
        }
      }
      else if (path[1] != "~" && chdir(path[1]) == 0){
        ;
      }
      else{
        printf("cd: %s: No such file or directory\n", path[1]) ;
      }
      continue;
    }
    else{
      char *args[10];
      int argsCounter = 0;

      char *token = strtok(input, " ");

      while (token != NULL && argsCounter < 10){
        args[argsCounter++] = token;
        token = strtok(NULL, " ");
      }
      args[argsCounter] = NULL; 

      if(findPath(args[0], result) == 0){
        programExecution(result, args);
      }
      else{
        printf("%s: command not found\n", input);
      }
    }
  }
  return 0;
}

int isExecutable(char *path){
 return access(path, X_OK) ;
}

int findPath(char file[],char *result){
    char *path = getenv("PATH");
    if(path == NULL){
      return 2;
    }

    char pathCopy[1024];
    strncpy(pathCopy,path,sizeof(pathCopy) -1);
    pathCopy[sizeof(pathCopy) -1 ] = '\0';
    char *dir  = strtok(pathCopy, ":");
    int found = 0;  

    while(dir != NULL){
      char fullPath[500];
      snprintf(fullPath, sizeof(fullPath), "%s/%s",dir,file);
      if(isExecutable(fullPath) == 0){
        snprintf(result,100,"%s",fullPath);
        found = 1;
        return 0;
        break;
      }
      dir = strtok(NULL, ":");
    }
    if(!found){
      return 1;
    }
}

void programExecution(char *path, char *args[]){
  int pid = fork();
  if(pid == 0){
    execv(path, args);
    perror("execv");
    exit(1);
  }
  else if(pid < 0){
    perror("fork");
  }
  else{
    int status;
    waitpid(pid, &status, 0);
  }
}
