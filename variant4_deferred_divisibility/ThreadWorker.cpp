#include "ThreadWorker.h"
#include "Task.h"

#include <thread>

ThreadWorker::ThreadWorker(TaskQueue& q)
    : queue(q), t(&ThreadWorker::run, this) {}

ThreadWorker::~ThreadWorker(){
    if(t.joinable()) t.join();
}

void ThreadWorker::run(){
    Task task;
    while (queue.pop(task)){
        doTask(task);
    }
}

void ThreadWorker::doTask(const Task& task){
    for (std::size_t p = task.start; (p <= task.end) and (shared_bool); p+=2){
        if (task.n % p == 0) {
            this->shared_bool = false;
        }
    }
    queue.task_done();
}