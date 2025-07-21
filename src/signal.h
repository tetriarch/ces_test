#pragma once

#include <cassert>
#include <functional>
#include <vector>

template <class... TArgs>
class Signal {
public:
    size_t subscribe(std::function<void(TArgs...)> callback) {
        if (freeItems_.empty()) {
            callbacks_.emplace_back(callback);
            return callbacks_.size() - 1;
        }

        auto idx = freeItems_.back();
        freeItems_.pop_back();
        callbacks_[idx] = std::move(callback);
        return idx;
    }

    void unsubscribe(size_t id) {
        assert(id < freeItems_.size());
        callbacks_[id] = nullptr;
        freeItems_.push_back(id);
    }

    void fire(TArgs const&... args) {
        for(auto && callback : callbacks_) {
            callback(args...);
        }
    }

private:
    std::vector<std::function<void(TArgs...)>> callbacks_;
    std::vector<size_t> freeItems_;
};