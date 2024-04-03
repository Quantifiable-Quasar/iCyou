# to integrate

this directory puts testing code that is to be integrated into the server 

curl is pretty much done and fairly simple
comments in the code explain whats going on


parse is going to parse the api results
nvd is annoying so we are using redhat security advisories for ease

### Proposed execution flow
Take in package list
check to see is package in list is cached
if not: 
    NVD search by keyword
    take that cve and plug it into red hat
    take out affected version package name and number
        - store in cache files of package:fixed version - CVE
if cache:
    vercompare from cache

### todo
parse:
- get list of cve # from api query
- get list (package:fixver - CVE#) from redhat

server:
- add curl
- add parse (2 func)
- add vercompate
- cache requests
- vercompare based on cache


