/***********************************
** AUTHOR: DAVID RIDER
** TITLE: riderda.adventure.c
** CLASS: OSU CS 344
** DATE: NOVEMBER 2, 2019
***********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>

/******GLOBAL VALUES******/
/*BOOL VALUES*/
#define true 1
#define false 0
/*PATH MANAGEMENT*/
#define PATH 10

/*****ROOM TYPE ENUM******/
enum roomType {
	START_ROOM, 
	END_ROOM, 
	MID_ROOM
};

/*****ROOM STRUCT******/
struct Room {

  /*INTEGER ID FOR FUNCTION USAGE*/
  int id;

  /*ROOM NAME*/
  char name[10];

  /*ROOM TYPE*/
  enum roomType rT;

  /*NUMBER OF OUTBOUND CONNECTIONS*/
  int numConnections;

  /*STRING ARRAY TO HOLD IDS OF OTHER ROOMS*/
  int connectedRooms[6];
  /*STRING ARRAY TO HOLD NAMES OF OTHER ROOMS*/
  char connectedRoomNames[6][10];
};

/*****GLOBAL VARIABLES******/
char filePath[100];
int victoryPath[100];
char timeString[100];
pthread_mutex_t tMutex = PTHREAD_MUTEX_INITIALIZER;

/*****FUNCTION DECLARATIONS******/
/*DIRECTORY FUNCTIONS*/
void GetDirectory();
/*ROOM FUNCTIONS*/
int GetStartRoom(struct Room* rooms);
int GetEndRoom(struct Room* rooms);
void BuildRooms(struct Room* rooms);
/*GAME FUNCTIONS*/
void PlayGame();
int GetUserInput(struct Room* rooms, int currentRoom);
void DisplayRoom(struct Room* rooms, int currentRoom);
/*TIME FUNCTIONS*/
void MakeThread();
void* GetTime();
void GetTimeFromFile();

/*TESTING*/
void PrintRooms(struct Room* rooms);

/*****PROGRAM FLOW******/
int main() {

	/*MAKE ARRAY OF ROOMS*/
	struct Room room_array[7];
	/*GET MOST RECENT DIRECTORY*/
	GetDirectory();
	/*POPULATE ROOM STRUCTS*/
	BuildRooms(room_array);
	/*TESTING*/
	/*PrintRooms(room_array);*/
	/*START GAME*/
	PlayGame(room_array);
	pthread_mutex_destroy(&tMutex);
	return 0;
};
/******DIRECTORY FUNCTIONS*******/
/****************************************************
** NAME:
**    GetDirectory
** DESCRIPTION:
**    FINDS THE CORRECT (NEWEST) FILE DIRECTORY
** PARAMETERS:
**    None
** POSTCONDITIONS:
**    The directory is changed 
** CITATION:
**    Reading 2.4 "Manipulating Directories"
****************************************************/
void GetDirectory() {
  int newestDirTime = -1;                           
  char targetDirPrefix[32] = "riderda.rooms.";        
  char newestDirName[256];                           
  memset(newestDirName, '\0', sizeof(newestDirName));

  DIR* dirToCheck;                          
  struct dirent *fileInDir;                  
  struct stat dirAttributes;                  

  dirToCheck = opendir("."); 

  if (dirToCheck > 0) {

    while ((fileInDir = readdir(dirToCheck)) != NULL) {

      if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) {

        stat(fileInDir->d_name, &dirAttributes); 

        if ((int)dirAttributes.st_mtime > newestDirTime) {

          newestDirTime = (int)dirAttributes.st_mtime;
          memset(newestDirName, '\0', sizeof(newestDirName));
          strcpy(newestDirName, fileInDir->d_name);

        };
      };
    };
  };

  closedir(dirToCheck);                             
  strcpy(filePath, newestDirName);
  chdir(newestDirName);
};
//*****ROOM FUNCTIONS******/
/****************************************************
** NAME:
**    GetStartRoom
** DESCRIPTION:
**    Finds the starting room
** PARAMETERS:
**    An array of Room structs
** POSTCONDITIONS:
**    An integer value representing the index in the
**	  room array is returned.
****************************************************/
int GetStartRoom(struct Room* rooms) {
	/*RETURN VALUE*/
	int roomIndex = -1;

	/*LOOP THROUGH ROOM ARRAY*/
	int x;
	for (x = 0; x < 7; x++) {
		if (rooms[x].rT == 0) {
			roomIndex = x;
		};
	};

	return roomIndex;

};
/****************************************************
** NAME:
**    GetEndRoom
** DESCRIPTION:
**    Finds the ending room
** PARAMETERS:
**    An array of Room structs
** POSTCONDITIONS:
**    An integer value representing the index in the
**	  room array is returned.
****************************************************/
int GetEndRoom(struct Room* rooms) {
	/*RETURN VALUE*/
	int roomIndex = -1;

	/*LOOP THROUGH ROOM ARRAY*/
	int x;
	for (x = 0; x < 7; x++) {
		if (rooms[x].rT == 1) {
			roomIndex = x;
		};
	};

	return roomIndex;
};
/****************************************************
** NAME:
**    BuildRooms
** DESCRIPTION:
**    Reads the room files to populate the Room array
** PARAMETERS:
**    An array of Room structs
** POSTCONDITIONS:
**    ??????
****************************************************/
void BuildRooms(struct Room* rooms) {
	char curDir[100];

	getcwd(curDir, sizeof(curDir));

	DIR *dir;
	struct dirent *fileInDir;
	int z = 0;
	/*GET ROOM NAMES FROM FILE NAMES*/
	if ((dir = opendir(curDir)) != NULL) {
		while ((fileInDir = readdir (dir)) != NULL) {
			if ( !strcmp(fileInDir->d_name, ".") || !strcmp(fileInDir->d_name, "..")) {
				/*TO IGNORE THE "." and ".."*/
			} else {
				strcpy(rooms[z].name, fileInDir->d_name);
				z++;
			};
			/*ID ROOMS*/
			rooms[z].id = z;
		};
		closedir(dir);
	} else {
		perror("");
	};
	
	/*LOOP THROUGH EACH FILE IN DIRECTORY*/
	int y;
	for (y = 0; y < 7; y++) {
		/*GET NEW FILEPATH*/
		char newPath[100];
		sprintf(newPath, curDir);
		strcat(newPath, "/");
		strcat(newPath, rooms[y].name);
		/*OPEN FILE*/
		/******TESTING******/
		/*printf("PATH BEING USED: %s\n", newPath);*/
		/*GET ROOM FILE*/
		FILE* roomFile;
		roomFile = fopen(newPath, "r");
		/*BUFFER TO HOLD CONTENTS OF THE LINE*/
		char lineBuff[100];
		
		int con = 0;
		while (fgets(lineBuff, 100, roomFile) != NULL) {
			/*GRAB EACH SECTION OF THE LINE*/
			char command1[36];
			char command2[36];
			char command3[36];

			sscanf(lineBuff, "%s %s %s", command1, command2, command3);
			/*ASSIGN CONNECTIONS*/
			if ((strcmp(command1, "CONNECTION") == 0)) {
				/*ADD CONNECTED ROOM NAME*/
				strcpy(rooms[y].connectedRoomNames[con], command3);
				con++;
				rooms[y].numConnections = con;
			/*ASSIGN ROOM TYPE*/
			} else if ((strcmp(command2, "TYPE:") == 0)) {
				
				if ((strcmp(command3, "START_ROOM") == 0)) {
					/*ROOM IS THE START_ROOM*/
					rooms[y].rT = 0;
				} else if ((strcmp(command3, "END_ROOM") == 0)) {
					/*ROOM IS THE END_ROOM*/
					rooms[y].rT = 1;
				} else {
					/*ROOM IS A MID_ROOM*/
					rooms[y].rT = 2;
				};
			} else {
				
			};
		};
		/*CLOSE FILE*/
		fclose(roomFile);
	};
	/*POPULATE CONNECTIONS INTEGER ARRAY
	LOOP THROUGH ROOM STRUCTS*/
	int x;
	for (x = 0; x < 7; x++) {
		/*LOOP THROUGH EACH ROOM'S CONNECTIONS*/
		int y;
		for (y = 0; y < rooms[x].numConnections; y++) {
			/*VARIABLE TO HOLD CONNECTION'S ID*/
			int roomID;
			int z;
			/*LOOP THROUGH ROOM STRUCTS*/
			for (z = 0; z < 7; z++) {
				/*COMPARE EACH NAME*/
				if ((strcmp(rooms[x].connectedRoomNames[y], rooms[z].name)) == 0) {
					/*SET VARIABLE*/
					roomID = z;
				};
			};
			/*ASSIGN INTEGER ID*/
			rooms[x].connectedRooms[y] = roomID;
		};
	};
	/*GO BACK TO DIRECTORY WHERE OUR EXECUTABLES ARE*/
	chdir("..");
};
//*****GAME FUNCTIONS*****
/****************************************************
** NAME:
**    PlayGame
** DESCRIPTION:
**    Controls the functions of the game
** PARAMETERS:
**    An array of Room structs
** POSTCONDITIONS:
**    Game is run
****************************************************/
void PlayGame(struct Room* rooms) {
	/*LOCK MAIN THREAD*/
	pthread_mutex_lock(&tMutex);
	/*BOOL TO END GAME*/
	int endRoomFound = false;
	/*VARIABLE TO HOLD # OF STEPS*/
	int steps = 0;
	/*GET STARTING ROOM*/
	int curRoom = GetStartRoom(rooms);
	/*PATH MANAGEMENT*/
	int pathIndex = 0;
	/*LOOP UNTIL GAME IS OVER*/
	while (endRoomFound == false) {
		/*DISPLAY ROOM OPTIONS*/
		DisplayRoom(rooms, curRoom);
		/*GET USER CHOICE*/
		/*USER INPUT*/
		char userInput[36];
		/*PROMPT*/
		printf("Where to? >");
		/*GRAB INPUT*/
		scanf("%s", userInput);
		
		int badInputFlag = 0;
		int i = curRoom;
		int x;
		for (x = 0; x < rooms[i].numConnections; x++) {

			int roomToCompare = rooms[i].connectedRooms[x];
			if (strcmp(userInput, rooms[roomToCompare].name) == 0) {
				/*UPDATE CURRENT ROOM*/
				curRoom = roomToCompare;
				/*ADD ROOM TO PATH*/
				victoryPath[pathIndex] = curRoom;
			} else if (strcmp(userInput, "time") == 0) {
				/*UNLOCK MAIN THREAD*/
				pthread_mutex_unlock(&tMutex);
				/*WHEN TIME GETS TRIGGERED
				IT PUSHES WHATEVER ROOM HAS ID 0 
				TO THE VictoryPsth*/
				MakeThread();
				GetTimeFromFile();
				/*IF TIME GETS CALLED WE DONT NEED TO INCREMENT
				SO WE WILL COUNTER THE LATER INCREMENT*/
				steps -= 1;
				pathIndex -= 1;
				break;
			} else {
				badInputFlag++;
			};
		};
		/*CHECK FOR ERROR FLAGS*/
		int c = rooms[i].numConnections;
		if (badInputFlag > c - 1) {
			printf("\nHUH? I DON'T UNDESTAND THAT ROOM. TRY AGAIN.\n");
		} else {
			/*INCREMENT COUNTERS IF INPUT WAS VALID*/
			steps++;
			pathIndex++;
		};

		/*CHECK IF END ROOM IS REACHED*/
		if (curRoom == GetEndRoom(rooms)) {
			endRoomFound = true;
		};
		printf("\n");
	};
	/*WINNING CONDITIONS HAVING BEEN MET*/
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUT PATH TO VICTORY WAS:\n", steps);
	/*LOOP THROUGH VICTORY PATH*/
	int v;
	for (v = 0; v < pathIndex; v++) {
		/*PRINT VISITED ROOMS*/
		printf("%s\n", rooms[victoryPath[v]].name);
	};

};

/****************************************************
** NAME:
**    DisplayRoom
** DESCRIPTION:
**    Displays the current room's connections for the
**	  player to the terminal
** PARAMETERS:
**    -An array of Room structs
**	  -An integer representing the current room
** POSTCONDITIONS:
**    The current room's connections are printed to
**	  the terminal
****************************************************/
void DisplayRoom(struct Room* rooms, int currentRoom) {
	int i = currentRoom;
	/*DISPLAY CURRENT ROOM*/
	printf("CURRENT LOCATION: %s\n", rooms[i].name);
	/*DISPLAY CONNECTIONS*/
	printf("POSSIBLE CONNECTIONS: ");
	int c;
	for (c = 0; c < rooms[i].numConnections; c++) {
		/*PRINT CONNECTIONS*/
		int roomToPrint = rooms[i].connectedRooms[c];
		printf("%s", rooms[roomToPrint].name);
		/*PUNCTUATION FORMATTING*/
		if (c < rooms[i].numConnections - 1) {
			printf(", ");
		} else {
			printf(".\n");
		};
	};
};
/****TIME FUNCTIONS****/
/****************************************************
** NAME:
**    MakeThread
** DESCRIPTION:
**    Handles the multithreading of the progrram
** PARAMETERS:
**    None
** POSTCONDITIONS:
**    The thread is created, mutex initieated & 
**	  locked, the time thread is created, joined,
**    then the mutex is unlocked and destroyed.
** CITATION:
**	  https://stackoverflow.com/questions/20276010/c-creating-n-threads
****************************************************/
void MakeThread() {
	/*CREATE PTHREAD*/
	pthread_t tThread;
	/*INITIALIZE THREAD*/                          
    pthread_mutex_init(&tMutex, NULL);
    /*LOCK MUTEX*/
    pthread_mutex_lock(&tMutex);
    /*CREATE TIME THREAD*/
    int timeThread = pthread_create(&tThread, NULL, GetTime, NULL);
    /*JOIN THREAD*/
    pthread_join(tThread, NULL);
    /*UNLOCK THREAD*/
    pthread_mutex_unlock(&tMutex);
    /*DESTROY THREAD*/
    pthread_mutex_destroy(&tMutex);
 
};
/****************************************************
** NAME:
**    GetTime
** DESCRIPTION:
**    Calculates time and writes it to a file named
**    "currentTime.txt"
** PARAMETERS:
**    None
** POSTCONDITIONS:
**    The time is calculated an written to a file.
** CITATION:
**	  https://stackoverflow.com/questions/7411301/how-to-introduce-date-and-time-in-log-file
****************************************************/
void* GetTime() {
	/*CREATE FILE*/
	FILE* timeFile;                             
    timeFile = fopen("currentTime.txt", "w+"); 

    /*GET TIME*/   
    struct tm *timeStruct;
    time_t curTime = time (0);
    timeStruct = gmtime (&curTime);

  	/*FORMAT TIME STRING*/
    strftime (timeString, sizeof(timeString), "%l:%M%P, %A, %B %d, 20%y\n", timeStruct);
   
    /*WRITE TIME TO FILE*/
    fputs(timeString, timeFile);

    /*CLOSE TIME FILE*/
    fclose(timeFile);
};
/****************************************************
** NAME:
**    GetTimeFromFile
** DESCRIPTION:
**    Reads time from currentTime.txt
** PARAMETERS:
**    None
** POSTCONDITIONS:
**    Time from file is printed
****************************************************/
void GetTimeFromFile() {
	/*OPEN FILE*/
	FILE* openTimeFile;
  	openTimeFile = fopen("currentTime.txt", "r");

  	/*TEMP BUFFER TO HOLD TIME STRING*/
  	char timeStringBuffer[100];
  
  	/*CHECK IF currentTime.txt IS THERE*/
  	if(openTimeFile == NULL){               
    	perror("Not found\n");
  	} else{
  		/*POPULATE TEMP BUFFER*/
    	fgets(timeStringBuffer, 100, openTimeFile);
    	/*PRINT TIME*/     
    	printf("\n%s\n", timeStringBuffer);
    	/*CLOSE FILE*/
    	fclose(openTimeFile);
  	};
};
/****************************************************
** NAME:
**    PrintRooms
** DESCRIPTION:
**	  Prints data stored in each room struct
** PARAMETERS:
**    An array of Room structs
** POSTCONDITIONS:
**    Room struct data is printed to the terminal
****************************************************/
void PrintRooms(struct Room* rooms) {
	/*LOOP THROUGH ROOMS*/
	int x;
	for (x = 0; x < 7; x++) {
		
		/*PRINT DATA*/
		printf("**************************\n");
		printf("Room #%d\n", rooms[x].id);
		printf("Name: %s\n", rooms[x].name);
		printf("Type: %d\n", rooms[x].rT); 
		printf("Connections: %d\n", rooms[x].numConnections);
		printf("---------------\n");
		int y;
		for (y = 0; y < rooms[x].numConnections; y++) {
			printf("Room: %s\n", rooms[x].connectedRoomNames[y]);
		};
		printf("**************************\n");
	};
};