all:    client-processes server                                                                      
client-processes: client-processes.c
	gcc -std=gnu99 -Wall $< -o $@
 
server: server.c 
	gcc -std=gnu99 -Wall $< -o $@
       
clean:
	rm -f client-processes server *.o *~ core
