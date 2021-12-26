 //contains all the required header files for the program
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

int header_parser(int socket_fd); //header_parser function declaration

int main()
{
    //url of the .gif file that we want to download
    const char text[] = "https://media2.giphy.com/media/BfbUe877N4xsUhpcPc/giphy.gif?cid=ecf05e477sy0luo14bczrbz1wldvur10hpg1mms5ep5v8czt&rid=giphy.gif&ct=g";
    char domain[200], path[200];                              //initialize domain and path
    sscanf(text, "https://%199[^/]/%199[^\n]", domain, path); //domain and path now contains the domain and the path value fetched from the url

    printf("domain = %s\n", domain); //print the domain
    printf("path = %s\n", path);     //print the path

    struct sockaddr_in server_addr;
    struct hostent *ip_addr; //to fetch the ip address

    ip_addr = gethostbyname(domain); //gethostbyname() returns the ip address from the domain
    if (ip_addr == NULL)             //if received ip address is not valid
    {
        printf("Provided domain name doesn't exist....");
        return -1;
    }
    int socket_fd, received_data;
    char data_to_send[1048596], data_to_receive[1048596]; //initialize the storage buffers

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
    snprintf(data_to_send, sizeof(data_to_send), "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, domain); //data_to_send now contains the http request of the proper format
    //HTTP 1.1 is persistent by default unless we specify connection=close in the http header
    if (send(socket_fd, data_to_send, strlen(data_to_send), 0) == -1) //send the request to the socket address
    {
        printf("Error occured while sending the data....");
        return -1;
    }
    int total_size = header_parser(socket_fd); //header_parser will return the total size of the object that we are going to download
    if (total_size)                            //only true when the object contains some data
    {
        int curr_data = 0;                  //data that we have received till now
        FILE *fd = fopen("test.gif", "wb"); //open the file in binary mode
        printf("\n\n**************************End of Header Info************************************\n\n");
        while (received_data = recv(socket_fd, data_to_receive, 1048596, 0)) //run this loop till we are getting some data from the server
        {
            fwrite(data_to_receive, 1, received_data, fd);                 //write the received data into the file
            curr_data += received_data;                                    //update the curr_data we have
            printf("Bytes Recieved: %d from %d\n", curr_data, total_size); //to show how much data we got till now and how much still needed
            if (curr_data == total_size)                                   //if all the data is received then simply break
                break;
        }
        fclose(fd); //close the file that was opened for writing
    }
    close(socket_fd); //close the socket
    printf("\n\nDownload Completed.....\n\n");
    return 0;
}

int header_parser(int socket_fd) //parses the value of content-length from the http header
{
    char storage[1048596], *curr = storage + 4;
    int received_data;
    printf("\n\n**************************Start of Header Info************************************\n\n");
    while (received_data = recv(socket_fd, curr, 1, 0))
    {
        if ((curr[-3] == '\r') && (curr[-2] == '\n') && (curr[-1] == '\r') && (curr[0] == '\n'))
            break;
        curr++;
    }
    *curr = 0;
    curr = storage + 4;
    printf("%s", curr);
    if (received_data != 0)
    {
        curr = strstr(curr, "Content-Length:");
        sscanf(curr, "%*s %d", &received_data);
    }
    return received_data;
}