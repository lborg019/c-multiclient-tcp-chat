all:    client-multithreaded server                                                                      

client-multithreaded: client-multithreaded.c
	gcc -std=gnu99 -pthread -Wall $< -o $@
 
server: server.c 
	gcc -std=gnu99 -Wall $< -o $@
       
clean:
	rm -f client-multithreaded server *.o *~ core
