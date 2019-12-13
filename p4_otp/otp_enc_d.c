/****************************************************
** AUTHOR: DAVID RIDER
** PROGRAM NAME: otp_enc_d.c
** CLASS: CS 344
** DATE: 12/07/2019
** DESCRIPTION: 
** CITATION: 
**      server.c
**      Beej's Guide to Network Programming
****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
//***********************GLOBALS***********************//
#define BUFF_SIZE 80000           
//****************FUNCTION DECLARATIONS****************//
void AuthArgs(int arg);
void CheckBind(int bnd);
void CheckFeed(int newFeed);
void Encrypt(char msg[], char key[], int len);
char ConvertInt(int _int);
int ConvertChar(char _char);
//************************MAIN************************//
int main(int argc, char* argv[]) {                    
    //***LOCAL VARIABLES***
    int sFeed;
    int new_sFeed;                   
    int portNum;                                
    struct sockaddr_in cAddress;   
    struct sockaddr_in sAddress;
    socklen_t cSize;   
    pid_t pid;
    char buffer[BUFF_SIZE];
    //AUTHENTICATE NUMBER OF ARGS
    AuthArgs(argc);
    //GET PORT NUMBER
    portNum = atoi(argv[1]);
    //SET UP FEED
    sFeed = socket(AF_INET, SOCK_STREAM, 0);
    //CHECK FEED
    if(sFeed < 0) {
        //PRINT ERROR MESSAGE
        perror("Error! Can't open socket...\n");
        //EXIT
        exit(1);
    }
    //SET SOCKET OPTIONS
    int optval = 1; 
    setsockopt(sFeed, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    //CLEAN SERVER ADDRESS
    bzero((char *) &sAddress, sizeof(sAddress));                 
    //MAKE SOCKET                    
    sAddress.sin_family = AF_INET;  
    //GET PORT NUMBER                   
    sAddress.sin_port = htons(portNum);
    //GET ADDRESS             
    sAddress.sin_addr.s_addr = INADDR_ANY;             
    //BIND 
    int _bind = bind(sFeed, (struct sockaddr *) &sAddress, sizeof(sAddress));
    CheckBind(_bind);
    //LISTEN ON SOCKET
    listen(sFeed, 5);       
    //HANDLE CHILD PROCESSES
    while(1) {
        //GET CLIENT ADDRESS SIZE
        cSize = sizeof(cAddress);
        //GET NEW FEED
        new_sFeed = accept(sFeed, (struct sockaddr *) &cAddress, &cSize); 
        //CHECK FEED 
        CheckFeed(new_sFeed);
        //FORK
        pid = fork();
        //IF FORK IS SUCCESSFUL
        if(pid == 0) {
            //CLEAR BUFFER
            bzero(buffer, sizeof(buffer)); 
            //KEY 
            char* ky;
            //BUFFER DATA
            int buffData = sizeof(buffer);
            int readData = 0;
            //READ BUFFER
            read(new_sFeed, buffer, sizeof(buffer) - 1); 
            //IF MESSAGE IS "e"     
            if(strcmp(buffer, "e") == 0){
                //WRITE MESSAGE
                char authMsg[] = "e";
                write(new_sFeed, authMsg, sizeof(authMsg));
            }
            //IF MESSAGE IS NOT "e" 
            if (strcmp(buffer, "e") != 0) {
                //WRITE ERROR MESSAGE
                char authMsg[] = "not valid";
                write(new_sFeed, authMsg, sizeof(authMsg));
                //EXIT
                exit(2);
            }
            //ZERO OUT BUFFER
            bzero(buffer, sizeof(buffer));
            //MAKE TEMPORARY BUFFER                 
            char* tempBuff = buffer;
            //LOOP THROUGH DATA
            int fwd = 0;
            while(1) {
                //READ DATA
                readData = read(new_sFeed, tempBuff, buffData);
                //IF NO DATA   
                if(buffData == 0){                    
                    break;
                }
                //GO THROUGH AND CHECK FOR NEW LINES
                int x;        
                for(x = 0; x < readData; x++) {      
                    if(buffer[x] == '\n') {
                        //NEXT LINE
                        fwd += 1;                                 
                        if(fwd == 1){
                            ky = buffer + x + 1;            
                        }
                    }
                }
                //IF FINISHED
                if(fwd == 2) {  
                    break;
                }
                //DECREMENT BY THE BYTES WE'VE READ
                buffData -= readData;
                //INCREMENT TO NEW POSITION       
                tempBuff += readData;            
            }
            //MAKE NEW MESSAGE
            char newMsg[BUFF_SIZE];
            //ZERO OUT BUFFER                         
            bzero(newMsg, sizeof(newMsg));
            //COPY BUFF CONTENTS TO NEW MESSAGE
            strncpy(newMsg, buffer, ky-buffer);
            //ENCRYPT MESSAGE       
            Encrypt(newMsg, ky, strlen(newMsg));
            //WRITE MESSAGE TO SOCKET
            write(new_sFeed, newMsg, sizeof(newMsg));
        }
        //IF FORK IS UNSUCCESSFUL
        if(pid < 0) {
            //PRINT ERROR
            perror("Error! Problem forking process...");
            //EXIT
            exit(1);
        }
        //CLOSE NEW FEED
        close(new_sFeed);                             
    }
    //CLOSE FEED
    close(sFeed);
    return 0;
}
//***********************FUNCTION IMPLEMENTATIONS***********************//
/*************************************************************************
** FUNCTION NAME: AuthArgs
** PARAMENTERS: an integer -arg
** DESCRIPTION: Checks for correct number of arguments
**************************************************************************/
void AuthArgs(int arg) {
    //CHECK NUMBER OF ARGUMENTS
    if(arg != 2) {
        //PRINT ERROR MESSAGE 
        printf("Error! Invalid number of arguments...\n");
        //EXIT
        exit(0);
    }
}
/*************************************************************************
** FUNCTION NAME: CheckBind
** PARAMENTERS: an integer -bnd
** DESCRIPTION: Checks that the binding was successful
**************************************************************************/
void CheckBind(int bnd) {
    //CHECK RESULT OF BIND
    if(bnd < 0) { 
        //PRINT ERROR MESSAGE 
        perror("Error! Problem with binding...\n");
        //EXIT
        exit(1);
    }
}
/*************************************************************************
** FUNCTION NAME: CheckFeed
** PARAMENTERS: an integer -newFeed
** DESCRIPTION: Checks that the binding was successful
**************************************************************************/
void CheckFeed(int newFeed) {
    //CHECK NEW FEED 
    if(newFeed < 0) {
    //PRINT ERROR MESSAGE            
        perror("Error! Problem with new feed...\n");
        //EXIT
        exit(1);
    }
}
/*************************************************************************
** FUNCTION NAME: ConvertInt
** PARAMENTERS: an integer -_int
** DESCRIPTION: converts an integer to a char
**************************************************************************/
char ConvertInt(int _int){    
  static const char *chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";      

  if (_int < 0 || _int > 27){                 
    return 'a';
  }
  return chars[_int];                       
}
/*************************************************************************
** FUNCTION NAME: ConvertChar
** PARAMENTERS: a char -_char
** DESCRIPTION: converts a char to an integer
**************************************************************************/
int ConvertChar(char _char){
    //POSSIBLE CHARACTERS
    static const char *chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";         
    //SEARCH FOR CHAR
    int x;
    for(x = 0; x < 27; x++){           
        //IF PRESENT
        if (_char == chars[x]) {
            //RETURN INDEX
            return x;
        }
    }
    //RETURN ERROR IF NOT FOUND 
    return -1;                  
}
/*************************************************************************
** FUNCTION NAME: Encrypt
** PARAMENTERS: a string -msg, a string -key, an integer -len
** DESCRIPTION: encrypts a message
**************************************************************************/
void Encrypt(char msg[], char key[], int len) {    
    //TEXT                         
    int txt;
    //KEY
    int ky;
    //GET LENGTH
    len = (strlen(msg)-1);
    //ENCRYPT
    int enc;
    int x;
    for (x = 0; x < len; x++){
        txt = ConvertChar(msg[x]);        
        ky = ConvertChar(key[x]);

        enc = (txt + ky) % 27;
        msg[x] = ConvertInt(enc);     
    }
    msg[x] = '\0';                                  
  
    return;
}