#omri mizrahi
#303082549
#ex3a


app: ex3.o
	gcc -Wall -Wvla -g ex3.c -o app -lpthread
	

BF.o: ex3.h ex3.c
	gcc -Wall -c -g ex3.c 

clean:	
	rm ex3.o app
	echo Clean done
