# Axel Månson Lokrantz, s232081
## 1. Experiment

The experiment which was conducted was to see if a thread pool could increase the performance of the program. The two branches which were compared are 'BRANCH 1' and 'BRANCH 2'. A thread pool is a model for achieving concurrent execution. It can hold multiple threads waiting for tasks to be allocated to them. By maintaining a pool, the model could potentially increase performance and avoid latency. Opposed to a regular thread implementation, a thread pool does not create and destroy a thread each time it handles a task. Instead a thread pool reuses previously created threads to execute current tasks to diminish the overhead associated with creating and destroying threads. Since the thread already exist when the request arrives, the delay introduced by thread creation is eliminated, which, in theory, should make the program more responsive.

## 2. Setup

The experiment was performed with the built-in request generator where the client generates reverse hashing requests tailored to specific command line arguments. The generator takes 9 command line arguments as described below:

- **Hostname**: This is the hostname or IP of the server. In this experiment 192.168.101.10 will be used which is the same ip address used by 'run-client-final.sh'.

- **Port**: This is the port number of the server. The experiment will use port 5003.

- **Seed**: This is the seed for the random number generator. The experiment will use the seed '5041', which is the initial seed used by 'run-client-final.sh'.

- **Total**: This is the total number of reverse hashing requests to be generated. A higher number will yield a more precise score, however for benchmark purposes, the experiment will use a total of '100' reverse hashing requests.

- **Start**: Hashes will be generated from input numbers that are greater or equal to start. The experiment will set it to 0 which randomizes 'start' for each request.

- **Difficulty**: This is the difficulty of the generated hashes. The experiment will use 30000000, which is the same difficulty used by 'run-client-final.sh'.

- **Rep**: This is the repetition probability percentage (%). Since this iteration of the program does not store any of the hash values, the percentage does not matter. For simplicity, 'rep' will be set to 20, which is the same number used by 'run-client-final.sh'.

- **Delay**: This is the delay between requests in microseconds (usec). Varying the delay parameter will test how the two implementations behave under different levels of load. A lower delay will send requests at a higher rate and a higher delay will send requests at a lower rate. By observing how the system responds to different delay parameters, the performance of the two implementations can be evaluated. One might perform better under moderate loads but struggle under heavier traffic, while the other might handle high loads efficiently due to its optimized resource management. Therefore, in this experiment, the following delay parameters will be tested: 800000, 700000, 600000, 500000, and 400000 (usec).

- **Lambda**: The priority level of each request is generated randomly. There are 16 priority levels, where 16 correspond to the highest priority and vice versa. In this experiment, lambda was set to 1.5, which is the same number used by 'run-client-final.sh'.

The command line argument used for this experiment will be as follows (where the delay will vary from 400000 to 800000.

```
$ ./client 192.168.101.10 5003 5041 100 0 30000000 20 800000 1.5
```

## 3. Result

| Delay μs | Threadpooling + Priority Queue (avg. score) | Threads + Priority Queue (avg. score) |
|--------------|--------------------------------|--------------------------|
| 400,000      |34790198                                |46063471                          |
| 500,000      |55110702                                |46049598                          |
| 600,000      |27153616                                |29967037                          |
| 700,000      |57357754                                |19220121                          |
| 800,000      |13947025                                |16367480                          |


## 4. Conclusion
s
