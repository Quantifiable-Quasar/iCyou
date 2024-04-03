#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int vercomp(const char *pkg1, const char *pkg2){
        // compares versions of two packages in the linux format (packagename-version-release.architecture.rpm"
        char *pkg_copy1 = strdup(pkg1);
        char *pkg_copy2 = strdup(pkg2);

        if (pkg_copy1 == NULL || pkg_copy2 == NULL){
                fprintf(stderr, "Uhh vertok error\n");
                if (pkg_copy1 != NULL) free(pkg_copy1);
                if (pkg_copy2 != NULL) free(pkg_copy2);
                // negative numbers is error 
                return -1;
        }


        // convert string name to token for easier search
        char *vertok1 = strtok(pkg_copy1, ":-.");
        char *vertok2 = strtok(pkg_copy2, ":-.");

        // check that both are not empty
        while(vertok1 != NULL && vertok2 != NULL) {
                
                // convert to number
                int num1 = atoi(vertok1);
                int num2 = atoi(vertok2);

                // return number of lower paramater
                // 1 means 1st param lower
                // 2 means 2nd param lower
                if (num1 < num2) {
                        free(pkg_copy1);
                        free(pkg_copy2);
                        return 1;
                } else if (num1 > num2) {
                        free(pkg_copy1);
                        free(pkg_copy2);
                        return 2;
                }
                
                vertok1 = strtok(NULL, ":-.");
                vertok2 = strtok(NULL, ":-.");
        }

         if (vertok1 != NULL) {
                free(pkg_copy1);
                free(pkg_copy2);
                return 900;
        }

        if (vertok2 != NULL) {
                free(pkg_copy1);
                free(pkg_copy2);
                return 900;
        }

        // inconclusive results (maybe equal)
        free(pkg_copy1);
        free(pkg_copy2);
        return 0;
}

int main() {
        int comp = vercomp("grub2-1:2.02-0.86.el7", "grub2-1:2.06-70.el9_3.2.alma.2");
        printf("Lower version: %d\n", comp);
        return 0;
}
