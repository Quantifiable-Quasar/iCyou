#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int compare_versions(const char *version1, const char *version2) {
    // Make copies of the input strings since strtok modifies them
    char *copy1 = strdup(version1);
    char *copy2 = strdup(version2);

    // Check for memory allocation failure
    if (copy1 == NULL || copy2 == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        if (copy1 != NULL) free(copy1);
        if (copy2 != NULL) free(copy2);
        return 0; // Return 0 indicating equal versions (assuming failure)
    }

    // Tokenize version strings
    char *token1 = strtok(copy1, ":-.");
    char *token2 = strtok(copy2, ":-.");

    while (token1 != NULL && token2 != NULL) {
        // Convert tokens to integers
        int num1 = atoi(token1);
        int num2 = atoi(token2);

        // Compare numbers
        if (num1 < num2) {
            free(copy1);
            free(copy2);
            return -1;
        } else if (num1 > num2) {
            free(copy1);
            free(copy2);
            return 1;
        }

        // Move to next token
        token1 = strtok(NULL, ":-.");
        token2 = strtok(NULL, ":-.");
    }

    // Check for remaining tokens
    if (token1 != NULL) {
        free(copy1);
        free(copy2);
        return 1; // version1 has more segments, so it's greater
    }
    if (token2 != NULL) {
        free(copy1);
        free(copy2);
        return -1; // version2 has more segments, so it's greater
    }

    // Versions are equal
    free(copy1);
    free(copy2);
    return 0;
}


int main() {
    const char *version1 = "grub2-1:2.02-0.86.el7";
    const char *version2 = "1:2.06-70.el9_3.2.alma.2";

    int result = compare_versions(version1, version2);

    if (result < 0) {
        printf("%s is less than %s\n", version1, version2);
    } else if (result > 0) {
        printf("%s is greater than %s\n", version1, version2);
    } else {
        printf("%s is equal to %s\n", version1, version2);
    }

    return 0;
}
