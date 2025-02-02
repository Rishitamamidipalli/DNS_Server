#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#define TLDPORT 4000
#define AUTHPORT 8000

int main() {
        int socketfd = 0, authfd = 0;

        socklen_t length = sizeof(struct sockaddr_in);

        struct sockaddr_in host_addr, auth_addr, client_addr;
        char buffer[64];
        char reqip[30];
        int recvbytes, sentbytes;

        socketfd = socket(AF_INET, SOCK_DGRAM, 0);

        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                return -1;
        }

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(TLDPORT);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

        if(bind(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
                fprintf(stderr, "Error in binding port to socket.\n");
                return -1;
        }

        while(1) {
                recvbytes = recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &length);

                if(strncmp(buffer, "exit", sizeof("exit")) == 0)
                     break;

                fprintf(stdout, "Request from client : %s.\n", buffer);

                char domain[20];
                domain[0] = '\0';
                int i = 0, j = 0;

                while(buffer[i++] != '.');

                while(buffer[i] != '\0')
                        domain[j++] = buffer[i++];

                domain[j] = '\0';

                fprintf(stdout, "%s\n", domain);

                FILE* fd = fopen("tld.txt", "r");

                if(!fd) {
                        fprintf(stderr, "Could not access DNS records.\n");
                        sendto(socketfd, "ERROR", strlen("ERROR") + 1, 0, (struct sockaddr*)&client_addr, length);
                        continue;
                }

                char linebuff[40], filebuff[400], ip[20], tempbuff[40], lastbuff[40];
                char *temp, *iptemp;
                int flag = 0;

                linebuff[0] = '\0';
                lastbuff[0] = '\0';
                filebuff[0] = '\0';
                ip[0] = '\0';

                while( fgets(linebuff, sizeof(linebuff), fd) ) {
                        strcpy(tempbuff, linebuff);
                        temp = strtok(tempbuff, " ");

                        if(flag == 0 && strncmp(temp, domain, strlen(domain)) == 0) {
                                flag = 1;
                                strcpy(lastbuff, linebuff);
                                iptemp = strtok(NULL, "\n");
                            for(i = 0; *iptemp != '\0'; i++, iptemp++)
                                        ip[i] = *iptemp;

                                ip[i] = '\0';
                        }

                        else {
                                strcat(filebuff, linebuff);
                        }
                }

                fclose(fd);

                if(flag == 0) {
                        sentbytes = sendto(socketfd, "404", strlen("404") + 1, 0, (struct sockaddr*)&client_addr, length);
                        continue;
                }

                else {
                        int fdes = open("tld.txt", O_WRONLY);
                        strcat(filebuff, lastbuff);
                        write(fdes, filebuff, strlen(filebuff));
                        close(fdes);
                        fprintf(stdout, "IP for AUTHORITATIVE SERVER of %s : %s.\n(From local DNS records)\n\n", domain, ip);
                }
                fprintf(stdout, "Querying AUTH DNS...\n");
                authfd = socket(AF_INET, SOCK_DGRAM, 0);
                if(authfd < 0) {
                        fprintf(stderr, "Error in socket creation.\n");
                        return -1;
                }

                auth_addr.sin_family = AF_INET;
                auth_addr.sin_port = htons(AUTHPORT);
                inet_pton(AF_INET, "127.0.0.1", &auth_addr.sin_addr);

                sentbytes = sendto(authfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&auth_addr, length);
                recvbytes = recvfrom(authfd, reqip, sizeof(reqip), 0, NULL, NULL);

                fprintf(stdout, "Server IP for %s: %s.\n(returned by AUTH DNS)\nReturning to root DNS...\n\n", buffer, reqip);

                close(authfd);

                sentbytes = sendto(socketfd, reqip, strlen(reqip) + 1, 0, (struct sockaddr*)&client_addr, length);
        }

        close(socketfd);
        return 0;
}
