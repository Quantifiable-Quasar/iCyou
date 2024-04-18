#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
#define MAX_CMD_LENGTH 1024
#define MAX_PACKAGE_NAME_LENGTH 256
#define MAX_VERSION_LENGTH 50
#define MAX_PACKAGES 10000
#define OUTFILE "installed_pkg.txt"

typedef struct {
    char name[MAX_PACKAGE_NAME_LENGTH];
    char version[MAX_VERSION_LENGTH];
} PackageInfo;

void getInstalledPackages(PackageInfo packages[], int *packageCount);
void writePackagesToFile(PackageInfo packages[], int packageCount, const char *filename);

int main(int argc, char const* argv[]) {
    int client_fd;
    struct sockaddr_in serv_addr;
    char ip_addr[16] = {' '};

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    printf("Please enter an address to connect to: ");

    scanf("%s", &ip_addr);
    if (inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
        perror("\nInvalid address/ Address not supported");
        return -1;
    }

    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    // Get installed packages and serialize
    PackageInfo packages[MAX_PACKAGES];
    int packageCount = 0;
    getInstalledPackages(packages, &packageCount);
    writePackagesToFile(packages, packageCount, OUTFILE);


    //sent outfile over network
    FILE *fp = fopen(OUTFILE, "r");
    if (fp==NULL) {
            fprintf(stderr, "Error opening file\n");
            return -1;
    }

    char buffer[1024];
    size_t bytes_read;
    while((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            if (send(client_fd, buffer, bytes_read, 0) != bytes_read) {
                    fprintf(stderr, "Error sending file\n");
                    return -1;
            }
    }

    //close stuff
    fclose(fp);
    close(client_fd);

    return 0;
}

void getInstalledPackages(PackageInfo packages[], int *packageCount) {
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
    while (fscanf(fp, "%s %s", packages[*packageCount].name, packages[*packageCount].version) == 2) {
        (*packageCount)++;
        if (*packageCount >= MAX_PACKAGES) {
            printf("Max number of packages reached\n");
            break;
        }
    }

    fclose(fp);
}

void writePackagesToFile(PackageInfo packages[], int packageCount, const char *filename) {
    FILE *fp = fopen(filename, "w"); // Open file for writing
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < packageCount; i++) {
        fprintf(fp, "%s %s\n", packages[i].name, packages[i].version); // Write name and version to file
    }

    fclose(fp); // Close file
}
