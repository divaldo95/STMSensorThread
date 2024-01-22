#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <curl/curl.h>
#include <dirent.h>  
#include <sys/time.h>
#include <time.h>
#include "serialhandler.h"
#include "DataStructures.h"

#define ADDRESS "http://127.0.0.1:3000"

int fd = 0;
int worker_flag = 1;
int exit_cnt = 0;
FILE *fp = NULL;
int line_counter = 0;
int flush_every_nth_line = 1;


int last_file_id = 0;

uint64_t get_ts() 
{
	//struct timespec te;
	//clock_gettime(CLOCK_MONOTONIC, &te);
	return time(NULL);
	//return te.tv_sec;
}

int find_last_file(const char *path, const char *prefix, const char *file_type)
{
	struct dirent **namelist;
    int n;
    n = scandir(path, &namelist, NULL, alphasort);
    char *last_name = "";
    unsigned int last_num_helper = 0;
    unsigned int last_num = 0;
	
	char sscanf_string[100];
	
	snprintf(sscanf_string, 100, "%s%%u.%s", prefix, file_type);  //eg pic10.jpg  pic->prefix jpg-> file_type
	
	//printf("%d:%s\n",n,  sscanf_string);

    for(int i = 0; i < n; i++)
    {
        last_name = namelist[i]->d_name;
        if(strncmp(last_name, prefix, strlen(prefix)) == 0)
        {   
            //printf("Found previous file %s\n", last_name);
           
            if(sscanf(last_name, sscanf_string, &last_num_helper) == EOF)
            {
                last_num = 0;
            }
            else if(last_num < last_num_helper)
            {
                last_num = last_num_helper;
            }
        }
        free(namelist[i]);
    }
    free(namelist);
    
    printf("Last file number: %u\n", last_num);
	
	return last_num;
}

int open_next_log_file()
{
	const char log_path[] = "./log";
	last_file_id = find_last_file(log_path, "log", "txt");
	last_file_id++;
	
	char file_name_string[100];
	snprintf(file_name_string, 100, "%s/log%u.txt", log_path, last_file_id);
	printf("log file:%s\n", file_name_string);
	fp = fopen(file_name_string, "w");
    if(fp == NULL)
    {
        printf("Unable to open file");
        return -1;
    }

	//fprintf(fp, "sensorid;meas_data_type;mc_1p0;mc_2p5;mc_4p0;mc_10p0;nc_0p5;nc_1p0;nc_2p5;nc_4p0;nc_10p0;typical_particle_size;timestamp\n");
	fprintf(fp, "sensorid;connector;meas_data_type;data\n");
	return 0;
}



	
void sigintHandler(int sig_num)
{
	signal(SIGINT, sigintHandler);
	worker_flag = 0;
	exit_cnt++;
	if (exit_cnt >= 3) exit(1);
    //curl_global_cleanup();
	//serialClose(fd);
	//exit(1);
}

int send_curl_data(char *message)
{

    CURL *curl = curl_easy_init();
    if(curl) 
    {
        CURLcode res;

        struct curl_slist *headers = NULL;
        curl_slist_append(headers, "Accept: application/json");
        curl_slist_append(headers, "Content-Type: application/json");
        curl_slist_append(headers, "charset: utf-8");

        curl_easy_setopt(curl, CURLOPT_URL, ADDRESS);
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, message);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcrp/0.1");
        /* Set the expected POST size. */
        //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(jsonObj));

        res = curl_easy_perform(curl);
        printf("Curl returned with code %d\n", res);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));        
    
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
}

typedef struct thread_meas_data
{
	uint16_t node_id;
	uint8_t meas_data_type;
	uint8_t meas_data_length;
	uint8_t meas_data_raw[128];
} thread_meas_data;

typedef union SPS_raw
{
	uint8_t meas_data_raw[128];
	struct sps30_measurement meas_data;
} SPS_raw;

typedef union HDC_raw
{
	uint8_t meas_data_raw[128];
	struct HDC_data meas_data;
} HDC_raw;

typedef union ADS_raw
{
	uint8_t meas_data_raw[128];
	struct ADS_data meas_data;
} ADS_raw;



int parse_packet_string(uint8_t *str, thread_meas_data *packet)
{
	if(strlen(str) < 18)  //short line
	{
		return -10;
	}
	
	memset(packet->meas_data_raw, 0, 128);
	
	if(sscanf(str,"{\"%04hx\",\"%hhx\",\"%02hhx\",", &packet->node_id, &packet->meas_data_type, &packet->meas_data_length) != 3)
	{
		//printf("parse_packet_string() header parse error!\n");
		return -11;
	}
	
	uint8_t *data_str = str;
	for(int i=0;i<3;i++) //find start of data field
	{
		data_str = strchr(data_str, ',') + 1;
		if(data_str == NULL)
			return -12;
	}
	//printf("data field:%s\n",data_str);
	data_str += 1; // skip "
	
	uint32_t data_field_length = strlen(data_str);
	if(data_field_length < 2+2) //min 2 hex digit + end "}
	{
		return -13;
	}
	
	uint32_t data_field_bytes = (data_field_length -2)/2;
	for(int i=0;i<data_field_bytes;i++) 
	{
		if(sscanf(data_str, "%02hhx", &(packet->meas_data_raw[i])) != 1)
		{
			//printf("data field scanf error! i:%d %s\n", i, data_str);
			return -14;
		}
		data_str += 2;
	}
	
	if(*data_str != '\"' || *(data_str+1) != '}')
	{
		return -15;
	}
	
	return 0;
	//{"1234","01","28","0000c0420000000000000000000000000000c0420000000000000000000000000000c0420000c042"}
}

int read_packet(const int fd, struct thread_meas_data *packet)
{
	uint8_t rx_data[1024];
	int32_t rx_length;
	rx_length = serialGetLine(fd, rx_data, 1024);
	
	if(rx_length > 0)
	{
		if(rx_length != strlen(rx_data))
		{
			printf("rx_length mismatch %d %lu\n",rx_length, strlen(rx_data));
			return -1;
		}
		printf("received line:%s\n", rx_data);
		return (parse_packet_string(rx_data, packet));
	}
	else
	{
		return -2;
	}
	
}

void flush_file()
{
	if (line_counter % flush_every_nth_line == 0)
	{
		fflush(fp);
	}
}

int append_sps_to_file(uint16_t node_id, SPS_raw *packet)
{
	fprintf(fp, "%hhu;%d;%d;%.3f;%.3f;%.3f;%.3f;%.3f;%.3f;%.3f;%.3f;%.3f;%.3f;%lu\n",
			node_id,\
			packet->meas_data.connector,\
			meas_data_type_SPS30_extended,\
			packet->meas_data.mc_1p0,\
			packet->meas_data.mc_2p5,\
			packet->meas_data.mc_4p0,\
			packet->meas_data.mc_10p0,\
			packet->meas_data.nc_0p5,\
			packet->meas_data.nc_1p0,\
			packet->meas_data.nc_2p5,\
			packet->meas_data.nc_4p0,\
			packet->meas_data.nc_10p0,\
			packet->meas_data.typical_particle_size,\
			get_ts());

	line_counter++;
}

int append_hdc_to_file(uint16_t node_id, HDC_raw *packet)
{
	fprintf(fp, "%hhu;%d;%d;%.3f;%d;%lu\n",
			node_id,\
			packet->meas_data.connector,\
			meas_data_type_HDC,\
			packet->meas_data.temperature,\
			packet->meas_data.humidity,\
			get_ts());

	line_counter++;
}

int append_ads_to_file(uint16_t node_id, ADS_raw *packet)
{
	fprintf(fp, "%hhu;%d;%d;%.3f;%d;%lu\n",
			node_id,\
			packet->meas_data.connector,\
			meas_data_type_ADS,\
			packet->meas_data.milivolts,\
			packet->meas_data.raw,\
			get_ts());

	line_counter++;
}

int sps_raw_to_json(uint16_t node_id, SPS_raw *packet, char* out, int len)
{
	const char *jsonString =
					"{\n\
					\t\"connector\": %d,\n\
					\t\"sensorid\": %d,\n\
					\t\"meas_data_type\": %d,\n\
					\t\"mc_1p0\": %.3f,\n\
					\t\"mc_2p5\": %.3f,\n\
					\t\"mc_4p0\": %.3f,\n\
					\t\"mc_10p0\": %.3f,\n\
					\t\"nc_0p5\": %.3f,\n\
					\t\"nc_1p0\": %.3f,\n\
					\t\"nc_2p5\": %.3f,\n\
					\t\"nc_4p0\": %.3f,\n\
					\t\"nc_10p0\": %.3f\n\
					\t\"typical_particle_size\": %.3f\n\
					}";

	//uint32_t stringSize = strlen(jsonString) + 200; //allocate enough buffer
	//char* jsonObj = (char*)malloc(stringSize); 
	snprintf(out, len, jsonString, \
		packet->meas_data.connector,\
		node_id,\
		packet->meas_data,\
		packet->meas_data.mc_1p0,\
		packet->meas_data.mc_2p5,\
		packet->meas_data.mc_4p0,\
		packet->meas_data.mc_10p0,\
		packet->meas_data.nc_0p5,\
		packet->meas_data.nc_1p0,\
		packet->meas_data.nc_2p5,\
		packet->meas_data.nc_4p0,\
		packet->meas_data.nc_10p0,\
		packet->meas_data.typical_particle_size);
		
		//printf("Sending: %s\n", jsonObj);
		send_curl_data(out);

		//free(jsonObj);
}

int main(int argc, char** argv)
{
    signal(SIGINT, sigintHandler);
    curl_global_init(CURL_GLOBAL_ALL);
    char *device = "/dev/ttyS19";
	char *file = "measlog.txt";
    int c, index;
	int net_enabled = 0;
    while((c = getopt(argc, argv, "d:f:n:")) != -1)
    {
        switch (c)
        {
        case 'd':
            device = (char*)malloc(sizeof(optarg));
            strcpy(device, optarg);
            break;
		case 'f':
            device = (char*)malloc(sizeof(optarg));
            strcpy(file, optarg);
            break;
		case 'n':
            device = (char*)malloc(sizeof(optarg));
            net_enabled = atoi(optarg);
            break;
        default:
            abort();
        }
    }
    for (index = optind; index < argc; index++) printf ("Non-option argument %s\n", argv[index]);

	fd=serialOpen(device, 115200);
	//printf("serial fd:%d\n", fd);
	if(fd < 0)
	{
		printf("serial port open error: %s!\n", device);
		return -1;
	}
    serialFlush(fd);

	open_next_log_file();
	if (fp == NULL)
	{
		printf("file open error!\n");
		return -1;
	}

    while (worker_flag)
    {
		struct thread_meas_data packet;
		int ret;
		if((ret = read_packet(fd, &packet)) >= 0)
		{
			printf("meas packet received from:%04x, type:%02x, data_length:%02x\n", packet.node_id, packet.meas_data_type, packet.meas_data_length);
			switch (packet.meas_data_type)
			{
			case meas_data_type_SPS30_extended: ;
				SPS_raw sps_raw_data;
				memcpy(sps_raw_data.meas_data_raw, packet.meas_data_raw, 128);
				append_sps_to_file(packet.node_id, &sps_raw_data);
				break;
			case meas_data_type_ADS: ;
				ADS_raw ads_raw_data;
				memcpy(ads_raw_data.meas_data_raw, packet.meas_data_raw, 128);
				append_ads_to_file(packet.node_id, &ads_raw_data);
				break;
			case meas_data_type_HDC: ;
				HDC_raw hdc_raw_data;
				memcpy(hdc_raw_data.meas_data_raw, packet.meas_data_raw, 128);
				append_hdc_to_file(packet.node_id, &hdc_raw_data);
				break;
			default:
				printf("unknown meas_data_type:%x\n", packet.meas_data_type);
				break;
			}
		}
		else
		{
			if(ret <= -10)
				printf("parse error ret:%d\n", ret);
		}
		
    }

	fclose(fp);
	curl_global_cleanup();
	serialClose(fd);
	printf("Measurement ended with %d lines\n", line_counter);
}