# Axel Månson Lokrantz, s232081
## 1. Experiment

The experiment which was conducted was to see if a thread pool could increase the performance of the program. The two branches which were compared are 'milestone_threads_priority' and 'milestone_threadpool_priority'. A thread pool is a model for achieving concurrent execution. It can hold multiple threads waiting for tasks to be allocated to them. By maintaining a pool, the model could potentially increase performance and avoid latency. Opposed to a regular thread implementation, a thread pool does not create and destroy a thread each time it handles a task. Instead a thread pool reuses previously created threads to execute current tasks to diminish the overhead associated with creating and destroying threads. Since the thread already exist when the request arrives, the delay introduced by thread creation is eliminated which, in theory, should make the program more responsive.

## 2. Setup

The experiment was performed with the built-in request generator where the client generates reverse hashing requests tailored to specific command line arguments. The generator takes 9 command line arguments as described below:

- **Hostname**: This is the hostname or IP of the server. In this experiment 192.168.101.10 will be used which is the same ip address used by 'run-client-final.sh'.

- **Port**: This is the port number of the server. The experiment will use port 5003.

- **Seed**: This is the seed for the random number generator. The experiment will use the seed '5041', which is the initial seed used by 'run-client-final.sh'.

- **Total**: This is the total number of reverse hashing requests to be generated. A higher number will yield a more precise score, however for benchmark purposes, the experiment will use a total of '100' reverse hashing requests.

- **Start**: Hashes will be generated from input numbers that are greater or equal to start. The experiment will be set to 0 which randomizes 'start' for each request.

- **Difficulty**: This is the difficulty of the generated hashes. The experiment will use 30000000, which is the same difficulty used by 'run-client-final.sh'.

- **Rep**: This is the repetition probability percentage (%). Since this iteration of the program does not store any of the hash values, the percentage does not matter. For simplicity, 'rep' will be set to 20, which is the same number used by 'run-client-final.sh'.

- **Delay**: This is the delay between requests in microseconds (usec). Varying the delay parameter will test how the two implementations behave under different levels of load. A lower delay will send requests at a higher rate and a higher delay will send requests at a lower rate. By observing how the system responds to different delay parameters, the performance of the two implementations can be evaluated. One might perform better under moderate loads but struggle under heavier traffic, while the other might handle high loads efficiently due to its optimized resource management. Therefore, in this experiment, the following delay parameters will be benchmarked: 800000, 700000, 600000, 500000, and 400000 (usec). Additionally, both implementations will be benchmarked with extremely small delay values ranging from 60000 to 6.

- **Lambda**: The priority level of each request is generated randomly. There are 16 priority levels, where 16 correspond to the highest priority and vice versa. In this experiment, lambda was set to 1.5, which is the same number used by 'run-client-final.sh'.

The command line argument used for this experiment will be as follows (where the delay parameter will vary depending on the benchmark).
```
$ ./client 192.168.101.10 5003 5041 100 0 30000000 20 800000 1.5
```

## 3. Result

The results of the benchmarks for the different delay values can be seen below. For each delay value, 10 runs were conducted and the best, average and worst scores were subsequently calculated.

**Table 1: Average, best and worst scores for moderate to high delay values.**
| Delay μs | Threadpooling + Priority (avg. score) | Best score | Worst score | Threads + Priority (avg.score) | Best Score | Worst Score |
|----------|---------------------------------------------|---------------------------------------------|---------------------------------------------|---------------------------------------------|---------------------------------------------|-------------------|
| 400000   | **34790198**                                    | 32293945                                             | 36223244                                              | **36063471**                                | 34244711                                      | 40019967                                        |
| 500000   | **30300986**                                    | 29199252                                             | 33221529                                              | **36049598**                                | 35000112                                      | 39248819                                      | 
| 600000   | **27153616**                                    | 22981129                                              | 29221646                                             | **28967037**                                |24114199                                     | 32114386                                      | 
| 700000   | **19486033**                                    | 18477231                                            | 20009892                                             | **19220121**                                | 18148118                                      | 24000462                                       |
| 800000   | **13947025**                                    | 12994219                                             | 15911143                                             | **16367480**                                | 16012897                                     | 16924855                                      |

**Table 2: Reliability for small delay values.**
| Delay μs | Threadpooling + Priority Queue (relyability) | Threads + Priority Queue (relyability) |
|--------------|--------------------------------|--------------------------|
| 6      | 100%                                | 72%                          |
| 60      | 100%                                | 91%                          |
| 600      | 100%                                | 94%                          |
| 6000      | 100%                                | 100%                          |
| 60000      | 100%                                | 100%                          |


## 4. Conclusion

The comparison between the thread pool and the traditional thread implemention across various delay values reveal nuanced performance differences. Under moderate to high delays (400,000 to 800,000 μs), the threadpool outperformed the traditional threads, displaying lower average scores, which indicates better performance in these scenarios. This suggested that the advantages of a thread pool outweighed the disadvantages associated with the overhead of managing it. At small delay values, the thread pool was superior in terms of reliability maintaning 100% reliability from 6 μs to 60,000 μs. The reliability of the regular threads fluctuaded, dropping to 72% at 6 μs. At a delay of 700,000 μs, the traditional threads displayed better average performance compared to the thread pool. However, it is crucial to note that additional trials would help account for variability and ensure a more statistically significant analysis of the performance differences between the two approaches.

Even though the thread pool had a better average score in 4 out of the 5 measurements conducted in table 1, the group decided not to implement it. The reasoning was that the overlapping score ranges, especially where the thread pool's worst score was surpassed by at least one measurement of the traditional threads across all delay benchmarks, suggested insufficient justification for implementing the thread pool. Unfortunately, the reliability experiment presented in Table 2 was conducted after the deadline of the code. With the reliability measurements at hand during the decision-making process, the group would have made a different decision. The thread pool's consistent 100% reliability across all small delay benchmarks outweighs all other measurements, considering that achieving 100% reliability was one of the fundamental requirements of the project. Furthermore, the program has a 'MAXPENDING' constant which dictates the pending connections. When lowering the constant to 50, 25 and 10 both implementations managed to get a 100% reliance without timeouts from the client.

A general estimate of the ideal number of threads in a thread pool is usually influenced by various factors such as CPU count and the nature of tasks. For CPU-bound tasks, the optimal thread count is often close to the number of avaliable cores, while for IO-bound tasks, a larger number of threads can be beneficial. Generally, brute-forcing a hash value can be a CPU-heavy task because it involves repetitive hashing operations. To determine the optimal number of threads for a thread pool in a program run on the server environment with 17 CPUs and 2 threads per core a good starting point would have been:

Total Threads = 17 CPUs * 2 Threads per Core = 34 Threads. However, this number is speculative since the implementation only was benchmarked on our local system and not on the server environment. Factors such as, the nature of the task, memory usage and other concurrent processes running on the server play a large role. Therefor trial and error on the server environment would have been necessary to fine-tune the optimal number of threads.

