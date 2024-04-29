# iCyou
Group Members: Mason Miller, Treson Mariotti, and Javid Dovlatov

Overview
================
This is a vulnerability scanner in C. 
It will first use the built-in package manager to get a list of installed packages. Followingly, the client.c file will prompt for the IP address of the server to connect. After this two steps, the packages and their corresponding version numbers will be sent to the IP address entered. Then, server file will use the NVD REST API to search for vulnerabilities in the packages present. If it finds a package with a vulnerability, it will save the JSON output of the API to a report detailing all the vulnerable versions.

Design
==============
The code is designed by assuming that a client-server architecture is present, which means that clients request and receive service from a centralized server. The advantage of this design is that it effectively splits the workload between clients and servers. For example, the server code can function on only one server, and multiple clients can use this server to process their requests rather than sending requests by themselves. It also helps to manage network traffic efficiently by centralizing the handling of requests and responses.


Client --> Server --> National Vulnerability Database

Useage
===========
To use this program, first run the client with ./client. From there, the user is to input the IP address of the server. At that IP address the server should be running with the ./server command. 

For testing purposes, the client and server can run on the same machine, and in this case the client and server should be running on the same machine. For the IP address 127.0.0.1 should be entered.

Client.c file
=============

Firstly, in order to accurately contain all the information related to the package, new structure is defined and named as PackageInfo. This contains the name and version of the package. Other than that, the file contains of two main functions which are:
    - getInstalledPackages
    - writePackagesToFile

As can be also guessed from its name, after finding out which package manager is installed in OS, the getInstalledPackages function retrieves the installed operating system packages via system calls. Respectively, writePackagesToFile function is responsible for creating and writing the existing Operating system packages to the file named installed_pkg.txt. Lastly, the obtained packages and their versions are sent to the server. 


The client can be compiled with:
```bash
gcc client.c -o client
```


Server.c file
=============

The server recieves connections, and then takes the list of packages sent from the client and queries the NVD API for each package with the curlToFile function.

The server can be compiled with:
```bash
gcc server.c -o server -l curl
```

Testing & Details
============
The project only supports Linux Operating System. For test and demonstration purposes, the two terminal windows can be opened on the same machine, and the server IP address can be put as localhost (127.0.0.1) address.

