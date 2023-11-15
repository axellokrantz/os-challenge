# Axel Månson Lokrantz, s232081
## Experiment: Thread Pooling and Priority Queue vs. Threads and Priority Queue

The experiment which was conducted was to see if a thread pool could increase the performance of the program. The two branches which were compared were 'BRANCH 1' and 'BRANCH 2'. A thread pool is a model for achieving concurrent execution. It can hold multiple threads waiting for tasks to be allocated to them. By maintaining a pool, the model could potentially increase performance and avoid latency. Opposed to a regular thread implementation, a thread pool does not create and destroy a thread each time it handles a task. Instead a thread pool reuses previously created threads to execute current tasks to diminish the overhead associated with creating and destroying threads. Since the thread already exist when the request arrives, the delay introduced by thread creation is eliminated, which, in theory, should make the program more responsive.

## Setup:
