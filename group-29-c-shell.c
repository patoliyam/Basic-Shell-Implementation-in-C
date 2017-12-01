/*
=====================================================
Group - 29 
150101045 : Patoliya Meetkumar Krushnadas 
150101081 : Vaibhav Pratap Singh
150101054 : Saket SAnjay Agrawal
=====================================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>


#define TRUE 1
#define FALSE !TRUE
#define LIMIT 256 // max number of tokens for a command
#define MAXLINE 1024 // max number of characters from user input
#define MAXHIST 1000  //max number of history lines.
static char* currentDirectory;
extern char** environ;
int no_reprint_prmpt;
pid_t pid;

/**
 * SIGNAL HANDLERS
 */
// signal handler for SIGCHLD */
void signalHandler_child(int p);
// signal handler for SIGINT
void signalHandler_int(int p);
int changeDirectory(char * args[]);
int commandHandler(char * args[]);


/**
 * Method used to print the welcome screen of our shell
 */
void welcomeScreen(){
        printf("\n\t================================================\n");
        printf("\t          Group 29 : Basic Shell in C \n");
        printf("\t      Roll No.: 150101045,150101054,150101081 \n");
        printf("\t================================================\n");
        printf("\n\n");
}


/**
 * SIGNAL HANDLERS
 */

/**
 * Signal handler for SIGALRM
 */
void alarmHandler(int p){
	// We send a SIGTERM signal to the child process
	kill(getpid(),SIGTERM);		
}
/**
 * Signal handler for SIGINT
 */
void signalHandler_int(int p){
	// We send a SIGTERM signal to the child process
	if (kill(pid,SIGTERM) == 0){
		printf("\nProcess %d received a SIGINT signal\n",pid);
		no_reprint_prmpt = 1;			
	}else{
		printf("\n");
	}
}
/**
 * signal handler for SIGCHLD
 */
void signalHandler_child(int p){
	/* Wait for all dead processes.
	 * We use a non-blocking call (WNOHANG) to be sure this signal handler will not
	 * block if a child was cleaned up in another part of the program. */
	while (waitpid(-1, NULL, WNOHANG) > 0) {
	}
	printf("\n");
}

/**
 *	Displays the prompt for the shell
 */
void shellPrompt(){
	// We print the prompt in the form "<user>@<host> <cwd> >"
	char hostn[1204] = "";
	gethostname(hostn, sizeof(hostn));
	printf("%s@%s %s > ", getenv("LOGNAME"), hostn, getcwd(currentDirectory, 1024));
}

/**
 * Method to change directory
 */
int changeDirectory(char* args[]){
	// If we write no path (only 'cd'), then go to the home directory
	if (args[1] == NULL) {
		chdir(getenv("HOME")); 
		return 1;
	}
	// Else we change the directory to the one specified by the 
	// argument, if possible
	else{ 
		if (chdir(args[1]) == -1) {
			printf(" %s: no such directory\n", args[1]);
            return -1;
		}
	}
	return 0;
}

/**
 * Method used to manage the environment variables with different
 * options
 */ 
int manageEnviron(char * args[], int option){
	char **env_aux;
	switch(option){
		// Case 'environ': we print the environment variables along with
		// their values
		case 0: 
			for(env_aux = environ; *env_aux != 0; env_aux ++){
				printf("%s\n", *env_aux);
			}
			break;
		// Case 'setenv': we set an environment variable to a value
		case 1: 
			if((args[1] == NULL) && args[2] == NULL){
				printf("%s","Not enought input arguments\n");
				return -1;
			}
			
			// We use different output for new and overwritten variables
			if(getenv(args[1]) != NULL){
				printf("%s", "The variable has been overwritten\n");
			}else{
				printf("%s", "The variable has been created\n");
			}
			
			// If we specify no value for the variable, we set it to ""
			if (args[2] == NULL){
				setenv(args[1], "", 1);
			// We set the variable to the given value
			}else{
				setenv(args[1], args[2], 1);
			}
			break;
		// Case 'unsetenv': we delete an environment variable
		case 2:
			if(args[1] == NULL){
				printf("%s","Not enought input arguments\n");
				return -1;
			}
			if(getenv(args[1]) != NULL){
				unsetenv(args[1]);
				printf("%s", "The variable has been erased\n");
			}else{
				printf("%s", "The variable does not exist\n");
			}
		break;
			
			
	}
	return 0;
}
 
 /**
 * Checks whether entered charecter is digit or not.
 */
int isDigit(char *s)
{
	int i;
	for (i = 0; s[i]!=NULL; ++i)
		if((int)(s[i]) <48 || (int)(s[i]) >57) return 0;
	return 1;
}


/**
* Method for launching a program. It can be run in the background
* or in the foreground
*/ 
void launchProg(char **args, int background){	 
	 int err = -1;
	 
	 if((pid=fork())==-1){
		 printf("Child process could not be created\n");
		 return;
	 }
	 // pid == 0 implies the following code is related to the child process
	if(pid==0){
		// We set the child to ignore SIGINT signals (we want the parent
		// process to handle them with signalHandler_int)	
		int i=0;
		while(args[i]!=NULL) i++;
		i--;
		//Checking whether last argument is digit or ont.
		if(isDigit(args[i])) 
		{
			alarm(atoi(args[i]));
			signal(SIGALRM, alarmHandler);
			args[i]=NULL;
		}

		signal(SIGINT, SIG_IGN);
		
		// We set parent=<pathname>/simple-c-shell as an environment variable
		// for the child
		setenv("parent",getcwd(currentDirectory, 1024),1);	
		
		// If we launch non-existing commands we end the process
		if (execvp(args[0],args)==err){
			printf("Command not found");
			kill(getpid(),SIGTERM);
		}
	 }
	 // The following will be executed by the parent
	 
	 // If the process is not requested to be in background, we wait for
	 // the child to finish.
	 if (background == 0){
		 waitpid(pid,NULL,0);
	 }else{
		 // In order to create a background process, the current process
		 // should just skip the call to wait. The SIGCHILD handler
		 // signalHandler_child will take care of the returning values
		 // of the childs.
		 printf("Process created with PID: %d\n",pid);
	 }	 
}
 
/**
* Method used to manage I/O redirection
*/ 
void fileIO(char * args[], char* inputFile, char* outputFile, int option){
	 
	int err = -1;
	
	int fileDescriptor; // between 0 and 19, describing the output or input file
	
	if((pid=fork())==-1){
		printf("Child process could not be created\n");
		return;
	}
	if(pid==0){
		// Option 0: output redirection
		if (option == 0){
			// We open (create) the file truncating it at 0, for write only
			fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600); 
			// We replace de standard output with the appropriate file
			dup2(fileDescriptor, STDOUT_FILENO); 
			close(fileDescriptor);
		// Option 1: input and output redirection
		}else if (option == 1){
			// We open file for read only (it's STDIN)
			fileDescriptor = open(inputFile, O_RDONLY, 0600);  
			// We replace de standard input with the appropriate file
			dup2(fileDescriptor, STDIN_FILENO);
			close(fileDescriptor);
			// Same as before for the output file
			fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			dup2(fileDescriptor, STDOUT_FILENO);
			close(fileDescriptor);		 
		}
		// Option 2: input redirection
		else if (option == 2){
			// We open file for read only (it's STDIN)
			fileDescriptor = open(inputFile, O_RDONLY, 0600);  
			// We replace de standard input with the appropriate file
			dup2(fileDescriptor, STDIN_FILENO);
			close(fileDescriptor);
		}
		 
		setenv("parent",getcwd(currentDirectory, 1024),1);
		
		if (execvp(args[0],args)==err){
			printf("err");
			kill(getpid(),SIGTERM);
		}		 
	}

	waitpid(pid,NULL,0);
}


char * his[MAXHIST];
int recent_command_nos;
//This command will print recent commands. If 
int history(char **args)
{
  if (args[1] == NULL || !isDigit(args[1])) {
	printf("invalid argument: expected argument to \"history\"\n");
  } else {
	int cur_cmd = recent_command_nos;
	int argument = atoi(args[1]);
	int x2 = (recent_command_nos-argument);
	if( argument > recent_command_nos){
		//that is if unsufficient number of commands are there then it will print all commands available.
		x2 = 0;
	}
	int i = 0;
	for (i = x2; i < cur_cmd && i >= 0 ; ++i){
		printf("%s\n",his[i]);
	}  	
 }
  return 1;
}


int issue(char **args)
{
	  if (args[1] == NULL || *args[1] == '0') {
		fprintf(stderr, "invalid argument: expected argument to \"issue\"\n");
	  } 
	  else {
		int argument = atoi(args[1]);
		if( argument > recent_command_nos){
			fprintf(stderr, "Insufficient Number of Commands Entered\n" );
			return 1;
		}
		char linecopy[MAXLINE];
		strcpy(linecopy,his[argument-1]);
		printf("Executing %s\n",his[argument-1]);
		char * tokens[LIMIT];
		int numTokens = 0;
		tokens[0] = strtok(linecopy," \n\t");
		numTokens = 1;
		while((tokens[numTokens] = strtok(NULL, " \n\t")) != NULL) numTokens++;
		//tokenized command is passed to command handler
		commandHandler(tokens);
	 }
  return 1;
}

/**
* Method used to handle the commands entered via the standard input
*/ 

int removeExcept(char **args)
{
	if((pid=fork())==-1){
		 printf("Child process could not be created\n");
		 return 1;
	 }
	 // pid == 0 implies the following code is related to the child process
	if(pid==0){
		// // We set the child to ignore SIGINT signals (we want the parent
		// // process to handle them with signalHandler_int)	
		signal(SIGINT, SIG_IGN);
		
		// // We set parent=<pathname>/simple-c-shell as an environment variable
		// // for the child
		setenv("parent",getcwd(currentDirectory, 1024),1);	

		//making find command by appending name of files not to be deleted
		int i =0;
		char  find_command [1024] = "find . ";
		for(i=1;args[i]!=NULL;i++)
		{
			char  tail [25] = " ! -name ";
			strcat(tail, args[i]);
			strcat(find_command, tail);
		}
		char final_tail[] = " -type f -exec rm -f {} +\0";
		strcat(find_command, final_tail );
		//find_command is now complete command to be executed so need to be tokenized and then handled.
		int numTokens = 0;
		args[0] = strtok(find_command," \n\t");
		numTokens = 1;
		while((args[numTokens] = strtok(NULL, " \n\t")) != NULL) numTokens++;
		//tokenized command is passed to command handler
		commandHandler(args);
	 }
	 
	
  return 1;
}
/**
* Method used to handle the commands entered via the standard input
*/ 

int commandHandler(char * args[]){
	int i = 0;
	int j = 0;
	
	int fileDescriptor;
	int standardOut;
	
	int aux;
	int background = 0;
	
	char *args_aux[256];
	
	// We look for the special characters and separate the command itself
	// in a new array for the arguments
	while ( args[j] != NULL){
		if ( (strcmp(args[j],">") == 0) || (strcmp(args[j],"<") == 0) || (strcmp(args[j],"&") == 0)){
			break;
		}
		args_aux[j] = args[j];
		j++;
	}
	
	// 'exit' command quits the shell
	if(strcmp(args[0],"exit") == 0) exit(0);
	// 'pwd' command prints the current directory
 	else if (strcmp(args[0],"pwd") == 0){
		if (args[j] != NULL){
			// If we want file output
			if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) ){
				fileDescriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
				// We replace de standard output with the appropriate file
				standardOut = dup(STDOUT_FILENO); 	// first we make a copy of stdout
													// because we'll want it back
				dup2(fileDescriptor, STDOUT_FILENO); 
				close(fileDescriptor);
				printf("%s\n", getcwd(currentDirectory, 1024));
				dup2(standardOut, STDOUT_FILENO);
			}
		}else{
			printf("%s\n", getcwd(currentDirectory, 1024));
		}
	} 
	else if (strcmp(args[0],"history") == 0){
		if (args[j] != NULL){
			// If we want file output
			if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) ){
				fileDescriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
				// We replace de standard output with the appropriate file
				standardOut = dup(STDOUT_FILENO); 	// first we make a copy of stdout
													// because we'll want it back
				dup2(fileDescriptor, STDOUT_FILENO); 
				close(fileDescriptor);
				history(args);
				dup2(standardOut, STDOUT_FILENO);
			}
		}else{
			history(args);
		}
	}
	else if (strcmp(args[0],"issue") == 0){
		if (args[j] != NULL){
			// If we want file output
			if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) ){
				fileDescriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
				// We replace de standard output with the appropriate file
				standardOut = dup(STDOUT_FILENO); 	// first we make a copy of stdout
													// because we'll want it back
				dup2(fileDescriptor, STDOUT_FILENO); 
				close(fileDescriptor);
				issue(args);
				dup2(standardOut, STDOUT_FILENO);
			}
		}else{
			issue(args);
		}
	}
 	// 'clear' command clears the screen
	else if (strcmp(args[0],"clear") == 0) system("clear");
	//'rmexec' command deletes files except some files
	else if (strcmp(args[0],"rmexcept") == 0) removeExcept(args);
	// 'cd' command to change directory
	else if (strcmp(args[0],"cd") == 0) changeDirectory(args);
	// 'environ' command to list the environment variables
	else if (strcmp(args[0],"environ") == 0){
		if (args[j] != NULL){
			// If we want file output
			if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) ){
				fileDescriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600); 
				// We replace de standard output with the appropriate file
				standardOut = dup(STDOUT_FILENO); 	// first we make a copy of stdout
													// because we'll want it back
				dup2(fileDescriptor, STDOUT_FILENO); 
				close(fileDescriptor);
				manageEnviron(args,0);
				dup2(standardOut, STDOUT_FILENO);
			}
		}else{
			manageEnviron(args,0);
		}
	}
	// 'setenv' command to set environment variables
	else if (strcmp(args[0],"setenv") == 0) manageEnviron(args,1);
	// 'unsetenv' command to undefine environment variables
	else if (strcmp(args[0],"unsetenv") == 0) manageEnviron(args,2);
	else{
		// If none of the preceding commands were used, we invoke the
		// specified program. We have to detect if I/O redirection,
		// piped execution or background execution were solicited
		while (args[i] != NULL && background == 0){
			// If background execution was solicited (last argument '&')
			// we exit the loop
			if (strcmp(args[i],"&") == 0){
				background = 1;
			// If '<' is detected, we have Input and Output redirection.
			// First we check if the structure given is the correct one,
			// and if that is the case we call the appropriate method
			}else if (strcmp(args[i],"<") == 0){
				aux = i+1;
				if(args[aux] != NULL && args[aux+1] == NULL)
				{
					fileIO(args_aux,args[i+1],NULL,2);
					return 1;
				}
				else if(args[aux] != NULL && (strcmp(args[aux+1],">") != 0))
				{
					fileIO(args_aux,args[i+1],NULL,2);
					return 1;
				}
				else if (args[aux] == NULL || args[aux+1] == NULL || args[aux+2] == NULL ){
					printf("Not enough input arguments\n");
					return -1;
				}else{
					if (strcmp(args[aux+1],">") != 0){
						printf("Usage: Expected '>' and found %s\n",args[aux+1]);
						return -2;
					}
				}
				fileIO(args_aux,args[i+1],args[i+3],1);
				return 1;
			}
			// If '>' is detected, we have output redirection.
			// First we check if the structure given is the correct one,
			// and if that is the case we call the appropriate method
			else if (strcmp(args[i],">") == 0){
				if (args[i+1] == NULL){
					printf("Not enough input arguments\n");
					return -1;
				}
				fileIO(args_aux,NULL,args[i+1],0);
				return 1;
			}
			i++;
		}
		// We launch the program with our method, indicating if we
		// want background execution or not
		args_aux[i] = NULL;
		launchProg(args_aux,background);
	
	}
return 1;
}


/**
* Main method of our shell
*/ 
int main(int argc, char *argv[], char ** envp) {
	char line[MAXLINE]; // buffer for the user input
	char * tokens[LIMIT]; // array for the different tokens in the command
	int numTokens;
	char linecopy[MAXLINE];
		
	no_reprint_prmpt = 0; 	// to prevent the printing of the shell
							// after certain methods
	pid = -10; // we initialize pid to an pid that is not possible
	
	// We call the method of initialization and the welcome screen
	currentDirectory = (char*) calloc(1024, sizeof(char));
	signal(SIGCHLD, signalHandler_child);
	signal(SIGINT, signalHandler_int);
	welcomeScreen();
    
    // We set our extern char** environ to the environment, so that
    // we can treat it later in other methods

	environ = envp;
	memset(his, ' ', sizeof(his));
	// We set shell=<pathname>/simple-c-shell as an environment variable for
	// the child
	setenv("shell",getcwd(currentDirectory, 1024),1);
	
	// Main loop, where the user input will be read and the prompt
	// will be printed
	int command_nos = 0;

	while(TRUE){
		// We print the shell prompt if necessary
		if (no_reprint_prmpt == 0) shellPrompt();
		no_reprint_prmpt = 0;
		
		// We empty the line buffer
		memset ( line, '\0', MAXLINE );

		// We wait for user input
		fgets(line, MAXLINE, stdin);
	
		strcpy(linecopy,line);

		// If nothing is written, the loop is executed again
		if((tokens[0] = strtok(line," \n\t")) == NULL) continue;//
		
		// We read all the tokens of the input and pass it to our
		// commandHandler as the argument
		numTokens = 1;
		while((tokens[numTokens] = strtok(NULL, " \n\t")) != NULL) numTokens++;
		
		//storing in history, we wont store "issue" and "history" command in history
		if(strcmp(tokens[0],"history")!=0 && strcmp(tokens[0],"issue")!=0)
		{ 
			
			his[command_nos] = (char*) malloc(MAXLINE*sizeof(char));
			strcpy(his[command_nos] , linecopy);
			command_nos = (command_nos+1)%MAXHIST;
			recent_command_nos = command_nos;
		}
		commandHandler(tokens);
		
	}          

	exit(0);
}
