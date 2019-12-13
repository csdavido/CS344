/****************************************************
** AUTHOR: DAVID RIDER
** PROGRAM NAME: otp_enc.c
** CLASS: CS 344
** DATE: 12/07/2019
** DESCRIPTION: 
** CITATION: 
**      client.c
**      Beej's Guide to Network Programming
****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h> 
//***********************GLOBALS***********************//
#define BUFF_SIZE 80000

int t_size;
int k_size;
//****************FUNCTION DECLARATIONS****************//
void AuthArgs(int arg);
void VerrifyTextSize(int k, int t);
void VerrifyStatus(int stat);
void SendFile(char* fName, int sockFeed, int len);

//************************MAIN************************//
int main(int argc, char* argv[]) { 
    //***LOCAL VARIABLES***
    int sFeed;
    int portNum;
    int status;
    struct sockaddr_in sAddress;
    struct hostent* serverHostInfo;
    //SET AUTHORIZATION SYMBOL
    char authen[] = "e";
    char buffer[BUFF_SIZE];
    //CLEAR BUFFER
    bzero(buffer, sizeof(buffer));
    //AUTHENTICATE NUMBER OF ARGS
    AuthArgs(argc);
    //GET PORT NUMBER
    portNum = atoi(argv[3]); 
    //SET UP FEED
    sFeed = socket(AF_INET, SOCK_STREAM, 0); 
    //CHECK FEED
    if(sFeed < 0){ 
        //PRINT ERROR MESSAGE                      
        error("Error! Can't open socket...\n");
        exit(1);
    }
    //GET HOST INFO
    serverHostInfo = gethostbyname("localhost");    
    //CHECK HOST INFO
    if(serverHostInfo == NULL){ 
        //PRINT ERROR MESSAGE                    
        fprintf(stderr, "Error! Host error...\n");
        exit(0);
    }
    //SET SOCKET OPTIONS
    int optval = 1;
    setsockopt(sFeed, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    //CLEAN SERVER ADDRESS
    bzero((char *) &sAddress, sizeof(sAddress));
    //MAKE SOCKET
    sAddress.sin_family = AF_INET;
    //GET ADDRESS
    bcopy((char *)serverHostInfo -> h_addr, (char *)&sAddress.sin_addr.s_addr, serverHostInfo -> h_length);
    //GET PORT NUMBER
    sAddress.sin_port = htons(portNum);
    //CHECK CONNECTION
    if(connect(sFeed, (struct sockaddr *) &sAddress, sizeof(sAddress)) < 0) {
        //PRINT ERROR MESSAGE
        perror("Error! Can't connect...\n");
        exit(1);
    }
    //AUTHENTICATE
    write(sFeed, authen, sizeof(authen));
    //GET AUTHENTICATION SYMBOL
    read(sFeed, buffer, sizeof(buffer));
    //CHECK AUTHENTICAITON
    if(strcmp(buffer, "e") != 0){
        //PRINT ERROR
        fprintf(stderr, "Error! Can't write to socket...\n");
        //EXIT
        exit(2);
    }
    //GET KEY
    int key = open(argv[2], O_RDONLY);
    //GET TEXT 
    int text = open(argv[1], O_RDONLY);
    //VERRIFY KEY AND TEXT SIZE
    VerrifyTextSize(key, text);
    
    //CHECK FOR INVALID CHARACTERS
    int checker = open(argv[1], 'r');           
    while(read(checker, buffer, 1) != 0){
        if(isspace(buffer[0]) || isalpha(buffer[0])){
            //DO NOTHING
        }
        else{
            //PRINT ERROR MESSAGE
            fprintf(stderr, "Error! %s has invalid characters...\n", argv[1]);
            exit(1);
        }
    }
    //ZERO OUT BUFFER
    bzero(buffer, sizeof(buffer));          
    //SEND TEXT
    SendFile(argv[1], sFeed, t_size);
    //SEND KEY
    SendFile(argv[2], sFeed, k_size);
    //GET STATUS CODE
    status = read(sFeed, buffer, sizeof(buffer) - 1);
    //CHECK STATUS CODE
    VerrifyStatus(status); 
    //PRINT BUFFER
    printf("%s\n", buffer);
    //CLOSE SOCKET
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
    if(arg != 4) {
        //PRINT ERROR MESSAGE 
        printf("Error! Invalid number of arguments...\n");
        //EXIT
        exit(0);
    }
}
/*************************************************************************
** FUNCTION NAME: VerrifyTextSize
** PARAMENTERS: two integers k, for the size of the k and t, for the size
**              of the key
** DESCRIPTION: Gets the key and text size and checks the key is long 
**              enough
**************************************************************************/
void VerrifyTextSize(int k, int t) {
    //GET KEY SIZE
    int k_size = lseek(k, 0, SEEK_END);
    //GET TEXT SIZE         
    int t_size = lseek(t, 0 , SEEK_END);
    //CHECK SIZES
    if(t_size > k_size) {
        //PRINT ERROR MESSAGE                     
        fprintf(stderr, "Error! Key is not long enough...\n");
        //EXIT
        exit(1);
    }
}
/*************************************************************************
** FUNCTION NAME: VerrifyStatus
** PARAMENTERS: an integer -stat
** DESCRIPTION: Checks the status code
**************************************************************************/
void VerrifyStatus(int stat) {
    //CHECK STATUS
    if(stat < 0) {
        //PRINT ERROR                             
        perror("Error! Unable to read from socket...\n");
        //EXIT
        exit(1);
    }   
}
/*************************************************************************
** FUNCTION NAME: SendFile
** PARAMENTERS: a file name -fName, and socket feed integer, a length
** DESCRIPTION: Sends a file on a socket
**************************************************************************/
void SendFile(char* fName, int sockFeed, int len){
    //OPEN FILE
    FILE* file = fopen(fName, "r");         
    //SET BUFFER
    char fileBuff[BUFF_SIZE];
    //WRITE ZEROES TO BUFFER
    bzero(fileBuff, BUFF_SIZE);
    //DATA BEING SENT
    int bytes;
    //LOOP THROUGH DATA
    while((len = fread(fileBuff, sizeof(char), BUFF_SIZE, file)) > 0) { 
        //IF NO DATA
        if((bytes = send(sockFeed, fileBuff, len, 0)) < 0) { 
            break;
        }
        //WRITE ZEROES TO BUFFER AGAIN
        bzero(fileBuff, BUFF_SIZE);  
    }
    //IF ALL DATA IS ACCOUNTED FOR
    if(bytes == BUFF_SIZE) { 
        //SEND          
        send(sockFeed, "0", 1, 0);
    }
    //CLOSE FILE
    fclose(file);

    return;
}