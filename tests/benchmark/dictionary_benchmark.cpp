#include <vector>
#include <algorithm>
#include <random>

#include <benchmark/benchmark.h>

#include "dictionary.h"


std::vector<std::string> generateStrings(int n)
{
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0);

    const auto randString = [&]() -> std::string
    {
        const auto randChar = [&dist, &rng]() -> char 
        { 
            return 'a' + (dist(rng) % ('z' - 'a' + 1)); 
        };
        constexpr int len = 5;
        std::string str(len, '\0');
        std::generate_n(str.begin(), len, randChar);
        return str;
    };

    std::vector<std::string> result;
    result.reserve(n);
    std::generate_n(std::back_inserter(result), n, randString);
    return result;
}



// Test dictionary set function
static void Dictionary_set(benchmark::State& state)
{
    // Perform setup here
    Dictionary dict{false};
    std::vector<std::string> strings = generateStrings(10000);
    int i = 0;
    for (auto _ : state) {
        // This code gets timed
        const auto& s = strings[i++ % strings.size()];
        dict.set(s, s);
    }
}



// Test dictionary get function
static void Dictionary_getSuccesfull(benchmark::State& state)
{
    // Perform setup here
    Dictionary dict{false};
    std::vector<std::string> strings = generateStrings(10000);
    for (const auto& s : strings)
    {
        dict.set(s, s);
    }
    int i = 0;
    for (auto _ : state) {
        // This code gets timed
        const auto& s = strings[i++ % strings.size()];
        benchmark::DoNotOptimize(dict.get(s));
    }
}



// Test dictionary get function
static void Dictionary_getUnsuccesfull(benchmark::State& state)
{
    // Perform setup here
    Dictionary dict{false};
    std::vector<std::string> strings = generateStrings(10000);
    for (auto& s : strings)
    {
        dict.set(s, s);
        s += '2'; // change strings
    }
    int i = 0;
    for (auto _ : state) {
        // This code gets timed
        const auto& s = strings[i++ % strings.size()];
        benchmark::DoNotOptimize(dict.get(s));
    }
}



// Test dictionary get function
static void Dictionary_getUnsuccesfullWithBloomFilter(benchmark::State& state)
{
    // Perform setup here
    Dictionary dict{true};
    std::vector<std::string> strings = generateStrings(10000);
    for (auto& s : strings)
    {
        dict.set(s, s);
        s += '2'; // change strings
    }
    int i = 0;
    for (auto _ : state) {
        // This code gets timed
        const auto& s = strings[i++ % strings.size()];
        benchmark::DoNotOptimize(dict.get(s));
    }
}



// Register the functions as a benchmark
BENCHMARK(Dictionary_set);
BENCHMARK(Dictionary_getSuccesfull);
BENCHMARK(Dictionary_getUnsuccesfull);
BENCHMARK(Dictionary_getUnsuccesfullWithBloomFilter);

// Run the benchmark
BENCHMARK_MAIN();