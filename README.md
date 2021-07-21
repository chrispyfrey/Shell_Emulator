# Shell_Emulator

This program emulates a shell by using the getenv() function to verify that a "sh" executable is present on the system, then calling the execve() function with the "sh" executable path and user input.

With the exception of "cd", every user command is executed through a child process using the fork() function. Current working directory is a process-unique variable and may not be modified from outside of a given process.
