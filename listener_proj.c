/*
** listener.c -- a datagram sockets "server" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>

#define MYPORT "4950"	// the port users will be connecting to
#define SERVERPORT "4951"

#define MAXBUFLEN 1000
#define PICBUFFER 500

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
    struct timeval tv1, tv2;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

    while (1) {
        printf("listener: waiting to recvfrom...\n");

    	addr_len = sizeof their_addr;
    	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
    		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
    		perror("recvfrom");
    		exit(1);
    	}
        gettimeofday(&tv1, NULL);

    	printf("listener: got packet from %s\n",
    		inet_ntop(their_addr.ss_family,
    			get_in_addr((struct sockaddr *)&their_addr),
    			s, sizeof s));
    	printf("listener: packet is %d bytes long\n", numbytes);
    	buf[numbytes] = '\0';
    	printf("listener: packet contains \"%s\"\n", buf);

        //buf has email
        printf("listener: received desired email '%s'\n", buf);
        char response[MAXBUFLEN+1];
        int cur_ind = 0;
        char aux[100];
        char email[200];
        char c1, c2;
        FILE* fp = fopen("data.txt", "r");

        while (fscanf(fp, "%s", aux) != EOF) {
           // response[0] = '\0'; //resetting reponse
           if (strcmp(aux, "Email:") == 0) {
               cur_ind = 0;
               response[cur_ind++] = 'E';
               response[cur_ind++] = 'm';
               response[cur_ind++] = 'a';
               response[cur_ind++] = 'i';
               response[cur_ind++] = 'l';
               response[cur_ind++] = ':';
               // response[cur_ind++] = ' ';
               int i = 0;
               char next;
               fscanf(fp, "%c", &next); //get preceding blank space
               response[cur_ind++] = next;

               fscanf(fp, "%c", &next); //this gets first char
               response[cur_ind++] = next;

               while (next != '\n') {
                   email[i++] = next;
                   fscanf(fp, "%c", &next);
                   response[cur_ind++] = next;

               }
               email[i] = '\0';
               if (strcmp(email, buf) == 0) {
                   while (fscanf(fp, "%c%c", &c1, &c2) != EOF) {

                       if ((c1 == '\n' && c2 == '\n') || (c1 == '\n' && c2 == 'E')) {
                           goto respond_to_client_8;
                       }
                       else {
                           response[cur_ind++] = c1;
                           response[cur_ind++] = c2;
                       }

                   }
               }
           }

       }
       respond_to_client_8:
       response[cur_ind++] = '\0';

       // int len = strlen(response);
       //response is all the user's info

       if ((numbytes = sendto(sockfd, response, strlen(response), 0,
       		 (struct sockaddr *)&their_addr, addr_len)) == -1) {
       	perror("listener: sendto");
       	exit(1);
       }

       /*********************BEGIN SEND PICTURE LOGIC*************************/
       fclose(fp);
       fopen("data.txt", "r");
       char picname[200];
       while (fscanf(fp, "%s", aux) != EOF) {
          // response[0] = '\0'; //resetting reponse
          if (strcmp(aux, "Email:") == 0) {

              int i = 0;
              char next;
              fscanf(fp, "%c", &next); //get preceding blank space
              // response[cur_ind++] = next;
              fscanf(fp, "%c", &next); //this gets first char

              // response[cur_ind++] = next;

              while (next != '\n') {
                  email[i++] = next;
                  fscanf(fp, "%c", &next);
                  // response[cur_ind++] = next;
              }
              email[i] = '\0';
              printf("%s\n", email);

              if (strcmp(email, buf) == 0) { //if it is the email we are looking for
                  while (fscanf(fp, "%s", aux) != EOF) {
                      printf("%s\n", aux);
                      if (strcmp(aux, "Foto:") == 0) {
                          printf("----I ENTERED THE FOTO: CONDITION----\n");
                          fscanf(fp, "%s", picname); //save picture's name
                          goto leave_loops;
                      }
                  }
              }
          }
      }
      int len;
      leave_loops:
      len = strlen(picname);
      printf("picname: %s\n", picname);
      printf("LEN OF FILE NAME%d\n", len);
      fclose(fp);

      if ((numbytes = sendto(sockfd, picname, len, 0,
            (struct sockaddr *)&their_addr, addr_len)) == -1) {
       perror("listener: sendto");
       exit(1);
      }

      FILE* jpg_fp = fopen(picname, "rb");
      char response_pic[PICBUFFER+1];
      // int cur_ind = 0;
      // int j;
      // int k;
      // int len;
      // char next;



      // obtain file size:
      size_t file_size;
      fseek (jpg_fp , 0 , SEEK_END);
      file_size = ftell (jpg_fp);
      rewind (jpg_fp);


      int num_of_it = file_size/PICBUFFER + ((file_size%PICBUFFER) != 0);
      printf("NUM_OF_IT: %d\n", num_of_it);
      // response[0] = (char) (num_of_it+48);
      // response[1] = '\0';
      sprintf(response_pic, "%d", num_of_it);
      printf("NUM_OF_IT (response): %s\n", response_pic);

      len = strlen(response_pic);
      //send num_of_it to client
      if ((numbytes = sendto(sockfd, response_pic, len, 0,
            (struct sockaddr *)&their_addr, addr_len)) == -1) {
       perror("listener: sendto");
       exit(1);
      }
      int j;
      for (j = 0; j < num_of_it; j++) {
          printf("iteration no.: %d\n", j);
          // cur_ind = 0;
          size_t result;
          unsigned char rp[PICBUFFER+1];
          result = fread (rp, 1, PICBUFFER, jpg_fp);

          printf("result size: %lu\n", result);

          //send 500 size chunk
          if ((numbytes = sendto(sockfd, rp, result, 0,
          		 (struct sockaddr *)&their_addr, addr_len)) == -1) {
          	perror("listener: sendto");
          	exit(1);
          }
          if (j == num_of_it - 1) {
              gettimeofday(&tv2, NULL);
          }
          // system("sleep 0.3");
          printf("NUMBYTES: %d\n", numbytes);


      }
      printf("TEMPO DE CONSULTA NO SERVIDOR: %ld\n", (tv1.tv_sec - tv2.tv_sec)*1000000L + tv2.tv_usec - tv1.tv_usec);
       /*********************END SEND PICTURE LOGIC*************************/

    }

    freeaddrinfo(servinfo);
	close(sockfd);

	return 0;
}
