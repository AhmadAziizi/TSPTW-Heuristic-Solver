#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>

#define MAX_CITIES 50000

typedef struct {
    int id;
    long x, y;
    long open, close;
} City;

typedef struct {
    long length;
    long time;
    bool valid;
} Stats;

City cities[MAX_CITIES];
int n = 0;

// High-precision wall-clock time
long long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec) * 1000 + (long long)(tv.tv_usec) / 1000;
}

long dist(City a, City b) {
    double dx = (double)(a.x - b.x);
    double dy = (double)(a.y - b.y);
    return (long)floor(sqrt(dx * dx + dy * dy) + 0.5);
}

// Helper to insert an element into an array and shift right
void insert_at(int *arr, int *size, int pos, int val) {
    memmove(&arr[pos + 1], &arr[pos], (*size - pos) * sizeof(int));
    arr[pos] = val;
    (*size)++;
}

// Helper to remove an element from an array and shift left
int remove_at(int *arr, int *size, int pos) {
    int val = arr[pos];
    memmove(&arr[pos], &arr[pos + 1], (*size - pos - 1) * sizeof(int));
    (*size)--;
    return val;
}

// Swaps two elements
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Fisher-Yates shuffle
void shuffle(int *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(&array[i], &array[j]);
    }
}

Stats evaluate(int *route, int route_size) {
    Stats stats = {0, 0, false};
    if (route_size == 0) return stats;

    long currentLength = 0;
    long currentTime = cities[route[0]].open;

    if (currentTime > cities[route[0]].close) return stats;

    for (int i = 1; i < route_size; i++) {
        int prev = route[i - 1];
        int curr = route[i];
        long d = dist(cities[prev], cities[curr]);
        currentLength += d;
        long arrival = currentTime + d;
        long visit = arrival > cities[curr].open ? arrival : cities[curr].open;

        if (visit > cities[curr].close) return stats; // Invalid
        currentTime = visit;
    }

    long returnDist = dist(cities[route[route_size - 1]], cities[route[0]]);
    currentLength += returnDist;
    currentTime += returnDist;

    stats.length = currentLength;
    stats.time = currentTime;
    stats.valid = true;
    return stats;
}

bool is_strictly_better(Stats test, Stats curr) {
    if (test.length < curr.length) return true;
    if (test.length == curr.length && test.time < curr.time) return true;
    return false;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./tsptw <input_file> <output_file>\n");
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        printf("Error opening input file.\n");
        return 1;
    }

    int id;
    long x, y, op, cl;
    while (fscanf(f, "%d %ld %ld %ld %ld", &id, &x, &y, &op, &cl) == 5) {
        cities[n].id = id;
        cities[n].x = x;
        cities[n].y = y;
        cities[n].open = op;
        cities[n].close = cl;
        n++;
    }
    fclose(f);

    if (n == 0) return 0;

    int bestK = -1;
    long bestLength = -1;
    long bestTime = -1;
    int *absoluteBestTour = (int *)malloc(n * sizeof(int));

    long long startTimeMillis = get_time_ms();
    long long maxTimeLimit = 58000; // 58 seconds

    srand(42);
    int *startNodes = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) startNodes[i] = i;
    shuffle(startNodes, n);

    // Pre-allocate memory blocks once to maximize speed
    bool *visited = (bool *)malloc(n * sizeof(bool));
    int *currentTour = (int *)malloc(n * sizeof(int));
    int *active = (int *)malloc(n * sizeof(int));

    for (int s = 0; s < n; s++) {
        if (get_time_ms() - startTimeMillis > maxTimeLimit) break;

        int startNode = startNodes[s];

        // Reset arrays
        memset(visited, 0, n * sizeof(bool));
        for (int i = 0; i < n; i++) active[i] = i;
        int activeCount = n;
        int currentTourSize = 0;

        currentTour[currentTourSize++] = startNode;
        visited[startNode] = true;
        long currentTime = cities[startNode].open;
        int currentNode = startNode;

        // --- 1. GREEDY CONSTRUCTION ---
        while (true) {
            int bestActiveIdx = -1;
            long bestVisitTime = -1;
            long bestDist = -1;

            for (int i = 0; i < activeCount; i++) {
                int targetIdx = active[i];
                if (visited[targetIdx] || currentTime > cities[targetIdx].close) {
                    visited[targetIdx] = true;
                    active[i] = active[activeCount - 1];
                    activeCount--;
                    i--;
                    continue;
                }

                long d = dist(cities[currentNode], cities[targetIdx]);
                long arrival = currentTime + d;
                long visit = arrival > cities[targetIdx].open ? arrival : cities[targetIdx].open;

                if (visit <= cities[targetIdx].close) {
                    if (bestVisitTime == -1 || visit < bestVisitTime || (visit == bestVisitTime && d < bestDist)) {
                        bestVisitTime = visit;
                        bestDist = d;
                        bestActiveIdx = i;
                    }
                }
            }

            if (bestActiveIdx == -1) break;

            int nextNode = active[bestActiveIdx];
            visited[nextNode] = true;
            currentTour[currentTourSize++] = nextNode;
            currentTime = bestVisitTime;
            currentNode = nextNode;
        }

        // --- 2. ADAPTIVE LOCAL SEARCH ---
        bool improved = true;
        Stats currentStats = evaluate(currentTour, currentTourSize);
        if (!currentStats.valid) continue;

        while (improved && (get_time_ms() - startTimeMillis < maxTimeLimit)) {
            improved = false;
            bool useHeavyOperators = currentTourSize < 480;

            // Operator A: Unvisited Insertion
            for (int i = 0; i < n; i++) {
                if (!visited[i]) {
                    for (int pos = 1; pos <= currentTourSize; pos++) {
                        insert_at(currentTour, &currentTourSize, pos, i);
                        Stats testStats = evaluate(currentTour, currentTourSize);
                        if (testStats.valid) {
                            visited[i] = true;
                            currentStats = testStats;
                            improved = true;
                            break;
                        }
                        remove_at(currentTour, &currentTourSize, pos);
                    }
                }
            }

            // Operator B: Swap
            if (!improved && useHeavyOperators) {
                for (int i = 1; i < currentTourSize - 1; i++) {
                    for (int j = i + 1; j < currentTourSize; j++) {
                        swap(&currentTour[i], &currentTour[j]);
                        Stats testStats = evaluate(currentTour, currentTourSize);
                        if (testStats.valid && is_strictly_better(testStats, currentStats)) {
                            currentStats = testStats;
                            improved = true;
                            break;
                        }
                        swap(&currentTour[i], &currentTour[j]); // Revert
                    }
                    if (improved) break;
                }
            }

            // Operator C: Single Relocate
            if (!improved) {
                for (int i = 1; i < currentTourSize; i++) {
                    int movingCity = remove_at(currentTour, &currentTourSize, i);
                    for (int pos = 1; pos <= currentTourSize; pos++) {
                        if (pos == i) continue;
                        insert_at(currentTour, &currentTourSize, pos, movingCity);
                        Stats testStats = evaluate(currentTour, currentTourSize);
                        if (testStats.valid && is_strictly_better(testStats, currentStats)) {
                            currentStats = testStats;
                            improved = true;
                            break;
                        }
                        remove_at(currentTour, &currentTourSize, pos);
                    }
                    if (improved) break;
                    insert_at(currentTour, &currentTourSize, i, movingCity);
                }
            }

            // Operator D: Block Relocate
            if (!improved && useHeavyOperators && currentTourSize > 3) {
                for (int i = 1; i < currentTourSize - 1; i++) {
                    int c1 = remove_at(currentTour, &currentTourSize, i);
                    int c2 = remove_at(currentTour, &currentTourSize, i);

                    for (int pos = 1; pos <= currentTourSize; pos++) {
                        if (pos == i) continue;
                        insert_at(currentTour, &currentTourSize, pos, c1);
                        insert_at(currentTour, &currentTourSize, pos + 1, c2);

                        Stats testStats = evaluate(currentTour, currentTourSize);
                        if (testStats.valid && is_strictly_better(testStats, currentStats)) {
                            currentStats = testStats;
                            improved = true;
                            break;
                        }
                        remove_at(currentTour, &currentTourSize, pos + 1);
                        remove_at(currentTour, &currentTourSize, pos);
                    }
                    if (improved) break;
                    insert_at(currentTour, &currentTourSize, i, c1);
                    insert_at(currentTour, &currentTourSize, i + 1, c2);
                }
            }
        }

        // --- 3. LEXICOGRAPHICAL COMPARISON ---
        bool isNewBest = false;
        int k = currentTourSize;
        long len = currentStats.length;
        long time = currentStats.time;

        if (bestK == -1 || k > bestK) {
            isNewBest = true;
        } else if (k == bestK) {
            if (len < bestLength) {
                isNewBest = true;
            } else if (len == bestLength && time < bestTime) {
                isNewBest = true;
            }
        }

        if (isNewBest) {
            bestK = k;
            bestLength = len;
            bestTime = time;
            memcpy(absoluteBestTour, currentTour, currentTourSize * sizeof(int));
        }
    }

    // --- 4. OUTPUT ---
    FILE *out = fopen(argv[2], "w");
    if (out) {
        fprintf(out, "%d %ld %ld\n", bestK, bestLength, bestTime);
        for (int i = 0; i < bestK; i++) {
            fprintf(out, "%d\n", cities[absoluteBestTour[i]].id);
        }
        fprintf(out, "\n");
        fclose(out);
    }

    free(visited);
    free(currentTour);
    free(active);
    free(startNodes);
    free(absoluteBestTour);

    return 0;
}