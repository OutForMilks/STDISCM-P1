#include "TaskQueue.h"
#include "Task.h"
#include <stdexcept>

void TaskQueue::push(const Task& task){
    {
        std::lock_guard<std::mutex> lock(mut);
        if (closed) throw std::runtime_error("Push on closed TaskQueue");
        tasks.push(task);
        pending++;
    }
    cv.notify_one();
}

bool TaskQueue::pop(Task& task){
    std::unique_lock<std::mutex> lock(mut);
    
    cv.wait(lock, [this] { return closed || !tasks.empty();});

    if (tasks.empty()) return false;
    
    task = std::move(tasks.front());
    tasks.pop();
    return true;
}

void TaskQueue::close(){
    {
        std::lock_guard<std::mutex> lock(mut);
        closed = true;
    }
    cv.notify_all();
}

void TaskQueue::task_done(){
    bool all_done;
    {
        std::lock_guard<std::mutex> lock(mut);
        pending--;
        all_done = (pending == 0);
    }
    if (all_done) done_cv.notify_all();
}

void TaskQueue::wait_all(){
    std::unique_lock<std::mutex> lock(mut);
    done_cv.wait(lock, [this] { return pending == 0;});
}
