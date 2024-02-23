#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD_LENGTH 1024
#define MAX_PACKAGE_NAME_LENGTH 256
#define MAX_VERSION_LENGTH 50
#define MAX_PACKAGES 10000

typedef struct {
    char name[MAX_PACKAGE_NAME_LENGTH];
    char version[MAX_VERSION_LENGTH];
} PackageInfo;

void getInstalledPackages();
void printInstalledPackages(PackageInfo packages[], int packageCount);


int main() {
    getInstalledPackages();
    return 0;
}

void getInstalledPackages() {
    PackageInfo packages[MAX_PACKAGES];
    int packageCount = 0;
    char *command;
    char packageManager[MAX_PACKAGE_NAME_LENGTH] = "";

    // Detect package manager
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

    // Retrieve installed packages
    if (strcmp(packageManager, "apt") == 0) {
        command = "dpkg-query -W -f='${Package} ${Version}\n'";
    } else if (strcmp(packageManager, "dnf") == 0) {
        command = "dnf list installed | awk '{print $1, $2}'";
    } else if (strcmp(packageManager, "pacman") == 0) {
        command = "pacman -Q";
    }

    // Open pipe to execute command
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        return;
    }

    // Read installed packages and versions into array of tuples
    while (fscanf(fp, "%s %s", packages[packageCount].name, packages[packageCount].version) == 2) {
        packageCount++;
        if (packageCount >= MAX_PACKAGES) {
            printf("Max number of packages reached\n");
            break;
        }
    }

    pclose(fp);
    // Optional function to print out the recieved packages
    // printInstalledPackages(packages, packageCount);

}

void printInstalledPackages(PackageInfo packages[], int packageCount) {
    printf("Installed packages:\n");
    for (int i = 0; i < packageCount; i++) {
        printf("%s (%s)\n", packages[i].name, packages[i].version);
    }
}
