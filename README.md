# iCyou
Group Members: Mason Miller, Treson Mariotti, and Javid Dovlatov

Overview
================
This is a vulnerability scanner in C. 
It will first use the built in package manager to get a list of installed packages. Then,
it will use the NVD REST API to search for vulnerabilities in the packages present. If it 
finds a package with a vulnerability it will save the JSON output of the API to a report 
detailing all the vulnerable versions.

The project only supports Linux Operating System, and will not run if launched in any other Operating System. Both client and server C codes can be compiled by command `gcc [filename.c] -lcurl -o [output_filename]`

TODO
===============

### Client
- [X] detect package manager
- [X] store list of packages in array of tuples (package, version)
- [X] send list of tuples back to the server
- [ ] tm: recive the final report and save it to a file

### Server 
- [ ] tm: search the NVD for each package in list
- [ ] tm: compare version number of package to each CVE affected version number 
- [ ] save the result to a file and print to screen
- [ ] tm: send report back to the client
