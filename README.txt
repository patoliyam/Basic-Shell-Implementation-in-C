CS 341 : Operating Systems
Assignment 1 : Shell Implementation Using C
Submission Deadline: September 30, 2017 
=====================================================
Group - 29 
150101045 : Patoliya Meetkumar Krushnadas 
150101081 : Vaibhav Pratap Singh
150101054 : Saket SAnjay Agrawal
=====================================================
		

		

1) Basic commands: 
The following commands are Implemented using the execvp system call in child process except the 'cd' command which is implemented using the 'chdir' system call. These Programs are invoked by forking child process:

	a) cd <Directory_name> :- 
	- Changes the Directory, from the Current Directory to the Path Given as <Directory_name>. 
	- This command has been implemented using the chdir system call where <Directory_name> is passed to it as parameter. 
	- The Function by which this command is implemented is 'changeDirectory'
	
	b) ls :- 
	List all the Files and Folders in the Present Current Directory. 
	The Function by which this command is implemented is 'launchProg'
	
	c) rm {-f,-v,-r} <file_names> :-
	- -f : Removes the files forcefully(-f) 
	- -r : Remove folders and their contents recursively (-r) 
	- -v : Remove files/folders with verbose option on, i.e. all deletion actions are explained.
	- The Function in which this command is implemented is 'launchProg'
	
	d) history n :- 
	- Prints the most recent n commands issued by the user. If n is larger than total number of commands issued by user then it will print all commands issued by the user. 
	- Note that we haven't included previous "history n" and "issue i" commands in the history to avoid any looping of commands.
	- To implement this command we store all the previously issued command in array of strings and print once the command(history x) has been issued. We do not store this command into the previously issued commands. 
	-we have only stored MAXHIST number of history of recent commands in history array
	-We will overwrite oldest commands once all history list becomes full. 
	-The Function by which this command is implemented is 'history'

	e) issue n :-
	- Issues the nth command in the history once again. 
	- Since we already have stored all the previously issued commands as described above, so in order to implement this command we'll just get the nth command from the history array and provide it to command handler to execute. 
	- The Function by which this command is implemented is 'issue'
	
	f) <program_name> :- 
	- Creates a child process to run <program_name>. 
	- Supports the redirection operators '>' and '<' to redirect the input and output of the program to indicated files.
		Ex - ls > out.txt, ./a.out < in.txt > out.txt, etc
	- We have implemented this using 'dup' and 'dup2' commands.
	- The Function by which this command is implemented is 'fileIO' when the input or output is redirected, but in case there is no input or output redirection the command will get implemented using 'launchprog'

	g) exit :- 
	- Exits the shell. This command is executed using the exit system call. 
		Ex - exit(0);
	h) pwd :-
	- It prints current working directory of shell.
	

2) Some Advanced Commands Implementations:
	a) rmexcept <file_names> :- 
	- This command Remove all the Files in the current directory except the ones which are in the list <file_names>. 
	- To implement this command we have formed a string of this form ==> "find . ! -name 'file1' ! -name 'file2' -type f -exec rm -f {} +" --with the help of command issued and then executed it. 
	- The function in which this command is implemented is removeExcept()

	b) <program_name> m :- 
	- This command creates a child process to execute program_name, but aborts the process if it does not complete its operation in m seconds.
	- We have made an alarm signal which is invoked after m seconds and it will generate a SIGALARM if process isn't already over and will kill it.
	- Ex - ping 192.168.0.1 10 :- The command terminates after pinging for 10 secs
	- This function is implemented by adding alarm in "launchProg" function.


