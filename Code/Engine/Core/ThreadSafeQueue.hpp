#pragma once

#include <queue>

#include "Engine/Core/CriticalSection.hpp"

template<typename T>
class ThreadSafeQueue {
public:
    
    bool empty() {
        _cs.enter();
        bool result = _internal_queue.empty();
        _cs.leave();
        return result;
    }
    bool pop(T& out) {
        if(empty()) {
            return false;
        }
        _cs.enter();
        out = _internal_queue.front();
        _internal_queue.pop();
        _cs.leave();
        return true;
    }
    void push(const T& in) {
        _cs.enter();
        _internal_queue.push(in);
        _cs.leave();
    }
    T front() {
        if(empty()) {
            return std::move(T());
        }
        _cs.enter();
        auto result = _internal_queue.front();
        _cs.leave();
        return std::move(result);
    }
    std::size_t size() {
        _cs.enter();
        auto result = _internal_queue.size();
        _cs.leave();
        return std::move(result);
    }
protected:
private:
    std::queue<T> _internal_queue;
    CriticalSection _cs;
};


