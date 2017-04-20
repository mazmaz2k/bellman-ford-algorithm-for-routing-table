//omri mizrahi
//303082549
//ex3

#ifndef BF
#define BufSize 256 
#define SIZE 1000
 
#include "ex3.h"

#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include  <fcntl.h>
#include <stdint.h>
#include <signal.h>


void printError(char* str)
{
			
	fprintf(stderr,"%s\n",str);
	exit(0);
	
}

void freeAllRouterList(Myrouter * SelfRouter)
{
    int i;
    free(SelfRouter->name);
    SelfRouter->name=NULL;
    free(SelfRouter->ip);
    free(SelfRouter->my_dv);
    for(i=0;i<SelfRouter->numOfAllRouters;i++)
    {
        free(SelfRouter->allRou[i]->name);
        free(SelfRouter->allRou[i]->ip);
        free(SelfRouter->allRou[i]);
    }
    free(SelfRouter->allRou);
    for(i=0;i<SelfRouter->numOfAllRouters;i++)
    {
        free(SelfRouter->via[i]);
    }
    free(SelfRouter->via);
    for(i=1;i<SelfRouter->countOfNeighbours;i++)
    {
        free(SelfRouter->allRoutDv[i]);
    }
    free(SelfRouter->allRoutDv);
    free(SelfRouter->neighbors_sent);
    free(SelfRouter->neighbors_finished);
    for(i=1;i<SelfRouter->countOfNeighbours;i++)
    {
        free(SelfRouter->my_neighbors[i]->name);
        SelfRouter->my_neighbors[i]->ip=NULL;
        if(i!=0)
        	free(SelfRouter->my_neighbors[i]);
    }
    free(SelfRouter->my_neighbors);
    free(SelfRouter);
}


int main(int argc, char *argv[]) 
{ 
    	
    	
    	Myrouter * SelfRouter= (Myrouter *) malloc(sizeof(Myrouter));//create the main router
   	 if(SelfRouter==NULL)    
    	{
	printError("malloc error");
    	}
   	 SelfRouter->name=malloc(sizeof(char)*strlen(argv[2])+1);
    
   	if(SelfRouter->name==NULL)    
   	{
		printError("malloc error");
    	}
   	 strcpy(SelfRouter->name,argv[2]);
   	 initilize(SelfRouter ,argv);
   	 SelfRouter->neighbors_sent=calloc(sizeof(int),(SelfRouter->countOfNeighbours-1));
   	 if(SelfRouter->neighbors_sent==NULL)    
    	{
		printError("malloc error");
   	 }
   	 SelfRouter->neighbors_finished=calloc(sizeof(int),(SelfRouter->countOfNeighbours-1));
    	if(SelfRouter->neighbors_finished==NULL)    
   	 {
     	 printError("neighbors malloc error");
    	}
    	pthread_t * thread_receiver = malloc (sizeof(pthread_t)*(SelfRouter->countOfNeighbours-1)); 
   	 if(thread_receiver==NULL)    
    	{
      	  printError("thread rec malloc error");
    	}
    	pthread_t  * thread_sender=malloc(sizeof(pthread_t)*(SelfRouter->countOfNeighbours-1)); 
    	if(thread_sender==NULL)    
    	{
        	printError("thread send malloc error");
    	}
    	pthread_t  * calculator=malloc(sizeof(pthread_t)); 
	if(calculator==NULL)    
	{
       		printError("thread calc malloc error");
    	}
    	SendAndrecv ** dataForThreads = malloc(sizeof(SendAndrecv*)*(SelfRouter->countOfNeighbours-1));
    	if(dataForThreads==NULL)    
   	{
        	printError("malloc error");
    	}
   	SelfRouter->calcurator_finished=0;
   	int  i,numOfattemps=atoi(argv[3]);
	pthread_mutex_t mutx; 
	pthread_cond_t cond1; 
	pthread_cond_t cond2;
	pthread_mutex_init(&mutx, NULL); 
	pthread_cond_init(&cond1, NULL); 
	pthread_cond_init(&cond2, NULL);
	SelfRouter->cond_calc=&cond1;
	SelfRouter->cond_send=&cond2;
	SelfRouter->ChangesInDv=1;
	pthread_mutex_init(&mutx, NULL); 
        SelfRouter->mutx=&mutx;
	if (pthread_create(& calculator[0], NULL,calculate, SelfRouter)<0)	 	//create calculator thread !
    	{  
	    printError("create pthread failed"); 
    	} 
	for(i=1;i<SelfRouter->countOfNeighbours;i++)
	{ 
	    	dataForThreads[i-1] = malloc(sizeof(SendAndrecv));
	    	if(dataForThreads[i-1]==NULL)
	   	 {
	       		 perror("Can't alloceate memory");
                	 exit(1);
	    	}
		dataForThreads[i-1]->name=SelfRouter->name;
		//printf("%s\n",dataForThreads[i-1]->name);
		dataForThreads[i-1]->myPort=SelfRouter->port;
		//printf("%s\n",dataForThreads[i-1]->myPort);
		dataForThreads[i-1]->neighborPort=SelfRouter->my_neighbors[i]->port;
		//printf("%d\n",dataForThreads[i-1]->neighborPort);
		dataForThreads[i-1]->ip=(SelfRouter->ip);
		//printf("%s\n",SelfRouter->my_neighbors[i]->ip);
		dataForThreads[i-1]->router=SelfRouter;
		//printf("%s\n",dataForThreads[i-1]->router);
		dataForThreads[i-1]->index=SelfRouter->my_neighbors[i]->row;
		//printf("%d\n",dataForThreads[i-1]->indexp);
		dataForThreads[i-1]->neighbor_name=SelfRouter->my_neighbors[i]->name;
		//printf("%s\n",dataForThreads[i-1]->neighbor_name);
		dataForThreads[i-1]->numOfAttempts=numOfattemps;
		//printf("%d\n",dataForThreads[i-1]->numOfAttempts);
		dataForThreads[i-1]->dv = SelfRouter->my_dv;
		//for(j=0;j<SelfRouter->countOfNeighbours-1;j++)
		//{
			//printf("%d",dataForThreads[i-1]->dv[j]);
		//}
		dataForThreads[i-1]->dvMatrix = SelfRouter->allRoutDv;
		//printf("\n");
//create sender and reciver threads !!!
		if (pthread_create(&thread_sender[i-1], NULL,sender, dataForThreads[i-1])<0)
		{ 
	            printError("create pthread failed"); 
		} 
		if (pthread_create(&thread_receiver[i-1], NULL,receiver, dataForThreads[i-1])<0)
		{ 

	            printError("create pthread failed"); 
		} 

	}
	//join sender & reciver threads !!!
	for(i=0;i<SelfRouter->countOfNeighbours-1;i++)
	{
		pthread_join(thread_receiver[i], NULL);
		pthread_join(thread_sender[i], NULL);
	}
	if(SelfRouter->countOfNeighbours==1) //in case  there is no neighbors
    	{
        	sleep(2);//for the calculte reach to wait
        	pthread_cond_signal(SelfRouter->cond_calc);
   	}
	pthread_join(calculator[0], NULL);	//join calculator thread !!!
	free(thread_receiver);
	free(thread_sender);
	free(calculator);
	for(i=0;i<SelfRouter->countOfNeighbours-1;i++)
	{
	    free(dataForThreads[i]);
	}
	free(dataForThreads);
        freeAllRouterList(SelfRouter);
	return 0;
}



//---------------------------------------------------------------------------------------------------------------------------//


allRou * createRouter(char *line, Myrouter * SelfRouter, int index)
{
    char * token = strtok(line, " ");
     if(strcmp(token,SelfRouter->name)==0)
     {
        SelfRouter->col=index;
     }
    allRou  * rout = malloc(sizeof(allRou));
    rout->name=malloc(sizeof(char)*strlen(token)+1);
    if(rout->name==NULL )
    {
         printError(" malloc in create router func error error");
    }
    strcpy(rout->name,token);
    token = strtok(NULL, " ");
    rout->ip = malloc (sizeof(char)*strlen(token)+1);
    if(rout->ip==NULL )
    {
         printError(" malloc in create router func error error");
    }
    strcpy(rout->ip,token);
    token = strtok(NULL, " ");
    rout->port=atoi(token);
    rout->index=index;
    return rout;
      
      
}

//------------------------------------------------------------------------------------//
void initilize(Myrouter *SelfRouter,char *argv[] )
{ 
    size_t len = 0;
    ssize_t read;
    FILE * fp;
    char * line = NULL;
    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        perror("can't open file");
        exit(-1);
    }
    char * token;
    getline(&line, &len, fp);
    token = strtok(line, " ");
    SelfRouter->numOfAllRouters = atoi(token);
    int i=0;
    int count=SelfRouter->numOfAllRouters;
    char * name=SelfRouter->name;
    SelfRouter ->allRou= (allRou **) malloc(sizeof(allRou*)*SelfRouter->numOfAllRouters) ;//create allRouter list
    if( SelfRouter ->allRou==NULL)    
   {
        	printError("malloc in initilize func error");
    }
    SelfRouter->my_dv=calloc(sizeof(int),SelfRouter->numOfAllRouters);	//create self dv
    if(SelfRouter->my_dv==NULL)    
   {
        	printError("calloc in initilize func error error");
    }

    SelfRouter->via=calloc(sizeof(char*),SelfRouter->numOfAllRouters);	//create via array
     if(SelfRouter->via==NULL)    
   	{
        	printError(" calloc in initilize func error error");
    	}
    SelfRouter->my_neighbors=malloc(sizeof(Myrouter*));	//create neighbors list
     if(SelfRouter->my_neighbors==NULL)    
   	{
        	printError(" malloc in initilize func error error");
    	}

    while ((read = getline(&line, &len, fp)) != -1&&count!=0)  //create all router
    {
        count--;
        SelfRouter->allRou[i]=createRouter(line,SelfRouter,i);
        i++;
    }
    int countOfNeighbours=0;
    for(i=0;i<SelfRouter->numOfAllRouters;i++)
    {
        SelfRouter->my_dv[i]=INT_MAX;
    }
    while (read !=-1) 
    {  
        token = strtok(line, " ");
        char * c1=token;	
        char * c2="";
        int c3=0,i,port;
        token = strtok(NULL, " ");
        c2=token;
        token = strtok(NULL, " ");
        c3=atoi(token);
        if(strcmp(name,c1)==0||strcmp(name,c2)==0)
        {				
            countOfNeighbours++;	
          			
                SelfRouter->my_neighbors=realloc(SelfRouter->my_neighbors,((countOfNeighbours+1)*sizeof(Myrouter*)));
        	 if(SelfRouter->my_neighbors==NULL)    
   		{
        	printError(" malloc in initilize func error error");
    		}
           	 SelfRouter->my_neighbors[countOfNeighbours]=  malloc(sizeof(Myrouter));
                if(SelfRouter->my_neighbors[countOfNeighbours]==NULL)    
   		{
        	printError(" malloc in initilize func error error");
    		}
    		 SelfRouter->my_neighbors[countOfNeighbours]->name=calloc(sizeof(char),strlen(name)+1);
                 if(SelfRouter->my_neighbors[countOfNeighbours]->name==NULL)    
   		{
        		printError(" malloc in initilize func error error");
    		}
          	  SelfRouter->my_neighbors[countOfNeighbours]->my_dv=calloc(sizeof(int),SelfRouter->numOfAllRouters);
                if(SelfRouter->my_neighbors[countOfNeighbours]->my_dv==NULL)    
   		{
        		printError(" malloc in initilize func error error");
    		}

            
/*
	for(i=0;i<countOfNeighbours;i++)
	{
			printf("%d\n",SelfRouter->my_neighbors[i]->my_dv);
			
			printf("%s\n",SelfRouter->my_neighbors[i]->name);
			
			printf("%s\n",SelfRouter->my_neighbors[i]->ip);

			printf("%d\n",SelfRouter->my_neighbors[i]->port);
			
			printf("%d\n",);
			printf("\n\n\n");
	}
		*/
            for(i=0;i<SelfRouter->numOfAllRouters;i++)	//set the dv in the start to infinity 
            {
                SelfRouter->my_neighbors[countOfNeighbours]->my_dv[i]=INT_MAX;
            }
            if(strcmp(name,c1)==0)
            {
                
                SelfRouter->my_dv[find_index(SelfRouter,c2)]=c3;
                strcpy(SelfRouter->my_neighbors[countOfNeighbours]->name,c2);
            }
            else
            {
                SelfRouter->my_dv[find_index(SelfRouter,c1)]=c3;
                strcpy(SelfRouter->my_neighbors[countOfNeighbours]->name,c1);
            }		//set the dv in the start to infinity 
            SelfRouter->my_neighbors[countOfNeighbours]->row=countOfNeighbours;
            SelfRouter->my_neighbors[countOfNeighbours]->col=find_index(SelfRouter,SelfRouter->my_neighbors[countOfNeighbours]->name);
            SelfRouter->my_neighbors[countOfNeighbours]->my_dv[find_index(SelfRouter,SelfRouter->my_neighbors[countOfNeighbours]->name)]=0;
            SelfRouter->via[find_index(SelfRouter,SelfRouter->my_neighbors[countOfNeighbours]->name)]=malloc(sizeof(char)*strlen(SelfRouter->my_neighbors[countOfNeighbours]->name)+1);
            if(SelfRouter->via[find_index(SelfRouter,SelfRouter->my_neighbors[countOfNeighbours]->name)]==NULL)    
   		{
        		printError(" malloc in initilize func error error");
    		}

            strcpy( SelfRouter->via[find_index(SelfRouter,SelfRouter->my_neighbors[countOfNeighbours]->name)],SelfRouter->my_neighbors[countOfNeighbours]->name);
         
            port=0;
            for(i=0;i<SelfRouter->numOfAllRouters;i++)
            {
                if(strcmp(SelfRouter->allRou[i]->name,SelfRouter->my_neighbors[countOfNeighbours]->name)==0)
                {
                    port=SelfRouter->allRou[i]->port;
                }
            }
            SelfRouter->my_neighbors[countOfNeighbours]->port=port;
           // printf("%d\n",SelfRouter->my_neighbors[countOfNeighbours]->port);
        }
        read = getline(&line, &len, fp);
    }
    
    SelfRouter->countOfNeighbours=countOfNeighbours+1;
    SelfRouter->ip=malloc(sizeof(char)*strlen(SelfRouter->allRou[0]->ip)+1);
    SelfRouter->my_dv[find_index(SelfRouter,name)]=0;
     if( SelfRouter->ip==NULL)
    {
       printError(" malloc in initilize func error error");
    }
    strcpy(SelfRouter->ip,SelfRouter->allRou[0]->ip);
    SelfRouter->port=SelfRouter->allRou[SelfRouter->col]->port;
    SelfRouter->allRoutDv = malloc (sizeof (int*) * (SelfRouter->countOfNeighbours));
    if(SelfRouter->allRoutDv==NULL )
    {
        printError(" malloc in initilize func error error");
    }
    SelfRouter->my_neighbors[0]=SelfRouter; 
    for(i=0;i<SelfRouter->countOfNeighbours;i++)
    {
         SelfRouter->allRoutDv[i]=SelfRouter->my_neighbors[i]->my_dv;
    }

    fclose(fp);//close file
    if (line)
    {
        free(line);
     }  
}


//------------------------------------------------------------------------------------//
void * sender(void * params)
{
    	signal(SIGPIPE, SIG_IGN);
    	SendAndrecv *net_data = params;
    	struct sockaddr_in serv_addr;
    	serv_addr.sin_family = AF_INET;
   	int fd;
  	if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)		//create socket
	{
	    net_data->router->neighbors_finished[net_data->index-1]=SIZE;
            net_data->router->neighbors_sent[net_data->index-1]=SIZE;
            perror("ERROR opening socket");
	    return NULL;
	}
	serv_addr.sin_port=htons(net_data->neighborPort+sumChars(net_data->name));
	serv_addr.sin_addr.s_addr=inet_addr(net_data->ip);
	int i=0,conn,nbytes;
	while(i<net_data->numOfAttempts){	//check if connect the right amuont of times 
		//printf("try to connect to port %d\n",port);
		if((conn = connect(fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)))<0)	//connect to socket
		{
			//fprintf(stderr,"ERROR connecting\n");
			sleep(1);
	
		}else 
			break;

		i++;
	}
	if(conn<0)
	{
		close(fd);
		printError("connection  failed\n");
		close(fd);
		//exit(0);

	}//else
		//printf("Connected !!!!\n");
	
	int *  arrayToSend = malloc(sizeof(int32_t)*(net_data->router->numOfAllRouters+1));
	if( arrayToSend==NULL )
        {
       		perror("malloc in senther func error");
        	return NULL;
    	}
    	//int	countSame;
	pthread_mutex_lock(net_data->router->mutx); 
    	while(1)
	{         
	   
  	    	nbytes=1;
        	for (i=1 ; i < net_data->router->numOfAllRouters+1 ; ++i)
       		 {
        	    arrayToSend[i] =htonl(net_data->dv[i-1]) ;
        	}
        	if(net_data->router->ChangesInDv)
        	{
        		    arrayToSend[0]=htonl(1);
        		    //countSame++;
        	}
        	else
        	    arrayToSend[0]=htonl(0);
        	// printf("sending! ");	///send dv to socket
  		if((nbytes =send(fd,arrayToSend,sizeof(int32_t)*(net_data->router->numOfAllRouters+1),0) ==-1))
        	{
        	    perror("sending error!");
        	    pthread_mutex_unlock(net_data->router->mutx);
        	    close(fd);
        	    free(arrayToSend);
    		    return NULL;
        	}
        	else
        	{		// if we got answer fron clculator that it finished we exit
        	            if(net_data->router->calcurator_finished)
	                    {
	    	               pthread_mutex_unlock(net_data->router->mutx);
	    	               close(fd);
	    	               free(arrayToSend);
	    	               return NULL;
	    	            }

        	}//else
			//pthread_cond_signal(ctual_args->router->cond_send);
          	pthread_cond_wait(net_data->router->cond_send, net_data->router->mutx);//wait for the calculator to end 
		}
  	close(fd);
	return NULL;
}
//-----------------------------------------------------------------------------------------------------------------//
int sumChars(char * str)
{
    int i,sum=0, len = strlen(str);

    for ( i = 0; i < len; i++)
    {
        sum = sum + str[i];
    }
    return sum;
}
//----------------------------------------------------------------------------------------------------------------------//
void * receiver(void * params)
{  

    signal(SIGPIPE, SIG_IGN);
    struct sockaddr_in cli_addr;	      
    socklen_t cli_len = sizeof(cli_addr);	
    SendAndrecv *net_data = params;
    int nbytes=1, i = 0,counter;
    int fd,newfd;		
   /// create the socket 
    if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        net_data->router->neighbors_finished[net_data->index-1]=SIZE;
        net_data->router->neighbors_sent[net_data->index-1]=SIZE;
	    perror("Can't create TCP socket");
	    return NULL;
    }
	struct sockaddr_in srv;	
    srv.sin_family = AF_INET; 
    srv.sin_port = htons(net_data->myPort+sumChars(net_data->neighbor_name)); // bind socket  client 
    srv.sin_addr.s_addr = INADDR_ANY;
    if(bind(fd, (struct sockaddr*) &srv, sizeof(srv)) < 0)
    {
    	perror("ERROR in binding\n"); 
    	return NULL;
    }
    //printf("bind done listening on port: %d\n", srv.sin_port);
    if(listen(fd, 4) < 0) //listen queue 
    {
	    perror("listen failed\n");
    	return NULL;
    }
    newfd = accept(fd, (struct sockaddr*) &cli_addr, &cli_len);//accept the connection
    if(newfd < 0)
    {
	    perror("accept failed");
	    return NULL;
    }//}else
		//puts("Connection accepted\n");
			
    
    int * inArr = malloc(sizeof(int32_t)*(net_data->router->numOfAllRouters+1)) ;
    if( inArr==NULL )
    {
        perror("malloc in reciver thread func error");
        return NULL;
    }
    while(1)
    {	//recive dv from client(sender)!!
        if((nbytes =recv(newfd, inArr, sizeof(int32_t)*(net_data->router->numOfAllRouters+1), MSG_WAITALL))==0)
        {
            net_data->router->neighbors_finished[net_data->index-1]=SIZE;
            net_data->router->neighbors_sent[net_data->index-1]=SIZE;
            free(inArr);
            close(fd);
            return NULL;
        }else if(nbytes<0)
        {
   	
	     net_data->router->neighbors_finished[net_data->index-1]=SIZE;
             net_data->router->neighbors_sent[net_data->index-1]=SIZE;
             free(inArr);
             close(fd); 	
	     perror("recv failed!\n");
	     return NULL;
	    }
        net_data->router->neighbors_sent[net_data->index-1]=1;
        if(ntohl(inArr[0])==0)
        {
            net_data->router->neighbors_finished[net_data->index-1]=1;
        }
        else
            for(i=1;i<net_data->router->numOfAllRouters+1;i++)
            {
                net_data->dvMatrix[net_data->index][i-1]=ntohl(inArr[i]);
            }
         counter=0;
        for(i=0;i<net_data->router->countOfNeighbours-1;i++)
        {
            if( net_data->router->neighbors_sent[i]>0)
                counter++;
        }
        if(counter==net_data->router->countOfNeighbours-1)
        {
            sleep(1);
            pthread_cond_signal(net_data->router->cond_calc);//release calculator
        }
    }
    
    close(fd);
    return NULL;
}

//-----------------------------------------------------------------------------------------------------------------//
int find_index(Myrouter * SelfRouter,char * name)
{
    int i;
    for(i=0;i<SelfRouter->numOfAllRouters;i++)
    {
        if(strcmp(name,SelfRouter->allRou[i]->name)==0)
        {
            return SelfRouter->allRou[i]->index;
        }
           
    }
    return -1;
}


//-----------------------------------------------------------------------------------------------------------------//
void * calculate(void * params)
{
    int i=1,j=0;
    int count;
    sleep(1);
    while(1)
    {
        Myrouter * myRout = params;
        i=1;j=0;count=0;
        pthread_cond_wait(myRout->cond_calc, myRout->mutx); //wait to recive dv

        for(i=0;i<myRout->countOfNeighbours-1;i++)
        {
           // printf("%d\n",myRout->neighbors_finished[i]);
            if( myRout->neighbors_finished[i]>0)
                count++;
        }        myRout->ChangesInDv=0;
        for(i=1;i<myRout->countOfNeighbours;i++)
        {
            j=0;
            for(;j<myRout->numOfAllRouters;j++)
            {//cheak the if dv has shorter way 
           	 //printf("%d+%d",myRout->allRoutDv[i][j],myRout->my_dv[find_index(myRout,myRout->my_neighbors[i]->name)]);
           	// printf("%d\n",(myRout->my_dv[j]);
                if(myRout->my_dv[j]>myRout->allRoutDv[i][j]+myRout->my_dv[find_index(myRout,myRout->my_neighbors[i]->name)]&&myRout->allRoutDv[i][j]+myRout->my_dv[find_index(myRout,myRout->my_neighbors[i]->name)]>-1)
                {
                    myRout->ChangesInDv=1;//say that there was changes in the dv!
                    myRout->my_dv[j]=myRout->allRoutDv[i][j]+myRout->my_dv[find_index(myRout,myRout->my_neighbors[i]->name)];
                    myRout->allRoutDv[0]= myRout->my_dv;
                     free(myRout->via[j]);
                    myRout->via[j]=malloc(sizeof(char)*strlen(myRout->my_neighbors[i]->name)+1);
                    if(strcmp(myRout->via[find_index(myRout,myRout->my_neighbors[i]->name)],myRout->my_neighbors[i]->name)==0)//check that via array is the correct one
                         strcpy(myRout->via[j],myRout->my_neighbors[i]->name);
                    else 
                    {
                        strcpy(myRout->via[j],myRout->via[find_index(myRout,myRout->my_neighbors[i]->name)]);
                    }
              
                }
            }
        }
          if(count==myRout->countOfNeighbours-1||myRout->countOfNeighbours==1)
                {
                    
                    myRout->ChangesInDv=0;
                    myRout->calcurator_finished=1;
                    pthread_cond_broadcast(myRout->cond_send);
                    printf("from: %s\n",myRout->name); 
                    for(i=0;i<myRout->numOfAllRouters;i++)
                    {
                       
                        if(i!=myRout->col)	//print destination via weight
                        {
                            int x=myRout->my_dv[i];
                            if(x!=INT_MAX)
                                printf("\nto: %s via: %s weight: %d \n",myRout->allRou[i]->name,myRout->via[i],myRout->my_dv[i]);
                            else
                            printf("\nto: %svia: %s weight: inf \n",myRout->allRou[i]->name,myRout->via[i]);
                        }

                        
                    }
                    pthread_mutex_unlock(myRout->mutx);//unlock mutex after we change the dv
                    return NULL;
                }
        for(i=0;i<myRout->countOfNeighbours-1;i++)
        {
            myRout->neighbors_sent[i]--;

        }
         pthread_cond_broadcast(myRout->cond_send);//relase sender
    }
    return NULL;
}


#endif

