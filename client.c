#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <json-c/json.h>

#define PORT 8080
#define MAX_CMD_LENGTH 1024
#define MAX_PACKAGE_NAME_LENGTH 256
#define MAX_VERSION_LENGTH 50
#define MAX_PACKAGES 10000

/* Biggest changes to make
 * change ip from input to arg (default localhost)
 * no json package here (easier to compile on client)
 * help menu
 * print output to terminal for logs and stuff (send to log file instead??)
 *      - yes and use -v if you want output to terminal
 * yum support?
*/

typedef struct {
    char name[MAX_PACKAGE_NAME_LENGTH];
    char version[MAX_VERSION_LENGTH];
} PackageInfo;

char* serializePackages(PackageInfo packages[], int packageCount);
void getInstalledPackages(PackageInfo packages[], int *packageCount);

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
    char* serializedData = serializePackages(packages, packageCount);

    // Send serialized data
    if (send(client_fd, serializedData, strlen(serializedData), 0) < 0) {
        perror("Send failed");
        return -1;
    }
    printf("Serialized data sent: %s\n", serializedData);

    close(client_fd);
    return 0;
}

char* serializePackages(PackageInfo packages[], int packageCount) {
    struct json_object *packageArray = json_object_new_array();
    for (int i = 0; i < packageCount; i++) {
        struct json_object *packageObject = json_object_new_object();
        json_object_object_add(packageObject, "name", json_object_new_string(packages[i].name));
        json_object_object_add(packageObject, "version", json_object_new_string(packages[i].version));
        json_object_array_add(packageArray, packageObject);
    }
    const char *serializedData = json_object_to_json_string(packageArray);
    char *result = strdup(serializedData); // Copying to a new memory block
    json_object_put(packageArray); // Release the JSON object
    return result;
}

void getInstalledPackages(PackageInfo packages[], int *packageCount) {
    char *command;
    char packageManager[MAX_PACKAGE_NAME_LENGTH] = "";

    // Detect package manager
    if (system("command -v apt-get >/dev/null 2>&1") == 0) {
        strcpy(packageManager, "apt");

    // TODO yum support or to old??
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

    pclose(fp);
}

