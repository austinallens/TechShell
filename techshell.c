// Name: Austin Allen
// Description: Main File for the Techshell project.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 64
#define PATH_MAX 256

struct ShellCommand
{
    char* args[MAX_ARGS];
    char* inputFile;
    char* outputFile;
};


void displayPrompt();
char* getInput();
struct ShellCommand parseInput(char* input);
void executeCommand(struct ShellCommand command);

int main() // MAIN
{
    // Used to store character array from getInput()
    char* input;
    
    // Used to store ShellCommand struct from parseInput()
    struct ShellCommand command;

    // Repeatedly Prompt the User for Input
    for(;;)
    {
        // Display the Prompt
        displayPrompt();

        // Get the User's Input
        input = getInput();

        // Parse the Command Line
        command = parseInput(input);

        // Debug
        #ifdef DEBUG
            for (int i = 0; command.args[i] != NULL; i++) {
                printf("args[%d]: '%s'\n", i, command.args[i]);
            }
            printf("Output File: '%s'\n", command.outputFile);
            printf("Input File: '%s'\n", command.inputFile);
        #endif

        // Execute the Command
        executeCommand(command);

        // Frees input from memory
        // (since strdup doesn't automatically)
        free(input);
    }

    // Exits with error (Shouldn't need to run)
    exit(1);
}

void displayPrompt() {
    /*
    A function that causes the prompt to display in the terminal
    */

    // Gets 'home' directory
    // ie. '/home/USER'
    char* home;
    home = getenv("HOME");

    // Debug
    #ifdef DEBUG
    printf("Home path: %s\n", home);
    #endif

    // Gets current working directory
    // ie. '/home/USER/Desktop'
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    /* Prints:
          ~: Represents 'home' directory
         %s: Current working directory moved forward the
             length of 'home'
          $: So you know its done ig
    */
    printf("~%s$ ", cwd+strlen(home));
}

char* getInput() {
    /*
    A function that takes input from the user.
    */

    // Character Array to store user input (up to PATH_MAX in size)
    char input[PATH_MAX];

    // Gets user input from keyboard (stdin) and puts it in input up to
    // the size of input (with a \n)
    fgets(input, sizeof(input), stdin);

    // Creates a copy of 'input' called 'output' to return.
    // This is because 'input' would get cleared from mem.
    // once the function finishes. Now 'output' persists.
    char* output = strdup(input);
    return output;
}

struct ShellCommand parseInput(char* input) {
    /*
    A function that parses through the user input.
    See 'tokenizer.c' assignment for another usage of this.
    */

    // Removes '\n' from input (added in strdup)
    input[strcspn(input, "\n")] = 0;

    // tokenAmount is to iterate through array
    int tokenAmount = 0;
    char *token = strtok(input, " ");

    // Creates ShellCommand struct with
    // initial values.
    struct ShellCommand cmd;
    cmd.inputFile = NULL;
    cmd.outputFile = NULL;

    // As long as token doesn't equal 'NULL'
    // which is the end of token
    while (token != NULL) {
        if (strcmp(token, ">") == 0) {
            // If the token is > then,
            // adds what comes after to 'outputFile'
            token = strtok(NULL, " ");
            cmd.outputFile = token;
        } else if (strcmp(token, "<") == 0) {
            // If the token is < then,
            // adds what comes after to 'inputFile'
            token = strtok(NULL, " ");
            cmd.inputFile = token;
        } else {
            // Otherwise, adds token to args array
            // and adds one to 'tokenAmount'
            cmd.args[tokenAmount] = token;
            tokenAmount++;
        }
        // Breaks if not outside of the if elses.
        // So that output/inputFile doesn't get added
        // as a token
        token = strtok(NULL, " ");
    }

    // 'execvp' requires a 'NULL' at the end
    cmd.args[tokenAmount] = NULL;

    return cmd;
}

void executeCommand(struct ShellCommand command) {
    /*
    A function that executes the command.
    */

    if (strcmp(command.args[0], "cd") == 0) {
        // Checks if command is 'cd'
        if(chdir(command.args[1]) != 0) {
            // Runs 'chdir' and checks if it fails 
            // (ie. doesn't return 0) and returns perror
            printf("Error %d (%s)\n", errno, strerror(errno));
        }
    } else if (strcmp(command.args[0], "exit") == 0) {
        // Checks if command is 'exit' and exits
        exit(0);
    } else {
        // If there is no 'cd' then it forks and
        // runs execvp()

        pid_t pid = fork();

        if (pid == 0) {
            // Child Process:
            // This process holds the execvp() so that
            // it won't affect the bash terminal (Parent Process)

            if (command.outputFile != NULL) {
                int fd = open(command.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("Open output file failed.");
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            if (command.inputFile != NULL) {
                int fd = open(command.inputFile, O_RDONLY);
                if (fd < 0) {
                    perror("Open input file failed.");
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            execvp(command.args[0], command.args);
            printf("Error %d (%s)\n", errno, strerror(errno));
            exit(1);
        } else {
            // Parent Process:
            // waits until Child is brutally slaughtered
            wait(NULL);
        }
    }
}