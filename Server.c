/**
* * this is the working code
* * last update: Frits Duindam
*/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>  
#include <string.h>
#include "json-c/json.h"

#define PORT 3000 

//variabelen
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char copyBuffer[1024] = {0};
	char JsonBuffer[1024] = {0};
    char Temp1[] = {"A"};                   //temperature request 1
    char ch[] = {"Acknowledge\r"};
    FILE *fp;
	const char *IDname;
	float laagsteTemp = 10.00;


int main(int argc, char const *argv[])
{
    
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	// Forcefully attaching socket to the port 8888
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}


	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
	
	// Forcefully attaching socket to the port 8888
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

while(listen(server_fd, 3) >= 0){

        printf("listening....\n");

        //ondvang request
        while((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))>= 0)
        {

        printf("accepted\n");

            valread = read( new_socket , buffer, 1024); // ondvang bericht van de pi
			
			//fie manegement for json:
			fp = fopen("data.json", "w");
				fprintf(fp, "%s", buffer);
				fclose(fp);

			fp = fopen("data.json", "r");
				struct json_object *parsed_json;
				struct json_object *ID;
				struct json_object *temp;
				struct json_object *humi;
				fread(JsonBuffer, 1024, 1, fp);
				fclose(fp);
	
			parsed_json = json_tokener_parse(JsonBuffer); // copy data to parsed_json

			json_object_object_get_ex(parsed_json, "ID", &ID);

			//file closed
			IDname = json_object_get_string(ID);
				if(strcmp(IDname, "SensorWemos")== 0)
				{
					printf("ID		: %s\n", IDname); // print ID

					memcpy(copyBuffer, buffer, sizeof(buffer)); // copy buffer om deze later te kunnen versturen naar LCDwemos
					//	printf("%s", copyBuffer);
					json_object_object_get_ex(parsed_json, "temp", &temp);
					json_object_object_get_ex(parsed_json, "humi", &humi);

						if (json_object_get_int(temp)== 80) // compared de buffer met de gewenste waarde
						{
							printf("Foute temperatuur meeting! error: %d\n", json_object_get_int(temp));
							printf("Temperature	: %.1f °C\n",json_object_get_double(temp)); // print de temperatuur
							printf("humidity	: %d procent\n",json_object_get_int(humi)); // print de humidity

							float temp2 = json_object_get_double(temp);
							printf("temp2: %.2f\n",temp2);
							if(json_object_get_double(temp) < laagsteTemp)
							{
								laagsteTemp = json_object_get_double(temp);
								printf("laagste temp = %.1f\n",laagsteTemp);
							}

						}	
						else
						{
							printf("Temperature	: %.1f °C\n",json_object_get_double(temp)); // print de temperatuur
							printf("humidity	: %d procent\n",json_object_get_int(humi)); // print de humidity
						}
					
				}
				else if(strcmp(IDname, "LCDWemos")== 0)
				{

					printf("ID		: %s\n", IDname); // print ID
					printf("sending Data....\n");
					send(new_socket , copyBuffer , sizeof(copyBuffer)+5 , 0);
					
				}

        }  
    }
}

