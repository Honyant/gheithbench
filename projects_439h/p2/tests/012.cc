#include "debug.h"
#include "threads.h"
#include "atomic.h"
#include "barrier.h"
// in a random point cloud, find the smallest distance between two points, multithread to speed up and check every BARRIER_INTERVAL*POINT_COUNT/THREAD_COUNT points
constexpr uint32_t THREAD_COUNT = 4;
constexpr uint32_t POINT_COUNT = 1000;
constexpr uint32_t BARRIER_INTERVAL = 50;

struct Point {
    uint32_t x, y;
};

class PRNG {
private:
    uint32_t seed;
public:
    PRNG(uint32_t initial_seed) : seed(initial_seed) {}
    
    uint32_t next() {
        seed = seed * 1103515245 + 12345;
        return (seed / 65536) % 32768;
    }
};

uint32_t distance_squared(const Point& p1, const Point& p2) {
    int64_t dx = p1.x - p2.x;
    int64_t dy = p1.y - p2.y;
    return (uint32_t) (dx*dx + dy*dy);
}

Barrier* barriers[BARRIER_INTERVAL];
Barrier* endBarrier;

void kernelMain(void) {
    endBarrier = new Barrier(THREAD_COUNT+1);
    for(uint32_t i=0;i<POINT_COUNT*2/BARRIER_INTERVAL;i++){
        barriers[i] = new Barrier(THREAD_COUNT+1);
    }
    Debug::printf("*** starting search\n");

    // Generate random points
    PRNG rng(42); 
    Point* points = new Point[POINT_COUNT];
    for (uint32_t i = 0; i < POINT_COUNT; i++) {
        points[i] = {rng.next() % 2000 - 1000, rng.next() % 2000 - 1000};
    }

    uint32_t globalMinDistSquared = 0x7FFFFFFF;
    uint32_t* localMinDistSquared = new uint32_t[THREAD_COUNT];
    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        localMinDistSquared[i] = 0x7FFFFFFF;
    }

    for (uint32_t t = 0; t < THREAD_COUNT; t++) {
        thread([t, points, &globalMinDistSquared, localMinDistSquared] {
            uint32_t barrier_count = 0;
            uint32_t start = t * (POINT_COUNT / THREAD_COUNT);
            uint32_t end = (t == THREAD_COUNT - 1) ? POINT_COUNT : (t + 1) * (POINT_COUNT / THREAD_COUNT);

            for (uint32_t i = start; i < end; i++) {
                uint32_t currentMinDistSquared = 0x7FFFFFFF;
                for (uint32_t j = 0; j < POINT_COUNT; j++) {
                    if (i != j) {
                        uint32_t distSquared = distance_squared(points[i], points[j]);
                        if (distSquared < currentMinDistSquared) {
                            currentMinDistSquared = distSquared;
                        }
                    }
                }

                if (currentMinDistSquared < localMinDistSquared[t]) {
                    localMinDistSquared[t] = currentMinDistSquared;
                }

                if ((i - start + 1) % BARRIER_INTERVAL == 0 || i == end - 1) {
                    barriers[barrier_count]->sync();
                    barrier_count++;
                    
                    if (t == 0) {
                        // Update global minimum
                        for (uint32_t j = 0; j < THREAD_COUNT; j++) {
                            if (localMinDistSquared[j] < globalMinDistSquared) {
                                globalMinDistSquared = localMinDistSquared[j];
                            }
                        }
                        Debug::printf("*** after processing %d points, smallest squared distance so far: %d\n", 
                                      i - start + 1 + (POINT_COUNT / THREAD_COUNT) * t, globalMinDistSquared);
                    }
                    
                    barriers[barrier_count]->sync();
                    barrier_count++;
                }
            }
        });
    }

    // Wait for all threads to finish
    for (uint32_t i = 0; i < 10; i++) {
        barriers[i]->sync();
    }

    Debug::printf("*** smallest squared distance: %d\n", globalMinDistSquared);
    Debug::printf("*** done\n");

    delete[] localMinDistSquared;
}