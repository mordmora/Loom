#ifndef BENCH_H
#define BENCH_H

#ifdef UMBRA_BENCHMARK

#include <chrono>
#include <iostream>
#include <cstdint>

#define BENCH_BEGIN(name)                                                       \
    do {                                                                        \
        const char* _bench_name = (name);                                       \
        auto _bench_t0 = std::chrono::high_resolution_clock::now();

#define BENCH_END()                                                             \
        auto _bench_t1 = std::chrono::high_resolution_clock::now();            \
        auto _bench_us  = std::chrono::duration_cast<std::chrono::microseconds> \
                            (_bench_t1 - _bench_t0).count();                   \
        auto _bench_ns  = std::chrono::duration_cast<std::chrono::nanoseconds>  \
                            (_bench_t1 - _bench_t0).count();                   \
        std::cout << "[bench] " << _bench_name                                 \
                  << " -> " << _bench_us << " us"                              \
                  << " (" << _bench_ns << " ns)\n";                            \
    } while (false)

#define BENCH_ITERATIONS(name, iters, code)                                     \
    do {                                                                        \
        auto _bi_t0 = std::chrono::high_resolution_clock::now();               \
        for (uint64_t _bi_i = 0; _bi_i < (uint64_t)(iters); _bi_i++) {        \
            code;                                                               \
        }                                                                       \
        auto _bi_t1 = std::chrono::high_resolution_clock::now();               \
        auto _bi_total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>\
                                (_bi_t1 - _bi_t0).count();                     \
        auto _bi_avg_ns   = _bi_total_ns / (uint64_t)(iters);                  \
        std::cout << "[bench] " << (name)                                       \
                  << " x" << (iters)                                            \
                  << " -> total " << _bi_total_ns / 1000 << " us"              \
                  << ", avg " << _bi_avg_ns << " ns/iter\n";                   \
    } while (false)

#else

#define BENCH_BEGIN(name)      do {
#define BENCH_END()            } while (false)
#define BENCH_ITERATIONS(name, iters, code) do { (void)(name); (void)(iters); } while (false)

#endif
#endif
