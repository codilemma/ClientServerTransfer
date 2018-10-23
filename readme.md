## Client-Server File Transfer

  A simple client-server project was designed using sockets.  The client server pair was then used to transfer a file from the Server to the Client.

Client-Server communication can be described as follows:.

- Server (passive socket):     
  - 1) Create a socket
  - 2) Assign and address and port to socket
  - 3) Set socket to listen
  - 4) Accept new connection
  - 5) Communicate with the client (transfer the file)
  - 6) Close the connection

- Client (active socket):  
  - 1) Create a TCP socket
  - 2) Request a connection from the server (the client is then assigned a port by its host computer)
  - 3) Communicate with the server (recieve the file)
  - 4) Close the connection

## References

**1) Introduction to Sockets Programming in C using TCP/IP**
 - https://www.csd.uoc.gr/~hy556/material/tutorials/cs556-3rd-tutorial.pdf 

**2) Socket Programming Tutorial In C For Begginers**
- https://www.youtube.com/watch?v=LtXEMwSG5-8 

**3) Socket Programming In C on Linux**
- https://www.binarytides.com/socket-programming-c-linux-tutorial/ 

**4) Socket Programming**
- https://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming.html 

**4) TCP Echo Server**
- http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoServer.c 