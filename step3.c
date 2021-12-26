#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

int num_count;                    //counts the number of objects downloaded
int header_parser(int socket_fd); //header_parser function declaration

int main(int argc, char *argv[])
{

    char *fileName = argv[1];               //stores the input.txt file
    FILE *file = fopen(fileName, "r");      //open the file in the read mode
    char text[256];                         //contains the url information
    while (fgets(text, sizeof(text), file)) //this will read the .txt file line by line and store that line into text which is basically the url
    {
        char domain[200], path[200], garbage[200], end[200];      //buffer for domain,path,some unnecessary values and the end value like .gif , .pdf etc
        sscanf(text, "https://%199[^/]/%199[^\n]", domain, path); //from the url parse the domain and the path
        sscanf(path, "%199[^.].%3[^\n]", garbage, end);           //from the path parse the file type like .gif, .pdf etc

        printf("Domain = \"%s\"\n", domain); //print the domain
        printf("Path = \"%s\"\n", path);     //print the path

        struct hostent *ip_addr; //to fetch the ip address
        int socket_fd, received_data;
        char data_to_send[1048596], data_to_receive[1048596]; //initialize the storage buffers
        ip_addr = gethostbyname(domain);                      //gethostbyname() returns the ip address from the domain

        if (ip_addr == NULL) //if received ip address is not valid
        {
            printf("Provided domain name doesn't exist....");
            return -1;
        }
        struct sockaddr_in server_addr;

        if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) //check if there are any errors during the socket creation
        {
            printf("Error while creating socket....");
            return -1;
        }
        server_addr.sin_family = AF_INET;                                                       //defining the network family
        server_addr.sin_port = htons(80);                                                       //converting from host byte order to network byte order
        server_addr.sin_addr = *((struct in_addr *)ip_addr->h_addr);                            //it now contains the ip address
        if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) //checking if any error occured while connecting the socket
        {
            printf("Error while connecting the socket....");
            return -1;
        }
        snprintf(data_to_send, sizeof(data_to_send), "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, domain); //data_to_send is now contains the http request of the proper format
        //HTTP 1.1 is persistent by default unless we specify connection=close in the http header
        if (send(socket_fd, data_to_send, strlen(data_to_send), 0) == -1) //send the request to the socket address
        {
            printf("Error occured while sending the data....");
            return -1;
        }
        int total_data = header_parser(socket_fd); //header_parser will return the total size of the object that we are going to download
        if (total_data != 0)                       //only true when the object contains some data
        {
            int curr_data = 0;             //data that we have received till now
            char num[10];                  //this will store the num count required during saving the file
            num_count++;                   //increment the count
            sprintf(num, "%d", num_count); //changing num from int to str so that it can be concat later
            char nameoffile[64] = "Test ";
            strcat(nameoffile, num); //concat the num with Test
            char ext[10] = ".jpeg";  //file format

            if (!strcmp(end, "pdf")) //if it matches then the file format is pdf
            {
                strcpy(ext, ".pdf");
            }
            else if (!strcmp(end, "gif")) //if it matches then the file format is gif
            {
                strcpy(ext, ".gif");
            }
            else
            {
                strcpy(ext, ".jpeg"); //if none matches then it's jpeg format
            }
            strcat(nameoffile, ext);            //this will get the name of output file of the form Test 1.format
            FILE *fd = fopen(nameoffile, "wb"); //open the file in binary mode

            printf("\n\n**************************End of Header Info************************************\n\n");
            while (received_data = recv(socket_fd, data_to_receive, 1048596, 0)) //run this loop till we are getting some data from the server
            {
                fwrite(data_to_receive, 1, received_data, fd);                 //write the received data into the file
                curr_data += received_data;                                    //update the curr_data we have
                printf("Bytes recieved: %d from %d\n", curr_data, total_data); //to show how much data we got till now and how much still needed
                if (curr_data == total_data)                                   //if all the data is received then simply break
                    break;
            }
            fclose(fd); //close the file that was opened for writing
        }
        close(socket_fd); //close the socket
    }
    fclose(file); //close the file
    printf("\n\n %d objects downloaded...\n\n", num_count);
    return 0;
}

int header_parser(int socket_fd) //parses the value of content-length from the http header
{
    char storage[1048596], *curr = storage + 4; //initializing the storage buffers
    int received_data;
    printf("\n\n**************************Start of Header Info************************************\n\n");
    while (received_data = recv(socket_fd, curr, 1, 0)) //run this loop till we are getting some data from the server
    {
        if ((curr[-3] == '\r') && (curr[-2] == '\n') && (curr[-1] == '\r') && (curr[0] == '\n')) //\r\n\r\n is encountered that means we have reached the end of header
            break;
        curr++; //otherwise increment the current pointer
    }
    *curr = 0;
    curr = storage + 4;     //reinitializing
    printf("%s", curr);     //print the entire http header info
    if (received_data != 0) //if we got some valid data
    {
        curr = strstr(curr, "Content-Length:"); //returns pointer to the first occurence of the matched string in the given string
        sscanf(curr, "%*s %d", &received_data); //converts the data size to the integer format
    }
    return received_data; //returns the total size of the object
}

//used HTTP 1.1 in the http request header so that the server can know that we want a persistent connection as it is persistent by default
//unless otherwise requested as connection=close in the header(i.e for non-persistent connection)