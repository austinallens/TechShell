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
    // If anything but 0 will initialize all the code I used
    // for testing.
    int logMode = 0;

    char* input;
    struct ShellCommand command;

    // Repeatedly Prompt the User for Input
    for(;;)
    {
        // Display the Prompt
        displayPrompt();

        // Get the User's Input
        input = getInput(); // Don't forget to use 'free(input);' once done,
                            // from inside the loop. Finish 'parseInput()' and
                            // 'executeCommand()' first to know exactly where to place.

        // Parse the Command Line
        command = parseInput(input);

        if (logMode != 0) {
            for (int i = 0; command.args[i] != NULL; i++) {
                printf("args[%d]: '%s'\n", i, command.args[i]);
            }
            printf("Output File: '%s'\n", command.outputFile);
            printf("Input File: '%s'\n", command.inputFile);
        }

        // Execute the Command
        executeCommand(command);
    }

    exit(0);
}

void displayPrompt() {
    /*
    A function that causes the prompt to display in the terminal
    */
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    printf("%s$ ", cwd);
}

char* getInput() {
    /*
    A function that takes input from the user.
    It may return return the input to the calling statement or store
    it at some memory location using a pointer.
    */
    char input[PATH_MAX];
    fgets(input, sizeof(input), stdin);
    char* output = strdup(input);
    return output;
}

struct ShellCommand parseInput(char* input) {
    /*
    A function that parses through the user input.
    Consider having this function return a struct that stores vital information about the parsed instruction such as:
        - The command itself
        - The arguments that come after the command
            Hint: When formatting your data, look into execvp and how it takes in args.
        - Information about if a redirect was detected such as >, <, or |
        - Information about whether or not a new file needs to be created and what that filename may be.

    Some helpful functions when doing this come from string.h and stdlib.h, such as strtok, strcmp, strcpy, calloc, malloc,
    realloc, free, and more.

    Be sure to consider/test for situations when a backslash is used to escape the space char and when quotes are used to group together
    various tokens.
    */

    input[strcspn(input, "\n")] = 0;

    int tokenAmount = 0;
    char *token = strtok(input, " ");

    struct ShellCommand cmd;
    cmd.inputFile = NULL;
    cmd.outputFile = NULL;

    while (token != NULL) {
        if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            cmd.outputFile = token;
        } else if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            cmd.inputFile = token;
        } else {
            cmd.args[tokenAmount] = token;
            tokenAmount++;
        }
        token = strtok(NULL, " ");
    }

    cmd.args[tokenAmount] = NULL;

    return cmd;
}

void executeCommand(struct ShellCommand command) {
    /*
    A function that executes the command.
    This function might take in a struct that represents the shell command.

    Be sure to consider each of the following:
        1. The execvp() function.
            This can execute commands that already exist, that is, you don't need to recreate
            the functionality of the commands on your computer, just the shell.
            Keep in mind that execvp takes over the current process.
        2. The fork() function.
            This can create a process for execvp to take over.
        3. 'cd' is not a command like 'ls' and 'mkdir'.
            'cd' is a tool provided by the shell,
            so you WILL need to recreate the functionality of cd.
        4. Be sure to handle standard output redirect and stadard input redirects here.
            That is, the symbols: > and <.
            Pipe isn't required, but could be a nice addition.
    */

    if (strcmp(command.args[0], "cd") == 0) {
        // Checks if command is 'cd'
        if(chdir(command.args[1]) != 0) {
            // Checks if 'chdir' fails (ie. doesn't return 0)
            // and returns perror
            printf("Error %d: (%s)\n", errno, strerror(errno));
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
            wait(NULL);
        }
    }
}