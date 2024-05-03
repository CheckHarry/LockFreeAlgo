#include <iostream>
#include <thread>
#include <stop_token>

#include "gtest/gtest.h"
#include "FixedSizeLockFreeQueue.h"


class FixedSizeLockFreeQueueTestFixture : public ::testing::Test {
public:
    static bool
    TestMPSC(std::uint8_t producer_num, std::int64_t queue_size, std::int64_t insert_num) {
        return TestMPMC(producer_num, 1, queue_size, insert_num);
    }

    static bool
    TestMPMC(std::uint8_t producer_num, std::uint8_t consumer_num, std::int64_t queue_size, std::int64_t insert_num) {
        auto q = FixedSizeLockFreeQueue<Payload>(queue_size);

        std::vector<std::vector<std::vector<int>>> result(consumer_num, std::vector<std::vector<int>>(producer_num));

        auto func_producer = [&q, insert_num](int id) {
            for (int i = 0; i < insert_num; i++) {
                while (!q.push(Payload{id, i}));
            }
        };

        auto func_consumer = [&q, &result](const std::stop_token &token, std::uint8_t consumer_id) {
            Payload t;
            int i = 0;
            while (true) {
                if (q.pop(t)) {
                    i++;
                    //if (i % 100000 == 0) std::cout << q.get_reader_count() << '\n';
                    result[consumer_id][t.id].push_back(t.index);
                } else if (token.stop_requested()) break;
            }
        };


        std::vector<std::jthread> threads_producer;
        std::vector<std::jthread> threads_consumer;
        for (std::uint8_t i = 0; i < producer_num; i++) {
            std::jthread t(func_producer, i);
            threads_producer.push_back(std::move(t));
        }

        for (std::uint8_t i = 0; i < consumer_num; i++) {
            std::jthread t(func_consumer, i);
            threads_consumer.push_back(std::move(t));
        }

        for (auto &t: threads_producer) t.join();
        for (auto &t: threads_consumer) t.request_stop();
        for (auto &t: threads_consumer) t.join();

        for (const auto &v: result) {
            if (!validate_increasing(v)) return false;
        }

        for (std::uint8_t j = 0; j < producer_num; j++) {
            std::vector<int> v;
            for (std::uint8_t i = 0; i < consumer_num; i++) {
                for (int num: result[i][j]) v.push_back(num);
            }
            sort(v.begin(), v.end());
            if (v.size() != insert_num) {
                return false;
            }
            if (!is_valid(v)) {
                return false;
            }
        }
        return true;
    }

private:
    struct Payload {
        int id;
        int index;

        friend std::ostream &operator<<(std::ostream &os, const Payload &p) {
            os << p.id << " : " << p.index;
            return os;
        }
    };

    static bool is_valid(const std::vector<int> &v) {
        int cur = -1;
        for (int i: v) {
            if (cur + 1 != i) {
                return false;
            }
            cur++;
        }
        return true;
    }

    static bool validate_increasing(const std::vector<std::vector<int>> &result) {
        for (const auto &v: result) {
            for (int i = 1; i < v.size(); i++) {
                if (v[i - 1] >= v[i]) return false;
            }
        }
        return true;
    }
};

