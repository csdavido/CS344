/************************************************************************************************************
** AUTHOR: DAVID RIDER
** PROGRAM NAME: smallsh.c
** CLASS: CS 344
** DATE: 11/20/2019
** DESCRIPTION: Small shell program implemented in C
** CITATION: https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf
*************************************************************************************************************/
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//****************FUNCTION DECLARATIONS****************//
//***SIGNAL HANDLING***//
void catchCTRLC_parent();
void catchCTRLC_child(int signal);
void catchCTRLZ(int signal);
//***REDIRECTION***//
void fileIN(char* fileName);
void fileOUT(char* fileName);
//***COMMANDS***//
void getStatus();
void changeDir(char** args);
//***UTILITIES***//
char* changePIDString(const char* _string, const char* old, const char* new);
void killZombie();

//****************GLOBALS****************//
//TODO: CHANGE SOME OF THIS SHIT
char* input = NULL;
char* output = NULL;
//STDN FLAGS
int inputFlag;
int outputFlag;
//PROCESS HANDLING
int backPID = 0;
int foreFlag = 0;
int backFlag = 0;
int cProcess_exit; 

//****************MAIN****************//
int main() {
    //***VARIABLES***//
    //FLAG FOR EXIT CONDITION
    int exitFlag = 1;
    // ARRAY TO HOLD ARGUMENTS
    char** args; 
    //***SIGNAL CONTROL***//
    //CATCH CTRL-C
    signal(SIGINT, catchCTRLC_parent);
    // catch CRTL-Z signal
    signal(SIGTSTP, catchCTRLZ);


    //LOOP UNTIL EXIT CONDITION IS MET
    while (exitFlag) {
        //RESIZE ARGUMENTS TO ALLOW FOR MAXIMUM # OF ARGUMENTS
        args = malloc(sizeof(char*) * 512);
        //STORE USER INPUT
        char* userInput = NULL;
        //SET FLAGS TO ZERO
        inputFlag = 0;
        outputFlag = 0;
        //CATCH CTRLC
        signal(SIGINT, catchCTRLC_parent);
        //PROMPT
        printf(":");
        //GET USER INPUT
        ssize_t inputLength = 0; 
        getline(&userInput, &inputLength, stdin);
        //FLUSH STDOUT
        fflush(stdout); 
        //CURRENT ARGUMENT
        char** curArgArray = malloc(sizeof(char*) * 512);
        char* curArg; 
        curArg = strtok(userInput, " \n");
        //ITERATOR
        int i = 0;
        //LOOP THROUGH ARGUMENTS
        while(curArg) {
            if (curArg == NULL) {
                args = curArgArray;
                break;
            }
            else if (strcmp(curArg, "<") == 0) {
                //CURRENT ARGUMENT
                curArg = strtok(NULL, " \n");
                //IF THERE IS AN ARGUMENT
                if (curArg != NULL) {
                    //FLIP INPUT FLAG
                    inputFlag = 1; 
                    //RESIZE INPUT VARIBLE
                    input = malloc(sizeof(char) * strlen(curArg));
                    //COPY INPUT
                    strcpy(input, curArg);
                }
            }
            else if (strcmp(curArg, ">") == 0) {
                //CURRENT ARGUMENT
                curArg = strtok(NULL, " \n");
                //IF THERE IS AN ARGUMENT
                if (curArg != NULL) {
                    //FLIP OUTPUT FLAG
                    outputFlag = 1;
                    //RESIZE OUTPUT VARIABLE
                    output = malloc(sizeof(char) * strlen(curArg));
                    //COPY OUTPUT
                    strcpy(output, curArg);
                } else {
                    args = curArgArray;
                    break;
                }
            }
            else if (strcmp(curArg, "&") == 0) {
                //FLIP BACKGROUND FLAG
                backFlag = 1;
                //FOREGROUND MODE
                if (foreFlag == 1) {
                    //FLIP BACKGROUND FLAG
                    backFlag = 0;
                }
            }
            else if (strstr(curArg, "$$") != NULL) {
                //GET CURRENT PID
                pid_t currentPID = getpid();
                //RESIZE PID
                char* pID = malloc(sizeof(char) * 512);
                sprintf(pID, "%d", currentPID);
                //CONVERT PID TO STRING
                pID = changePIDString(curArg, "$$", pID);
                //ADD PID TO ARGUMENTS ARRAY
                curArgArray[i] = pID;
                //INCREMENT ITERATOR
                i++;
            }
            //ANY ADITIONAL COMMAND
            else {
                //ADD CURRENT ARGUMENT TO THE ARGUMENT ARRAY
                curArgArray[i] = curArg;
                //INCREMENT ITERATOR
                i++;
            }
            //CLEAR curArg FOR THE NEXT ARGUMENT
        args = curArgArray;
        curArg = strtok(NULL, " \n");

        }
        //***EXECUTE ARGUMENTS***//
        //NO ARGUMENTS
        if (args[0] == NULL) {
            //CONTINUE LOOP
            exitFlag = 1;
        }
        //COMMENTS
        else if (strstr(args[0], "#") != NULL) {
            //DO NOTHING
            //CONTINUE LOOP
            exitFlag = 1;
        }
        //EXIT
        else if (strcmp(args[0], "exit") == 0) {
            //FLIP EXIT FLAG
            exitFlag = 0;
        }
        //CHANGE DIRECTORY
        else if (strcmp(args[0], "cd") == 0) {
            //CALL changeDir
            changeDir(args);
            //CONTINUE LOOP
            exitFlag = 1;
        }
        //STATUS
        else if (strcmp(args[0], "status") == 0) {
            //SET BACKGROUND FLAG
            backFlag = 0;
            //GET STATUS
            getStatus();
            //CONTINUE LOOP
            exitFlag = 1;
        }
        //OTHER FUNCTIONS
        else {
            //SET TO VALUE BELOW -1
            pid_t newPID = -3;
            cProcess_exit = -3;

            newPID = fork();
            if (newPID == -1) {
                perror("Error forking!");
                break;
            }
            else if (newPID == 0) {
                //INPUT REDIRECTION
                if (inputFlag == 1) {
                    fileIN(input);
                }
                //OUPUT REDIRECTION
                if (outputFlag == 1) {
                    fileOUT(output);
                }
                //ERROR & EXIT
                if (execvp(args[0], args) < 0) {
                    perror(args[0]);
                    exit(1);
                }
            } 
            else {
                if (backFlag == 1) {
                    //PRINT BACKGROUND PID
                    printf("background pid is %d\n", newPID);
                    backPID = newPID;
                    backFlag = 0;
                } else {
                    //CTL-C INTERUPTION
                    signal(SIGINT, catchCTRLC_child);
                    waitpid(newPID, &cProcess_exit, 0);
                }
            }
            exitFlag = 1;
        }
        //KILL ZOMBIE PROCESSES
        killZombie();
    }

    return 0;

}
//****************FUNCTION IMPLEMENTATIONS****************//
//***SIGNAL HANDLING***//
/*************************************************************************
** FUNCTION NAME: catchCTRLC_parent
** PARAMENTERS: none
** PRECONDITIONS: none
** POSTCONDITIONS: CTRL-C is ignored
** CITATION: https://stackoverflow.com/questions/4217037/catch-ctrl-c-in-c
**************************************************************************/
void catchCTRLC_parent() {

    //DO NOTHING
    return;

}
/*************************************************************************
** FUNCTION NAME: catchCTRLC_child
** PARAMENTERS: an integer
** PRECONDITIONS: none
** POSTCONDITIONS: CTRLC is handled in child process
** CITATION: https://stackoverflow.com/questions/4217037/catch-ctrl-c-in-c
**************************************************************************/
void catchCTRLC_child(int signal) {
    //GET TERMINATING SIGNAL
    int termSignal = WTERMSIG(signal);
    //PRINT MESSAGE
    printf("terminated by signal %d\n", termSignal);

}
/***************************************************
** FUNCTION NAME: catchCTRLZ
** PARAMENTERS: an integer
** PRECONDITIONS: none
** POSTCONDITIONS: foreground-only mode is switched
***************************************************/
void catchCTRLZ(int signal) {
        //IF ALREADY IN FOREGROUND
        if(foreFlag == 1) {
            //FLIP FOREGROUND FLAG
            foreFlag = 0;
            //PRINT MESSAGE
            printf("\nExiting foreground-only mode\n:");
        }
        //IF NOT IN FOREGROUND
        else if (foreFlag == 0) {
            //FLIP FOREGROUND FLAG
            foreFlag = 1;
            //PRINT MESSAGE
            printf("\nEntering foreground-only mode (& is now ignored)\n:");
        }
}
//***REDIRECTION***//
/************************************************
** FUNCTION NAME: fileIN
** PARAMENTERS: a file name
** PRECONDITIONS: none
** POSTCONDITIONS: stdIN handled
** CITATION: https://stackoverflow.com/a/11518304
*************************************************/
void fileIN(char* fileName) {
    //OPEN FILE WITH READ ONLY MODE
    int inFile = open(fileName, O_RDONLY);
    //IF ERROR
    if (inFile == -1) {
        //PRINT ERROR MESSAGE WITH FILE NAME
        printf("cannot open %s for input\n", fileName);
        //EXIT
        exit(1);
    }
    //DUPLICATE FILE DESCRIPTOR
    int dupFile = dup2(inFile, 0);
    //IF ERROR
    if (dupFile < 0) {
        //PRINT MESSAGE
        perror("File error!");
    }
    //CLOSE FILE
    close(inFile);

}
/************************************************
** FUNCTION NAME: fileOUT
** PARAMENTERS: a file name
** PRECONDITIONS: none
** POSTCONDITIONS: stdOUT handled
** CITATION: https://stackoverflow.com/a/11518304
*************************************************/
void fileOUT(char* fileName) {
    //OPEN FILE, CREATE IF DOESN'T EXIST WITH WRITE ONLY 
    //AND SET PERMISSIONS
    int outFile = open(fileName, O_CREAT | O_WRONLY, 0777);
    //IF ERROR
    if (outFile == -1) {
        //PRINT MESSAGE
        perror("Error opening file!");
    }
    //DUPLICATE FILE DESCRIPTOR
    int dupFile = dup2(outFile, 1);
    //IF ERROR
    if (dupFile < 0) {
        //PRINT ERROR
        perror("File error!");
    }
    //CLOSE FILE
    close(outFile);

}
//***COMMANDS***//
/**************************************************************
** FUNCTION NAME: getStatus
** PARAMENTERS: none
** PRECONDITIONS: none
** POSTCONDITIONS: exit value or termination signal is printed
**************************************************************/
void getStatus() {
    //CHECK EXITED STATUS
    if (WIFEXITED(cProcess_exit) != 0) {
        //GET EXIT STATUS
        int exit = WEXITSTATUS(cProcess_exit);
        //PRINT MESSAGE
        printf("exit value %d\n", exit);
    }
    //IF SIGNALED WAS RAISED TO TERMINATE
    else if (WIFSIGNALED(cProcess_exit) != 0) {
        //TERMINATE 
        int terminate = WTERMSIG(cProcess_exit);
        //PRINT MESSAGE
        printf("terminated by signal %d\n", terminate);
    }

}
/*************************************************
** FUNCTION NAME: changeDir
** PARAMENTERS: pointer to argument array
** PRECONDITIONS: none
** POSTCONDITIONS: cd procedes or error is thrown
*************************************************/
void changeDir(char** args) {
        //IF THERE IS A DIRECTORY SPECIFIED
        if (args[2] != NULL) {
            //PROCEDE WITH CD
            return;
        } else {
            //IF NO ARGUMENT
            if (args[1] == NULL) {
                //NAVIGATE TO USER'S HOME
                args[1] = getenv("HOME");
            }
            //IF ERROR   
            if (chdir(args[1]) != 0) {
                //PRINT ERROR
                perror("cd error");
            }
        }

}
//***UTILITIES***//
/*****************************************************************************************
** FUNCTION NAME: changePIDString
** PARAMENTERS: 3 const char*s: _string, old, and new
** PRECONDITIONS: pID converted to a string
** POSTCONDITIONS: pID is adjusted
** CITATION: https://www.geeksforgeeks.org/c-program-replace-word-text-another-given-word/
******************************************************************************************/
char* changePIDString(const char* _string, const char* old, const char* new) {
    //KEEP TRACK OF OLD WORDS
    int oldWords = 0;
    //LENGTH OF NEW STRING
    int newLength = strlen(new);
    //LENGTH OF OLD STRING
    int oldLength = strlen(old);
    //RETURN VALUE
    char *dirName; 
    //LOOP THROUGH STRING
    int x;
    for (x = 0; _string[x] != '\0'; x++) {
        if (strstr(&_string[x], old) == &_string[x]) {
            //INCREMENT
            oldWords++;
            x += oldLength;
        }
    }
    //MAKE NEW STRING FOR DIRECTORY NAME
    dirName = malloc(sizeof(char) * (x + oldWords * (newLength - oldLength)));

    int y = 0;
    while (*_string) {
        //COMPARE SUBTRING
        if (strstr(_string, old) == _string) {
            //COPY THE STRING
            strcpy(&dirName[y], new);
            //INCREMENT STRING LENGTHS
            y += newLength;
            _string += oldLength;
        }
        else
            //SET VALUE
            dirName[y++] = *_string++;
        }
    //ADD \0 TO THE END OF THE STRING
    dirName[y] = '\0';
    //RETURN DIRECTORY NAME
    return dirName;

}
/***************************************************
** FUNCTION NAME: killZombie
** PARAMENTERS: none
** PRECONDITIONS: end of main while loop
** POSTCONDITIONS: completed processes are killed
** CITATION: //https://stackoverflow.com/a/28459548
***************************************************/
void killZombie() {
        //COMPLETED PID
        pid_t fin_pid = 0;
        fin_pid = waitpid(-1, &cProcess_exit, WNOHANG);
        //IF NEGATIVE
        if (fin_pid == -1) {
            //DO NOTHING
            return;
        }
        //IF PROCESS IS COMPLETE
        if (fin_pid != 0) {
            //PRINT MESSAGE
            printf("background pid %d is done: ", backPID);
            //GET STATUS
            getStatus();
            //FLUSH STDOUT
            fflush(stdout);
        }

}