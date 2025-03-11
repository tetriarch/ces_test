#pragma once

#include <functional>

class Scoped {

public:
    Scoped(std::function<void()> scopeExit) : scopeExit_(std::move(scopeExit)) {}
    ~Scoped() { scopeExit_(); }

private:
    std::function<void()> scopeExit_;
};

#define SCOPED_CONCAT_(x, y) x##y
#define SCOPED_CONCAT(x, y) SCOPED_CONCAT_(x, y)

#define SCOPED(fn)                           \
Scoped SCOPED_CONCAT(scoped_, __COUNTER__) { \
    fn                                       \
}                                            
