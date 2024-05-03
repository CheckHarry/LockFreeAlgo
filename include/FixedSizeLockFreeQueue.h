#pragma once
#include <atomic>
#include <vector>
#include <cstdlib>
#include <concepts>


template<typename T> requires std::is_trivially_copy_assignable_v<T> && std::is_trivially_destructible_v<T>
class FixedSizeLockFreeQueue {
public:
    explicit FixedSizeLockFreeQueue(std::int64_t size) : size_(size), start_protected_(0), start_(0), processing_(0),
                                                         commited_(-1) {
        data_ = static_cast<T *>(malloc(size_ * sizeof(T)));
    }

    FixedSizeLockFreeQueue(const FixedSizeLockFreeQueue &) = delete;

    FixedSizeLockFreeQueue operator=(const FixedSizeLockFreeQueue &) = delete;

    FixedSizeLockFreeQueue(FixedSizeLockFreeQueue &&) = delete;

    FixedSizeLockFreeQueue operator=(FixedSizeLockFreeQueue &&) = delete;

    ~FixedSizeLockFreeQueue() {
        free(data_);
    }

    bool push(const T &t) {
        const std::int64_t cur = processing_.load(std::memory_order_acquire);
        const std::int64_t next = cur + 1;
        const std::int64_t prev = cur - 1;
        const std::int64_t cur_end = start_ + size_;

        if (cur >= cur_end) return false;

        std::int64_t tmp = cur;
        if (!processing_.compare_exchange_strong(tmp, next, std::memory_order_relaxed)) return false;

        data_[cur % size_] = t;

        std::int64_t tmp_prev = prev;
        while (!commited_.compare_exchange_strong(tmp_prev, cur, std::memory_order_release)) {
            tmp_prev = prev;
        }

        return true;
    }

    bool pop(T &t) {
        const std::int64_t cur = commited_.load();
        const std::int64_t start = start_.load();
        if (cur < start) return false;
        std::int64_t tmp_start = start;
        if (!start_protected_.compare_exchange_strong(tmp_start, start + 1, std::memory_order_relaxed)) return false;
        t = data_[start % size_];
        start_++;
        return true;
    }

private:
    std::int64_t size_;
    T *data_;
    std::atomic<std::int64_t> start_protected_{};
    std::atomic<std::int64_t> start_{};
    std::atomic<std::int64_t> processing_{};
    std::atomic<std::int64_t> commited_{};
};
