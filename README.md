
# HTTP Downloader - Socket Programming

## Version 1

Implemented a simple HTTP based downloader to download a given object
specified by an URL. Used a TCP connection and sent a HTTP request to download the file
from the website.

#### Note 

    Sometimes due to heavy traffic to the website the code might not work in the 1st try and give an error saying 
    “Domain Name doesn’t exist”, kindly run it atleast 2-3 times if that happens.
    
### Instructions on running the program(Version 1)

Platform Used : Linux with GCC

Compiling the Codes : 
    
    gcc step1.c

Running the Client: 

    ./a.out

- The url is directly fed inside the code and will return an object of .gif format.
### Screenshots Version 1

![Output Screenshot](https://github.com/AyanPahari/HTTP-Downloader-Socket-Programming/blob/master/Screenshots/version%201.JPG)


## Version 2

- Using Non-Persistent HTTP

        Wrote a downloader program to download the set of objects(.jpeg, .pdf, .gif) in the text file(input.txt) using non-persistent HTTP. It would create as many TCP connections as the number of objects in sequence.

- Using Persistent HTTP

        Wrote a downloader program to download the set of objects in the text file(input.txt) using persistent HTTP without parallel TCP connections.

For Non-Persistent Connections: “Connection: close”

For Persistent Connections: “Connection: keep-alive”
#### Note

    Sometimes due to heavy traffic to the website the code might not work in the 1st try and give an error saying “Domain Name doesn’t exist”, or for some url’s “301 Moved Permanently” . 
    Kindly re-run it atleast 2-3 times until all the objects of different types are downloaded successfully.
### Instructions on running the program(Version 2)

Platform Used : Linux with GCC

Compiling the Codes : 
    
    gcc step2.c , gcc step3.c

Running the Client: 

    ./a.out input.txt

### Screenshots Version 2(Non-Persistent HTTP)

![Output Screenshot](https://github.com/AyanPahari/HTTP-Downloader-Socket-Programming/blob/master/Screenshots/version%202.JPG)

Proof of Connection being Non-Persistent

![Output Screenshot](https://github.com/AyanPahari/HTTP-Downloader-Socket-Programming/blob/master/Screenshots/version%202(NP%20Proof).JPG)


### Screenshots Version 2(Persistent HTTP)

![Output Screenshot](https://github.com/AyanPahari/HTTP-Downloader-Socket-Programming/blob/master/Screenshots/version%202(Persistent).JPG)

Proof of Connection being Persistent

![Output Screenshot](https://github.com/AyanPahari/HTTP-Downloader-Socket-Programming/blob/master/Screenshots/Version%202(Proof%20P).JPG)


