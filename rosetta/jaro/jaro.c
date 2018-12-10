#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
 
#define TRUE    1
#define FALSE   0
 
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
 
double jaro(const char *str1, const char *str2) {
    int str1_len = strlen(str1);
    int str2_len = strlen(str2);
 
    if (str1_len == 0) return str2_len == 0 ? 1.0 : 0.0;
 
    int match_distance = (int) max(str1_len, str2_len)/2 - 1;
 
    int *str1_matches = calloc(str1_len, sizeof(int));
    int *str2_matches = calloc(str2_len, sizeof(int));
 
    double matches = 0.0;
    double transpositions = 0.0;
 
    for (int i = 0; i < str1_len; i++) {
        int start = max(0, i - match_distance);
        int end = min(i + match_distance + 1, str2_len);
 
        for (int k = start; k < end; k++) {
            if (str2_matches[k]) continue;
            if (str1[i] != str2[k]) continue;
            str1_matches[i] = TRUE;
            str2_matches[k] = TRUE;
            matches++;
            break;
        }
    }
 
    if (matches == 0) {
        free(str1_matches);
        free(str2_matches);
        return 0.0;
    }
 
    int k = 0;
    for (int i = 0; i < str1_len; i++) {
        if (!str1_matches[i]) continue;
        while (!str2_matches[k]) k++;
        if (str1[i] != str2[k]) transpositions++;
        k++;
    }
 
    transpositions /= 2.0;
 
    free(str1_matches);
    free(str2_matches);
 
    return ((matches / str1_len) +
        (matches / str2_len) +
        ((matches - transpositions) / matches)) / 3.0;
}
 
int main() {
    printf("%f\n", jaro("MARTHA",    "MARHTA"));
    printf("%f\n", jaro("DIXON",     "DICKSONX"));
    printf("%f\n", jaro("JELLYFISH", "SMELLYFISH"));
}
