# SOPE - MP2-1

## Description

This project is based on a client-server application capable of handling requests for the
execution of different load tasks. In this 1st stage, the Client code was developed.

### Compile and Run 

Run `make` on the root folder of the project to compile. 
The Client invoked with the command: `c <-t nsecs> <fifoname>`

### Code Structure

Source files:
```
|- client.c           
|- client_signals.c          
|- client_utils.c                  
```

### Implementation Details

* In order to ensure the main thread handles both SIGPIPE and SIGALRM signals, the request threads block those signals.
* In the sigpipe_handler() function we are sending signals to threads to finish them, if a request has already been made to the Server, the signal is blocked.
* When timeout occurs, the threads are cancelled and the function thread_gavup(), pushed to the thread cleanup stack, accuses the GAVUP event.

### Group Members and Self Evaluation

André de Jesus Fernandes Flores,	201907001 - 25%  
Diogo Luís Araujo de Faria,	201907014 - 25%  
Maria Marta Nunes Andrade Lobo dos Santos,	201604530 - 25%  
Tiago Andre Batista Rodrigues,	201906807 - 25% 
