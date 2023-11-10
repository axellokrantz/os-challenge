#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/sha.h>
#include <endian.h>
#include <pthread.h>

#define MAXPENDING 200    // Maximum outstanding connection requests - possibly unlimited
#define PRIORITIES 16
#define THREAD_POOL_SIZE 5 // CPU cores + 1

pthread_cond_t taskReady = PTHREAD_COND_INITIALIZER;
pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t threadPool[THREAD_POOL_SIZE];

typedef struct ThreadArgs { // Wrapping the client socket in a structure to ensure correct type (void pointer)
    int clntSock;
} ThreadArgs;

typedef struct {
    uint8_t hash[32];
    uint64_t start;
    uint64_t end;
    uint8_t priority;
    int clntSock;
} Request;

typedef struct Node {
    Request data;
    struct Node* next;
}Node;

Node* priorityQueue[PRIORITIES];

uint64_t reverse_hash(uint8_t hash[SHA256_DIGEST_LENGTH], uint64_t start, uint64_t end){
    uint8_t candidate[sizeof(uint64_t)];
    uint8_t output[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;

    for(uint64_t i = start; i < end; i++){
        uint64_t little_endian_i = htole64(i);
        memcpy(candidate, &little_endian_i, sizeof(uint64_t));
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, candidate, sizeof(candidate));
        SHA256_Final(output, &sha256);

        if(memcmp(hash, output, SHA256_DIGEST_LENGTH) == 0) 
            return i;
    }

    puts("No corresponding hash");
    return 0;
}

void enqueue(Request request){
    int priority = request.priority-1;
    Node* node = (Node*)malloc(sizeof(Node));
    node -> data = request;
    node -> next = NULL;

    if(priority >= 0 && priority <= 15){
        pthread_mutex_lock(&queueMutex);

        if(priorityQueue[priority] == NULL){
            priorityQueue[priority] = node;
        }
        else{
            Node* current = priorityQueue[priority];
            while(current -> next != NULL)
                current = current -> next;
            current -> next = node;
        }

        pthread_mutex_unlock(&queueMutex);
    }
}

Request dequeue(){
    Request request;
    pthread_mutex_lock(&queueMutex);

    int i;
    for(i = 15; i >= 0; i--){
        if(priorityQueue[i] != NULL){
           break;
        }
    }

    if(i < 0){ // No requests in the queue
        pthread_mutex_unlock(&queueMutex);  
        request.priority = 0;
        return request;
    }

    Node* head = priorityQueue[i];
    request = head -> data;
    priorityQueue[i] = head -> next;
    free(head);
    pthread_mutex_unlock(&queueMutex);  
    return request;
}

void *processTasks(void *threadArgs){
    while(1){
        printf("process task\n");
        pthread_mutex_lock(&queueMutex);
        Request request;
        request.priority = 0;
        while(request.priority == 0){
            request = dequeue();
            if(request.priority == 0){
                pthread_cond_wait(&taskReady, &queueMutex);
            }
        }
        printf("HELLO?\n");
        pthread_mutex_unlock(&queueMutex);
        uint64_t answer = reverse_hash(request.hash, request.start, request.end);
        answer = htobe64(answer);
        write(request.clntSock, &answer, sizeof(answer));
        close(request.clntSock);
    }
    return NULL;
   
}

void *ThreadMain(void *threadArgs){
    int clntSock = ((ThreadArgs *) threadArgs) -> clntSock; // type casting 
    free(threadArgs);
    
    uint8_t hash[32];
    uint64_t start;
    uint64_t end;
    uint8_t p;

    read(clntSock, hash, 32);
    read(clntSock, &start, 8);
    read(clntSock, &end, 8);
    read(clntSock, &p, 1);
    
    end = be64toh(end);
    start = be64toh(start);
    Request request;
    memcpy(request.hash, hash, sizeof(hash));
    request.start = start;
    request.end = end;
    request.priority = p;
    request.clntSock = clntSock;
    enqueue(request);
    pthread_cond_signal(&taskReady);
    return NULL; //if we want to we can use the exit code to deliver a message to a parent
}

int main(int argc, char *argv[]) {


    if (argc != 2) {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    unsigned short servPort = atoi(argv[1]);  // First arg:  local port

    // Create socket for incoming connections
    int servSock;
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        perror("socket() failed");
        exit(1);
    }

    //printf("Socket created successfully.\n");

    // Construct local address structure
    struct sockaddr_in servAddr;
    //printf("The socket number: [%d]\n", servSock);
    memset(&servAddr, 0, sizeof(servAddr)); // Clean the struct before usage, fill with zeroes
    servAddr.sin_family = AF_INET;                // Internet address family, set the family to IPV4
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface, htonl() convert values between host and network byte order
    servAddr.sin_port = htons(servPort);          // Local port, htons() - convert values between host and network byte order

    // Bind to the local address
    if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0){  // bind()
        // param 1 = socket to bind
        // param 2 =  pointer to servadress (typecasted to sockaddr)
        // param 3 = length of addr struct)
        perror("bind() failed");
        close(servSock);
        exit(1);
    }
        //printf("Binding to the local address was successful.\n");

    // Mark the socket so it will listen for incoming connections
    if (listen(servSock, MAXPENDING) < 0){
        perror("listen() failed");
        close(servSock);
        exit(1);
    }
    
    //printf("Listening for incoming connections...\n");

    // Initializing priority que
    for(int i = 0; i < PRIORITIES; i++)
        priorityQueue[i] = NULL;

    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        if (pthread_create(&threadPool[i], NULL, processTasks, NULL) != 0) {
            perror("pthread_create() failed");
            exit(1);
        }
    }

    while (1) {   // Run forever
        struct sockaddr_in clntAddr;     // Client address
        socklen_t clntLen = sizeof(clntAddr);  // Length of client address data structure

        // Wait for a client to connect
        int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntLen);

        if (clntSock < 0){
            perror("accept() failed");
            continue;
        }
            
        printf("Accepted connection from client.\n");

        ThreadArgs *threadArgs = (ThreadArgs *) malloc(sizeof(ThreadArgs));
		threadArgs->clntSock = clntSock;

        pthread_t threadID;
        int returnValue = pthread_create(&threadID, NULL, ThreadMain, threadArgs);
        
        if (returnValue != 0){
            printf("pthread_create() failed");
            exit(1);
        }

        printf("Created thread successfully.\n");
    }

    close(servSock); //close server socket when terminating
    return 0;
}
