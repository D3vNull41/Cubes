#include "bbs.h"

#if BENCHMARK_BBS
// Benchmark comparison between BBS and rand()
void benchmark() {
    const I32 iterations = 1000000;
    U32 seed = get_seed();
    clock_t start, end;
    F32 bbs_time, rand_time;

    // Benchmark BBS
    start = clock();
    for (I32 i = 0; i < iterations; i++) {
        random_F32(&seed);
    }
    end = clock();
    bbs_time = (F32)(end - start) / CLOCKS_PER_SEC;
    printf("BBS Time: %f seconds\n", bbs_time);

    // Benchmark rand()
    start = clock();
    for (I32 i = 0; i < iterations; i++) {
        rand();
    }
    end = clock();
    rand_time = (F32)(end - start) / CLOCKS_PER_SEC;
    printf("rand() Time: %f seconds\n", rand_time);
}

// Main function to demonstrate and benchmark

int main() {
    // Run benchmark
    benchmark();
    
    // Block probabilities for easy level (example)
    I32 block_count = 7;
    U32 seed = get_seed();

    for (I32 i = 0; i < 10; i++) {
        F32 random_value = random_F32(&seed);
        I32 selected_block = (I32)(random_value * block_count);  // Simplified selection
        printf("Generated Block Index: %d\n", selected_block);
    }

    return 0;
}
#endif