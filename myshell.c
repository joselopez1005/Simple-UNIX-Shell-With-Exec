/* myshell.c 
 *
 *
 * The program is responsible for simulating the core parts of a bash shell found within the Unix environment. 
 * It will acomplish this using several important system calls including fork(), execvp() and wait(). 
 * The program will first obtain input from the user and tokenize it. Those tokens will then be stored in an array. 
 * The program will then create a child process using fork() system call. The child process will be responsible for 
 * executing the execvp() function with the necessary arguments. The child process code will then be replaced. 
 * This process will be repeated until exit becomes input and the program will terminate and print out the number of commands.
 * A special conidition within the program is wether a process will be background of foreground. The program tackles this 
 * condition by seeing the last token and finding the key character "&", If found then print PID of Child to the screen and 
 * no wait, but ask for the next command. 
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

const int MAXSIZE = 256;   //  Maximum Size of Command and Arguments

int GetCmdTokenize(char *cmd, int length, char **args);
void ExecCommand(char **args, int argCount);
int IsBackground(char **args, int argCount);

int main (void){
  char cmd[MAXSIZE];     //  Holds command to be executed
  char* args[MAXSIZE];   //  Holds command and its arguments
  int commandCount = 0;  //  Variable to be incremented after execution of command
  
  // Iterate until user types "exit" Will call on functions responsible for tokenizing command
  // and execution of command.  
  do{
    int argCount = GetCmdTokenize(cmd, sizeof(cmd), args);  //  Return number of arguments. 
    ExecCommand(args, argCount);  //Executes command
    commandCount++;
  }while(strcmp(args[0], "exit") != 0);
  //  Execution after user types "exit"
  printf("Number of Commands: %d\n", --commandCount);
  return 0;
}

/*
 * Function: GetCmdTokenize
 * ------------------------
 * Obtains the bash command from the user using fgets
 * After Obtaining the command, it will procceed to tokenize it using strtok
 * strtok will seperate the command using space as a delimiter
 * 
 * cmd: Char pointer to the char array made in main. Will hold contents of bash command
 * length: Holds the length of Char array cmd found in main
 * args: Array of strings that hold the cmd and its arguments
 *
 * returns: Returns the amount of arguments, this includes the command and its arguments
 */

int GetCmdTokenize(char *cmd, int length, char **args){
  int count = 0;  //  Variable to be incremented after finding an argument
  
  fgets(cmd, length, stdin);
  
  strtok(cmd, "\n");  //  Using \n as first delimeter to remove it from the string
  
  char *token = strtok(cmd, " ");  //  Using " " as second delimter to correctcly seperate the command and its arguments
  
  // Iterate until strtok has processed every part of the command. 
  // Occurs when no token has been found which = NULL 
  while(token != NULL) {
    args[count] = token;  //  Stores token into array of arguments
    count++;    
    token = strtok(NULL, " ");
  } 
  args[count] = NULL;  //NULL terminate the argument for execvp
  return count;
}

/*
 * Function: ExecCommand
 * ---------------------
 * Executes the command as a child process using the fork system call
 * The actual execution will occur by using the execvp() system call. 
 * execvp() will find any commands that are already in the PATH. 
 * Additionally, it will print out the PID of the child process if a "&" is found
 * 
 * args: Array of strings that hold the command and all of its arguments
 * argCount: Variable containing the amount of arguments
 */
 
void ExecCommand(char **args, int argCount){
  int pid;  //  Will hold the return value of the fork() system call
  int status;
  int background = IsBackground(args, argCount);  //  Will store 1 if background process or 0 if foreground process
  
  //  If statement that executes if process is background. 
  if(background == 1 && strcmp(args[0], "exit") != 0){
    pid = fork();
    if(pid == 0){    //  Child Process
      if(execvp(args[0],args) == -1){  //  If execvp() fails, then the command does not exist. Notify user. 
        printf("Error: Command Does Not Exist\n");
        exit(0);
      }
    }
    else{            //  Parent Process
      printf("Process ID of Child: %d\n", pid);
    }
  }
  
  //  If statement that executes if process is foreground
  else if(strcmp(args[0], "exit") != 0){
    pid = fork();
    if(pid == 0){    //  Child Process
      if(execvp(args[0],args) == -1){  //  If execvp() fails, then the command does not exist. Notify user. 
        printf("Error: Command Does Not Exist\n");
        exit(0);
      }
    }
    else{            //  Parent Process
    
      //  Want parent to wait for child to finish executing before continuing our shell program
      //  waitpid will return information about the child process to make sure it finished executing
      //  correctcly. This information is sotred in &status. Since simple program, options are not needed.
      int w = waitpid(pid, &status, 0);  
      if (w == -1) {
             perror("waitpid");
             exit(EXIT_FAILURE);
       }    
    }
  }
}

/*
 * Function: IsBackground
 * ----------------------
 * Determines wether the given command is background or foreground. 
 * It accomplishes this by looking at the last character of the array of strings. 
 * If it is background, the program replaces the character with a null terminator.
 *
 * args: Array of strings that hold the command and all of its arguments
 * argCount: Variable containing the amount of arguments
 *
 * Return: Will return 1 if command is a background process otherwise will return 0 if foreground. 
 */
int IsBackground(char **args, int argCount){
  if(args[argCount-1][strlen(args[argCount-1]+1)] == '&'){  //  Last character of the last argument
    args[argCount-1][strlen(args[argCount-1]+1)] = '\0';    //  Replace with null terminator
    return 1;
  }
  return 0;
}



