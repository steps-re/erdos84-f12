// Independent cycle-length-set enumerator for Erdős #84 (subset DP, not path DFS).
// Reads graph6 lines on stdin; prints "graphs <count>" then "<mask> <graph6>" per distinct cycle set (first witness).
// mask bit k <=> a cycle of length k+3 exists.
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define MAXN 16
static int decode(const char *s, int len, uint32_t *adj) {
    int n = s[0] - 63, k = 0;
    memset(adj, 0, sizeof(uint32_t) * MAXN);
    for (int j = 1; j < n; j++)
        for (int i = 0; i < j; i++, k++) {
            int byte = 1 + k / 6, bit = 5 - k % 6;
            if (byte >= len) { fprintf(stderr, "short graph6\n"); exit(2); }
            if (((s[byte] - 63) >> bit) & 1) { adj[i] |= 1u << j; adj[j] |= 1u << i; }
        }
    return n;
}
static uint16_t dp[1 << MAXN];
static uint32_t cycles(int n, const uint32_t *adj) {
    uint32_t found = 0, all = (1u << (n - 2)) - 1;
    for (int s = 0; s < n - 2; s++) {
        // vertices allowed: s and those > s; masks are over the high part, relative index
        int m = n - s;                      // vertex s+i <-> bit i
        uint32_t A[MAXN];
        for (int i = 0; i < m; i++) A[i] = adj[s + i] >> s;
        uint32_t full = 1u << m;
        memset(dp, 0, sizeof(uint16_t) * full);
        dp[1] = 1;                          // path {s} ending at s
        for (uint32_t mask = 1; mask < full; mask += 2) {   // masks always contain bit 0 (s)
            uint32_t ends = dp[mask];
            if (!ends) continue;
            int sz = __builtin_popcount(mask);
            if (sz >= 3 && (ends & A[0])) found |= 1u << (sz - 3);
            while (ends) {
                int v = __builtin_ctz(ends); ends &= ends - 1;
                uint32_t nx = A[v] & ~mask & (full - 1);
                while (nx) { int u = __builtin_ctz(nx); nx &= nx - 1; dp[mask | (1u << u)] |= 1u << u; }
            }
        }
        if (found == all) break;
    }
    return found;
}
int main(void) {
    static char line[256]; uint32_t adj[MAXN];
    static char *wit[1 << 14]; static uint8_t seen[1 << 14];
    unsigned long long count = 0; int n0 = -1;
    while (fgets(line, sizeof line, stdin)) {
        int len = strcspn(line, "\r\n"); line[len] = 0; if (!len) continue;
        int n = decode(line, len, adj);
        if (n0 < 0) n0 = n; else if (n != n0) { fprintf(stderr, "mixed orders\n"); return 2; }
        uint32_t m = n >= 3 ? cycles(n, adj) : 0;
        if (!seen[m]) { seen[m] = 1; wit[m] = strdup(line); }
        count++;
    }
    if (ferror(stdin)) return 3;
    printf("graphs %llu\n", count);
    for (int m = 0; m < (1 << 14); m++) if (seen[m]) printf("%d %s\n", m, wit[m]);
    return 0;
}
