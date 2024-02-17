# iCyou
Group Members: Mason Miller, Treson Mariotti, and Javid Dovlatov

Overview
================
This is a vulnerability scanner in C. 
It will first use the built in package manager to get a list of installed packages. Then,
it will use the NVD REST API to search for vulnerabilities in the packages present. If it 
finds a package with a vulnerability it will save the JSON output of the API to a report 
detailing all the vulnerable versions.

TODO
===============
- detect package manager
- sore list of packages in array of tuples (package, version)
- search the NVD for each package in list
- save query (moves faster with cache/offline option)?
- compare version number of package to each CVE affected version list
- save the result to a file
- print to screen
- implement as Client-Server thingamajig
