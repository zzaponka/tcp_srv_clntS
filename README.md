# tcp_srv_clntS
Simple multithreading TCP server/client on local machine with C sockets

How to get everything run:

$ gcc ./server.c -o ./server -lpthread
$ gcc ./client.c -o ./client -lpthread

Then just run 'server' and 'client' apps in different consoles.
To stop server, press 'Ctrl+C'.
