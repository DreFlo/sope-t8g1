# SOPE - MP2-2

## Description

This project is based on a client-server application capable of handling requests for the
execution of different load tasks. In this 2st stage, the Server code was developed.

### Compile and Run 

Clone the project and run `make` on its root folder to compile.   
The Server is invoked with the command:  
`./s <-t nsecs> [-l nbufsz] <fifoname>`

### Code Structure

Source files:
```
|- server.c           
|- server_signals.c          
|- server_utils.c                  
```

### Implementation Details

* For synchronization, we resort to mutexes to access the buffer and avoid memory conflicts.
* We are creating a file to store all the thread IDs which is also used to destroy the worker threads at the end 
* Unfourtunately, we could not solve a problem regarding the FAILD event, which leads to the fact that some producer threads are terminating before expected.

### Group Members and Self Evaluation

André de Jesus Fernandes Flores,	201907001 - 25%  
Diogo Luís Araujo de Faria,	201907014 - 25%  
Maria Marta Nunes Andrade Lobo dos Santos,	201604530 - 25%  
Tiago Andre Batista Rodrigues,	201906807 - 25% 
