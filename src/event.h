#pragma once

#include <cassert>
#include <functional>
#include <vector>

/**
 * An event. The type arguments are the arguments that are passed to the event handlers.
 * Subscribing will return an id that can be used to unsubscribe. Called like a function to invoke
 * the subscribed handlers.
 */
template <class... TArgs>
class Event {
public:
    size_t subscribe(std::function<void(TArgs...)> callback) {
        assert(!walking_ && "Do not subscribe while signaling");
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
        // signaling
        assert(!walking_ && "Do not unsubscribe while signaling");
        assert(id < freeItems_.size());
        callbacks_[id] = nullptr;
        freeItems_.push_back(id);
    }

    void operator()(TArgs const&... args) {
        // We should not allow modification of the signal while we're firing.
        walking_ = true;
        for(auto && callback : callbacks_) {
            if (callback)
            {
                callback(args...);
            }
        }
        walking_ = false;
    }

private:
    std::vector<std::function<void(TArgs...)>> callbacks_;
    std::vector<size_t> freeItems_;
    bool walking_ {false};
};

/**
 * An event with a friend. The T parameter indicates a class capable of calling the event.
 * @seealso Event
 */
template<class TFriend, class... TArgs>
class EventF {
public:
    size_t subscribe(std::function<void(TArgs...)> callback) {
        return signal_.subscribe(std::move(callback));
    }

    void unsubscribe(size_t id) {
        signal_.unsubscribe(id);
    }

private:
    friend TFriend;

    void operator()(TArgs const&... args) {
        signal_(args...);
    }

    Event<TArgs...> signal_;
};