# iCyou
Group Members: Mason Miller, Treson Mariotti, and Javid Dovlatov

Overview
================
This is a vulnerability scanner in C. 
It will first use the built-in package manager to get a list of installed packages. Followingly, the client.c file will prompt for the IP address of the server to connect. After this two steps, the packages and their corresponding version numbers will be sent to the IP address entered. Then, server file will use the NVD REST API to search for vulnerabilities in the packages present. If it finds a package with a vulnerability, it will save the JSON output of the API to a report detailing all the vulnerable versions.

Design
==============
The code is designed by assuming that a client-server architecture is present, which means that clients request and receive service from a centralized server. The advantage of this design is that it effectively splits the workload between clients and servers. For example, the server code can function on only one server, and multiple clients can use this server to process their requests rather than sending requests by themselves. It also helps to manage network traffic efficiently by centralizing the handling of requests and responses.


+-------------------+     +---------------------+     +-----------------------------------+
|       Client      | --> |        Server       | --> |   National Vulnerability Database |
+-------------------+     +---------------------+     +-----------------------------------+

Client.c file
=============

Firstly, in order to accurately contain all the information related to the package, new structure is defined and named as PackageInfo. This contains the name and version of the package. Other than that, the file contains of two main functions which are:
    - getInstalledPackages
    - writePackagesToFile

As can be also guessed from its name, after finding out which package manager is installed in OS, getInstalledPackages function retrieves the installed operating system packages by using the system command. Respectively, writePackagesToFile function is responsible for creating and writing the existing Operating system packages to the file named installed_pkg.txt. Lastly, the obtained packages and their versions are sent to the server. 


Server.c file
=============

Unlike Client.c file, server.c file contains of only main function, as the only purpose of this side (file) is sending the received packages and their versions to the REST API located at https://services.nvd.nist.gov/rest/json/cves/2.0. In order to do that, C library of Curl is utilized throughout the code. At the end of the execution, the report file is created so that users can see vulnerabilities existing in their system.


Testing & Details
============
The project only supports Linux Operating System, and will not run if launched in any other Operating System. Since the project utilizes lcurl library, both client and server C codes can be compiled by command `gcc [filename.c] -lcurl -o [output_filename]`. For test and demonstration purposes, the two terminal windows can be opened, and the server IP address can be put as localhost (127.0.0.1) address.



TODO
===============

### Client
- [X] detect package manager
- [X] store list of packages in array of tuples (package, version)
- [X] send list of tuples back to the server
- [X] tm: recive the final report and save it to a file

### Server 
- [X] tm: search the NVD for each package in list
- [X] tm: compare version number of package to each CVE affected version number 
- [X] save the result to a file and print to screen

