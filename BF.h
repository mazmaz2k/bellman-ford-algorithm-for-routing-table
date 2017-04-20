//omri mizrahi
//omrimi
//303082549
//ex3


#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <pthread.h>

//this is all rouers data  
typedef struct allRou {
    char * ip;
    int index;
    int port;
    char * name;
} allRou;

//structer of all the data concern the main router and his neighbors
typedef struct Myrouter {    
    int ChangesInDv; 
    int col;  //place in colom of the 2d dv 
    int * my_dv; //self dv of router
    int ** allRoutDv; //2d dv of nighbors
    int  row;//place in row of the 2d dv 
    char * name; //r
    char * ip;
    int port;
    int numOfAllRouters;
    char ** via; // array fo rhe destination
    int calcurator_finished; //find out if all the calc has finished
    int countOfNeighbours;//num of neighbors
    allRou ** allRou; //pointer to array of routers
    struct Myrouter ** my_neighbors;//array of neighbors
    int * neighbors_sent;
    int * neighbors_finished;	//find out if all the neighbors hass finished
    pthread_mutex_t * mutx;	//mutex to prevent simultantally access
    pthread_cond_t * cond_calc; //con variable for the calculator
    pthread_cond_t * cond_send;	//con variable for the sender
} Myrouter;

//all the relevent data to be sender and reciver functions.
typedef struct SendAndrecv {
    
    char * ip;		//main router ip
    int index;		//main router index
    int neighborPort; //neighbor port
    int myPort;		//my port
    int numOfAttempts;
    int * dv;	
    int ** dvMatrix;
    char * name;		//main router name
    char * neighbor_name;
    Myrouter * router;		//main router nighbores list
} SendAndrecv;


//thread to send the dv of all the neighbors
void * sender(void * params); 
//------------------------------------------------------------------------------------//

//thread to receive the dv from all the neighbors
void * receiver(void * params);//thread
//------------------------------------------------------------------------------------//

//esay print when there is errors
void printError(char* str);
//------------------------------------------------------------------------------------//

//thread calculate the shortes way to each neighbor we also print all the ways
void * calculate(void * params);
//------------------------------------------------------------------------------------//

//turn the string to number
int sumChars(char * str); 
//------------------------------------------------------------------------------------//

// return the index of the router 
int find_index(Myrouter * SelfRouter,char * name); 
//------------------------------------------------------------------------------------//

//create router for all the routers in the file
allRou * createRouter(char * line, Myrouter * SelfRouter,int index); 
//------------------------------------------------------------------------------------//


//initilaize the terminal router and his neighbors and all routers
void initilize( Myrouter * SelfRouter, char *argv[]); 
//------------------------------------------------------------------------------------//

//free all aloocated memory
void freeAllRouterList(Myrouter * SelfRouter);
//------------------------------------------------------------------------------------//


