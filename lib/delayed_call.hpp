#pragma once

#include <functional>

// need specialization
// see the implementation of std::function
template <class>
class DelayedCall;

template <class OUT, class... IN>
class DelayedCall<OUT(IN...)> {
private:
    using Proto = OUT(IN...);

    std::function<Proto> exec;

    static inline DelayedCall<Proto> *accessInstance(
        DelayedCall<Proto> *value = nullptr
    ) {
        // WARNING: thread_local
        // static thread_local DelayedCall<Proto> *instance = nullptr;
        static DelayedCall<Proto> *instance = nullptr;

        // assert(!instance || !value)
        std::swap(value, instance);
        return value;
    }

    // no dynamic allocation
    inline void *operator new(size_t) = delete;
    inline void operator delete(void *) = delete;

public:
    inline DelayedCall(): exec(nullptr) {
        accessInstance(this);
    }

    // notice: delete by pointer is not allowed
    // ~DelayedCall() {}

    static inline void put(std::function<Proto> &&func) {
        accessInstance()->exec = std::move(func);
    }

    inline operator bool() const {
        return bool(exec);
    }

    inline OUT operator()(IN... arg) const {
        // if exec == nullptr, error
        // exec could be called once and *only* once
        return exec(std::forward<IN>(arg)...);
    }
};
