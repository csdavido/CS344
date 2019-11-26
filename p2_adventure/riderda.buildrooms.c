/***********************************
** AUTHOR: DAVID RIDER
** TITLE: riderda.buildrooms.c
** CLASS: OSU CS 344
** DATE: NOVEMBER 2, 2019
***********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

/*****GLOBAL VALUES*****/
#define true 1
#define false 0

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

  /*STRUCT ARRAY TO HOLD CONNECTIONS*/
  struct Room* connectedRooms[6];

};

/*****FUNCTION DECLARATIONS*****/
/*TESTING*/
void PrintRooms(struct Room* rooms);
/*ROOM CREATION*/
void GenerateRooms(struct Room* rooms);
void IDRoom(struct Room* rooms);
void NameRoom(struct Room* rooms);
void TypeRoom(struct Room* rooms);
/*CONNECTION GRAPHING*/
int CanAddConnectionFrom(struct Room* x);
int ConnectionAlreadyExists(struct Room* x, struct Room* y);
int IsSameRoom(struct Room* x, struct Room* y);
int IsGraphFull(struct Room* rooms);
void AddRandomConnection(struct Room* rooms);
void ConnectRoom(struct Room* x, struct Room* y); 
/*FILE CREATION*/
void CreateRoomFiles(struct Room* rooms);
/*UTILITIES*/
int GetRandomNum(int upper, int lower);

/*****PROGRAM FLOW******/
int main() {

	/*RANDOM SEED*/
	srand(time(0));

	/*MAKE ARRAY OF ROOMS*/
	struct Room room_array[7];

	/*GENERATE ROOMS & POPULATE CONNECTIONS*/
	GenerateRooms(room_array);

	/*CREATE AND POPULATE FILES*/
	CreateRoomFiles(room_array);

	/*TESTING*/
	/*PrintRooms(room_array);*/

	return 0;
};
/*****ROOM CREATION*******/
/****************************************************
** NAME:
**    GenerateRooms
** DESCRIPTION:
**	  Populates each Room struct in the Room struct
**    array
** PARAMETERS:
**	  An array of Room structs
** POSTCONDITIONS:
**	  Each struct is created, and populated
****************************************************/
void GenerateRooms(struct Room* rooms) {
	/*FOR EACH NUMBER OF ROOMS*/
	int i;
	for (i = 0; i < 7; i++) {

		/*GENERATE IDS*/
		IDRoom(rooms);

		/*NAME ROOMS*/
		NameRoom(rooms);

		/*ADD CONNECTIONS*/
		AddRandomConnection(rooms);

	};
	/*GENERATE ROOM TYPE*/
	TypeRoom(rooms);
};
/****************************************************
** NAME:
**    IDRoom
** DESCRIPTION:
**	  Assigns an ID to each Room struct
** PARAMETERS:
**	  An array of Room structs
** POSTCONDITIONS:
**	  Each Room struct is assigned an ID
****************************************************/
void IDRoom(struct Room* rooms) {
	/*FOR EACH ROOM*/
	int x;
	for (x = 0; x < 7; x++) {

		/*OPEN FILE*/
		rooms[x].id = (x + 1);

		/*INITIALIZE CONNECTIONS TO 0*/
		rooms[x].numConnections = 0;

	};
};
/****************************************************
** NAME:
**    NameRoom
** DESCRIPTION:
**	  Assigns a name to each Room struct
** PARAMETERS:
**	  An array of Room structs
** POSTCONDITIONS:
**	  Each Room struct is assigned a name
****************************************************/
void NameRoom(struct Room* rooms) {

	/*ARRAY TO RANDOMIZE ROOM NAMES WITH INTEGERS*/
	char availRooms[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	/*ARRAY TO HOLD RANDOMIZED ROOM NAMES*/
	char availRoomNames[10][10];

	/*RANDOMIZE THE ORDER OF THE INTEGERS*/
	/*TEMP FOR STORING INT REPRESENTING ROOMS*/
	int temp;

	/*INDEX FOR POSITION IN ROOMS ARRAY*/
	int index;

	/*FOR EACH POSSIBLE ROOM*/
	int x;
	for (x = 0; x < 10; x++) {

		/*STORE TEMPORARY INT VALUE*/
		temp = availRooms[x];

		/*RANDOMIZE INDEX POSITION*/
		index = rand() % 10;
		/*IF IT HAPPENS TO BE THE SAME AS THE availRooms ARRAY*/
		if (availRooms[x] == availRooms[index]) {
			/*THEN DECREMENT ITERATOR BY ONE*/
			/*THIS ENSURES THAT THE ARRAY IS MIXED WELL*/
			x = x - 1;
		/*IF NOT*/
		} else {
			/*SET VALUE TO NEW RANDOM NUMBER*/
			availRooms[x] = availRooms[index];
			availRooms[index] = temp;
		};
	};
	
	/*NOW THAT WE HAVE A RANDOM INTEGER ARRAY
	**WE USE THAT ARRAY AS A BASIS TO ASSIGN
	**THE POSITION OF THE 10 POSSIBLE NAMES*/
	int y;
	for (y = 0; y < 10; y++) {
		/*TEMPORARY HOLDER*/
		int tempRoom = availRooms[y];
		/*ASSIGN ROOM NAME BASED ON RANDOM INTEGER ORDER
		**I.E. A VALUE OF "1" WILL ALWAYS BE SET TO STABLE,
		**BUT THE "1" IS ALWAYS IN A RANDOM POSITION, IF IT
		**OCCUPIES THE 8, 9, OR 10 POSITION, THEN IT WILL
		**BE DISREGARDED LATER*/
		switch (tempRoom) {
			case 1:
				strcpy(availRoomNames[y], "stable");
				break;
			case 2:
				strcpy(availRoomNames[y], "hallway");
				break;
			case 3:
				strcpy(availRoomNames[y], "stairway");
				break;
			case 4:
				strcpy(availRoomNames[y], "kitchen");
				break;
			case 5:
				strcpy(availRoomNames[y], "armory");
				break;
			case 6:
				strcpy(availRoomNames[y], "smithy");
				break;
			case 7:
				strcpy(availRoomNames[y], "tower");
				break;
			case 8:
				strcpy(availRoomNames[y], "chapel");
				break;
			case 9:
				strcpy(availRoomNames[y], "barracks");
				break;	
			case 10:
				strcpy(availRoomNames[y], "dungeon");
				break;
		};
	};
	/*ONLY THE FIRST 7 IN THE availRoomNames ARRAY*/
	/*BUT THE 7 WILL BE DIFFERENT EACH TIME THE PROGRAM RUNS*/
	/*LOOP THROUGH ROOMS TO BE USED*/
	int z;
	for (z = 0; z < 7; z++) {
		/*ASSIGN ROOM NAME*/
		strcpy(rooms[z].name, availRoomNames[z]);
	};
};
/****************************************************
** NAME:
**    TypeRoom
** DESCRIPTION:
**	  Assigns a room type to each Room struct
** PARAMETERS:
**	  An array of Room structs
** POSTCONDITIONS:
**	  Each Room struct is assigned a room type
****************************************************/
void TypeRoom(struct Room* rooms) {

	/*RANDOMIZE ROOM TYPE*/
	/*INTEGERS TO HOLD RANDOMLY*/
	/*GENERATED VALUES*/
	int startRoom, endRoom;
	int unique = 0;
	/*LOOP UNTIL BOTH START AND END*/
	/*ROOMS ARE UNIQUE*/
	while (unique == 0) {
		/*RANDOMIZE START ROOM*/
		startRoom = GetRandomNum(6, 0);
		/*RANDOMIZE END ROOM*/
		endRoom = GetRandomNum(6, 0);
		/*MAKE SURE THEY ARE DIFFERENT*/
		if (startRoom != endRoom) {
			unique = 1;
		} else {
			unique = 0;
		};
	};
	/*ASSIGN ROOM TYPE*/
	/*LOOP THROUGH NUMBER OF ROOMS*/
	int z;
	for (z = 0; z < 7; z++) {
		/*ASSIGN START ROOM*/
		if (startRoom == z) {
			rooms[z].rT = START_ROOM;
		/*ASSIGN END ROOM*/
		} else if (endRoom == z) {
			rooms[z].rT = END_ROOM;
		/*ASSIGN ALL MID ROOMS*/
		} else {
			rooms[z].rT = MID_ROOM;
		};
	};
};
/*****CONNECTION GRAPHING******/
/****************************************************
** NAME:
**    CanAddConnectionFrom
** DESCRIPTION:
**	  Checks to see if a connection can be added from a room
** PARAMETERS:
**	  A single Room struct
** POSTCONDITIONS:
**	  An int is returned
****************************************************/
int CanAddConnectionFrom(struct Room* x) {

	/*RETURN VALUE*/
	int canAdd = false;
	/*IF A ROOM HAS LESS THAN 6 CONNECTIONS*/
	if (x->numConnections < 6) {
		/*IT CAN ADD ANOTHER CONNECTION*/
		canAdd = true;
	} else {
		/*IT CAN'T ADD ANOTHER CONNECTION*/
		canAdd = false;
	};
	/*RETURN VALUE*/
	return canAdd;

};
/****************************************************
** NAME:
**    ConnectionAlreadyExists
** DESCRIPTION:
**	  Checks to see if two Room structs already have a connection
** PARAMETERS:
**	  Two Room structs
** POSTCONDITIONS:
**	  An int is returned
****************************************************/
int ConnectionAlreadyExists(struct Room* x, struct Room* y) {
	int connectionExits = false;
	/*LOOP THROUGH ROOM x's CONNECTIONS*/
	int i;
    for(i = 0; i < x->numConnections; i++) {
    	/*IF ROOM y IS CONNECTED TO ROOM x*/
        if(x->connectedRooms[i]->id == y->id) {
        	/*CONNECTION EXISTS*/
            connectionExits = true;
        };
    };
    /*CONNECTION DOESN'T EXIST*/
    return connectionExits;

};
/****************************************************
** NAME:
**    IsSameRoom
** DESCRIPTION:
**	  Checks to see if the two structs are different
** PARAMETERS:
**	  Two Room structs
** POSTCONDITIONS:
**	  An int is returned
****************************************************/
int IsSameRoom(struct Room* x, struct Room* y) {

	/*RETURN VALUE*/
	int isSame;
	/*IF THE 2 ROOMS HAVE THE SAME ROOM ID*/
	if (x->id == y->id) {
		/*THEY ARE THE SAME ROOM*/
		isSame = true;
	/*IF THE 2 ROOMS HAVE DIFFERENT IDS*/
	} else {
		/*THEY ARE NOT THE SAME*/
		isSame = false;
	};
	/*RETURN VALUE*/
	return isSame;

};
/****************************************************
** NAME:
**    IsGraphFull
** DESCRIPTION:
**	  Checks if a Room Struct can add any more
**    connecting rooms
** PARAMETERS:
**	  An array of Room structs
** POSTCONDITIONS:
**	  An int value is returned
****************************************************/
int IsGraphFull(struct Room* rooms)  {

	/*RETURN VALUE*/
	int full = false;
	/*COUNTER TO TRACK HOW MANY ROOMS ARE NOT FULL*/
	int counter = 0;

	/*LOOP THROUGH ROOMS*/
	int x;
	for (x = 0; x < 7; x++) {
		/*IF ANY OF THE ROOMS HAVE LESS THAN THREE CONNECTIONS*/
		if (rooms[x].numConnections < 3) {
			/*INCREMENT COUNTER*/
			counter += 1;
		};
	};

	/*IF COUNTER HAS NOT BEEN INCREMENTED*/
	if (counter == 0) {
		/*GRAPH IS FULL*/
		full = true;
	} 

	return full;

};
/****************************************************
** NAME:
**    AddRandomConnection
** DESCRIPTION:
**	  Creates a connection between two rooms by using
**    utility functions to check feasability
** PARAMETERS:
**	  Two Room structs
** POSTCONDITIONS:
**	  Two connections are added
****************************************************/
void AddRandomConnection(struct Room* rooms) {

	while (IsGraphFull(rooms) == false) {

		/*CHOOSE A RANDOM NUMBER FOR ROOM X*/
		int x = GetRandomNum(7, 0);
		/*CHOOSE A RANDOM NUMBER FOR ROOM Y*/
		int y = GetRandomNum(7, 0);
		
		/*MAKE SURE ROOM X CAN HAVE NEW CONECTIONS*/
		while (CanAddConnectionFrom(&rooms[x]) == false) {
			x = GetRandomNum(7, 0);
		};
		while (CanAddConnectionFrom(&rooms[y]) == false) {
			y = GetRandomNum(7, 0);
		};

		/*CHECK TO SEE IF ROOMS ARE SAME*/
		/*CHECK TO SEE IF ALREADY CONNECTED*/
		if (IsSameRoom(&rooms[x], &rooms[y]) == false && ConnectionAlreadyExists(&rooms[x], &rooms[y]) == false) {
			/*CONNECT BOTH ROOMS*/
			ConnectRoom(&rooms[x], &rooms[y]);
			ConnectRoom(&rooms[y], &rooms[x]);

		};
	};
};
/****************************************************
** NAME:
**    ConnectRoom
** DESCRIPTION:
**	  Creates a connection between two Room structs
** PARAMETERS:
**	  Two Room structs
** POSTCONDITIONS:
**	  Two connections are added
****************************************************/
void ConnectRoom(struct Room* x, struct Room* y) {

	/*ADD ROOM y TO ROOM x's CONNECTIONS*/
	x->connectedRooms[x->numConnections] = y;
	/*INCREMENT CONNECTION COUNTER INDEX*/
	x->numConnections++;

};
/*****FILE CREATION******/
/****************************************************
** NAME:
**    CreateRoomFiles
** DESCRIPTION:
**	  Creates files and populates the files with the
**	  previously created Room structs
** PARAMETERS:
**	  An array of Room structs
** POSTCONDITIONS:
**	  Each Room file is created and filled
****************************************************/
void CreateRoomFiles(struct Room* rooms) {

	/*CREATE DIRECTORY*/
	char dirName[200];
	/*CREATE PATH & GET PID*/
	sprintf(dirName, "riderda.rooms.%d", getpid());
	/*CREATE DIRECTORY & GIVE PERMISSIONS*/
	mkdir(dirName, 0700);
	/*NAVIGATE TO DIRECTORY*/
	chdir(dirName);

	/*CREATE FILES*/
	/*CITATION: https://stackoverflow.com/a/18970690*/
	int f;
	for (f = 0; f < 7; f++) {
		/*CREATE FILE*/
		FILE* file = fopen(rooms[f].name, "a");
		/*NAME FILE*/
		fprintf(file, "ROOM NAME: %s\n", rooms[f].name);
		/*LOOP THROUGH EACH ROOM'S CONNECTIONS */
		int c;
		for (c = 0; c < rooms[f].numConnections; c++) {
			/*PRINT CONNECTION*/
			fprintf(file, "CONNECTION %d: %s\n", c + 1, rooms[f].connectedRooms[c]->name);
		};

		/*ROOM TYPE*/
		/*TRANSLATE ENUM VALUE TO STRING VALUES*/
		switch (rooms[f].rT) {
			/*PRINT ROOM TYPE*/
			case 0:
				fprintf(file, "ROOM TYPE: %s\n", "START_ROOM");
				break;
			case 1:
				fprintf(file, "ROOM TYPE: %s\n", "END_ROOM");
				break;
			case 2:
				fprintf(file, "ROOM TYPE: %s\n", "MID_ROOM");
		};

		/*CLOSE FILE*/
		fclose(file);

	};
};
/*****UTILITIES******/
/****************************************************
** NAME:
**    GetRandomNum
** DESCRIPTION:
**	  Generates a random number.
** PARAMETERS:
**    int upper - an upper bound of an integer range
**    int lower - a lower bound of an integer range
** POSTCONDITIONS:
**    Returns a random integer.
****************************************************/
int GetRandomNum(int upper, int lower) {

	/*RANDOM FORMULA*/
	int ranNum = (rand() % (upper - lower + 1)) + lower;
	/*RETURN RANDOM NUMBER*/
	return ranNum;

};

/*****TESTING******/
/*****TESTING******/
/*****TESTING******/
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
		printf("Room #%d\n", rooms[x].id);
		printf("Name: %s\n", rooms[x].name);
		printf("Type: %d\n", rooms[x].rT); 
		printf("Connections: %d\n", rooms[x].numConnections);

	};
};