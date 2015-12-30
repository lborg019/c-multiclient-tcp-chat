all:    client-multithreaded server                                                                      
client-multithreaded: client-multithreaded.c
	gcc -lpthread -std=gnu99 -Wall $< -o $@
 
server: server.c 
	gcc -std=gnu99 -Wall $< -o $@
       
clean:
	rm -f client-multithreaded server *.o *~ core
