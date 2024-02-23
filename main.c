#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD_LENGTH 1024
#define MAX_PACKAGE_NAME_LENGTH 256

void getInstalledPackages();

int main() {
    getInstalledPackages();
    return 0;
}

void getInstalledPackages() {
    FILE *fp;
    FILE *outputFile;
    char buffer[MAX_CMD_LENGTH];
    char *command;
    char packageManager[MAX_PACKAGE_NAME_LENGTH] = "";

    // Find system package manager
    if (system("command -v apt-get >/dev/null 2>&1") == 0) {
        strcpy(packageManager, "apt");
    } else if (system("command -v dnf >/dev/null 2>&1") == 0) {
        strcpy(packageManager, "dnf");
    } else if (system("command -v pacman >/dev/null 2>&1") == 0) {
        strcpy(packageManager, "pacman");
    } else {
        printf("Unsupported package manager.\n");
        return;
    }

    // Run package list commands for each manager 
    if (strcmp(packageManager, "apt") == 0) {
        command = "dpkg -l | grep ^ii | awk '{print $2}'";
    } else if (strcmp(packageManager, "dnf") == 0) {
        command = "dnf list installed | awk '{print $1}'";
    } else if (strcmp(packageManager, "pacman") == 0) {
        command = "pacman -Qq";
    }

    outputFile = fopen("installed_packages.txt", "w");
    if (outputFile == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    // should take the command we found above and run
    snprintf(buffer, sizeof(buffer), "%s", command);
    fp = popen(buffer, "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        fclose(outputFile);
        return;
    }

    // Write packages to file
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        fprintf(outputFile, "%s", buffer);
    }

    fclose(fp);
    fclose(outputFile);
    printf("Done.\nPackages Written Successfully\n");
}

