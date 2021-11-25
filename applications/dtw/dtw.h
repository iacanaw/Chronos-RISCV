#ifndef __DTW_H__
#define __DTW_H__
#include "../applications.h"

#define bank        0
#define p1          1
#define p2          2
#define p3          3
#define p4          4
#define recognizer  5

#define MATX_SIZE           11       // tamanho da matriz
#define TOTAL_TASKS         4      // deve ser PAR para dividir igualmente o numero de padroes por task
#define NUM_PATTERNS        200 // 40
#define PATTERN_PER_TASK    NUM_PATTERNS / TOTAL_TASKS
#define MESSAGE_MAX_SIZE    128

static unsigned int P[TOTAL_TASKS] = {p1, p2, p3, p4};

int dtw_abs(int num) {
    if (num < 0)
        return (-1) * num;
    else
        return num;
}

int dtw_randNum(int seed, int min, int max) {
    int lfsr = seed;
    lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xB400u);
    return ((lfsr % max) + min);
}

void randPattern(int in[MATX_SIZE][MATX_SIZE]) {
    int i, j;
    for (i = 0; i < MATX_SIZE; i++) {
        for (j = 0; j < MATX_SIZE; j++) {
            in[i][j] = dtw_abs(dtw_randNum(23, 2, 100) % 5000);
        }
    }
}

int dtw_euclideanDistance(int *x, int *y) {
    int ed = 0;
    int aux = 0;
    int i;
    for (i = 0; i < MATX_SIZE; i++) {
        aux = x[i] - y[i];
        ed += aux * aux;
    }
    return ed;
}

int dtw_min(int x, int y) {
    if (x > y)
        return y;
    return x;
}

int dtw_dynamicTimeWarping(int x[MATX_SIZE][MATX_SIZE], int y[MATX_SIZE][MATX_SIZE]) {
    int lastCol[MATX_SIZE];
    int currCol[MATX_SIZE];
    int temp[MATX_SIZE];
    int maxI = MATX_SIZE - 1;
    int maxJ = MATX_SIZE - 1;
    int minGlobalCost;
    int i, j, k;

    currCol[0] = dtw_euclideanDistance(x[0], y[0]);
    for (j = 1; j <= maxJ; j++) {
        currCol[j] = currCol[j - 1] + dtw_euclideanDistance(x[0], y[j]);
    }

    for (i = 1; i <= maxI; i++) {
        // dtw_memcpy(temp, lastCol, sizeof(lastCol));
        for (k = 0; k < MATX_SIZE; k++) {
            temp[k] = lastCol[k];
        }

        // dtw_memcpy(lastCol, currCol, sizeof(lastCol));
        for (k = 0; k < MATX_SIZE; k++) {
            lastCol[k] = currCol[k];
        }

        // dtw_memcpy(currCol, currCol, sizeof(lastCol));
        for (k = 0; k < MATX_SIZE; k++) {
            currCol[k] = currCol[k];
        }

        currCol[0] = lastCol[0] + dtw_euclideanDistance(x[i], y[0]);
        for (j = 1; j <= maxJ; j++) {
            minGlobalCost = dtw_min(lastCol[j], dtw_min(lastCol[j - 1], currCol[j - 1]));
            currCol[j] = minGlobalCost + dtw_euclideanDistance(x[i], y[j]);
        }
    }

    return currCol[maxJ];
}

void *dtw_memset(void *dst, int c, unsigned long bytes) {
    unsigned char *Dst = (unsigned char *)dst;
    while ((int)bytes-- > 0)
        *Dst++ = (unsigned char)c;
    return dst;
}

void dtw_memcpy(void *dest, void *src, int n){
    int i;
    n = n / 4;
    // Typecast src and dest addresses to (int *)
    int *csrc = (int *)src;
    int *cdest = (int *)dest;
  
    // Copy contents of src[] to dest[]
    for (i=0; i<n; i++)
        cdest[i] = csrc[i];

    return;
}


#endif /*__DIJKSTRA_H__*/
