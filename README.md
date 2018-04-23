## Newsarticle program (c++ project)
------------------------------------
- The directory clientserver contains the communication classes Connection and Server,
  and the auxiliary classes ConnectionClosedException and Protocol. These
  classes are defined in the project description, sections 5.2 and 6.
- 'make' makes the library libclientserver.a.
- The subdirectory 'test' contains the test programs myclient and myserver,
  as defined in the project description, section 5.3.
- 'make' makes the executables myclient and myserver.

- To build the client and server, go to NewsApplication/clientserver/src and run 'make'.
- To start the server with the primary memory database run ./mainserver 'port-nbr'.
- To start the server with disk memory database run ./mainserver 'port-nbr' "disk".
------------------------------------
