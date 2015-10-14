#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <linux/netagg.h>
#include <netagg.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include "../include/netagg.h"

struct netagg_data {
	int type;
	int command;
	__be32 wPri;
	__be32 wSec;
		
};



void setBit(u_int64_t *bitmap, short index){
	u_int64_t mask = 01;
	mask <<= index;
	*bitmap |= mask;
}
u_int64_t generateSendVector(u_int32_t wPrimary, u_int32_t wSecondary)
{
	u_int64_t bitmap = 0;
	int n = sizeof(u_int64_t) * 8;
	double r = wSecondary / (double)(wPrimary + wSecondary);
	r = round(r*n) / n;
	int i;
	for(i = 1; i <= r*n; i++)
	{
		setBit(&bitmap,(short)(i/r));
	}
	return bitmap;
}
static void printUsage(){
	printf("NetAgg - Network Aggregation tool\n");
	printf("Usage:\n");
	printf("\n");
	printf("\t -A <sender|receiver>, Append new rule to rule chain\n");
	printf("\t -F <sender|receiver>, Flush rule chain\n");
	printf("\t -P <daddr:dport>, set primary destination address and port\n");
	printf("\t -S <daddr:dport>, set secondary destination address and port\n");
	printf("\t -R <weightPrimary:weightSecondary>, set weight ratio between primary and secondary path\n");
}
static void process_opt(struct netagg_data *data){
	int options;
	while((options = getopt(argc,argv,"A:FP:S:R:")) != -1){
		switch(options){
			case 'A':
				if(strcmp(optarg,"sender"))type = 1;
				else if(strcmp(optarg, "receiver"))type = 2;
				else {printf("Invalid command in append rule: -A, use -? for more information\n");exit(EXIT_FAILURE);}
				command = 0;
				break;
			case 'F':
				command = 1;
				break;
			case 'P':
				data->rule.pri_daddr = ntohl(inet_addr(strtok(optarg,":")));
				data->rule.pri_dport = atoi(strtok(NULL,""));
				break;
			case 'S':
				data->rule.sec_daddr = ntohl(inet_addr(strtok(optarg,":")));
				data->rule.sec_dport = atoi(strtok(NULL,""));
				break;
			case 'R':
				data->wPri = atol(strtok(optarg, ":"));
				data->wSec = atol(strtok(NULL, ""));
				break;
			case '?':
				printUsage();
				exit(EXIT_SUCCESS);
			default:
				printUsage();
				exit(EXIT_FAILURE);
		}
	}
}
int isValid(struct rule *rule){
	if(rule->pri_daddr != 0 && rule->sec_daddr != 0 && rule->pri_dport != 0 && rule->sec_dport != 0)
		return 1;
	else
		return 0;
}
void append_sender(struct netagg_data *data){
	data->rule.vector = generateSendVector(data->wPri,data->wSec);
	int fd = netagg_open(fd);
	netagg_append_sender(fd,&data->rule);
	netagg_close(fd);
}
void append_receiver(struct netagg_data *data){
	int fd = netagg_open(fd);
	netagg_append_receiver(fd,data->rule);
	netagg_close();
}
void flush_sender(){
	int fd = netagg_open();
	netagg_flush_sender(fd);
	netagg_close(fd);
}
void flush_receiver(){
	int fd = netagg_open();
	netagg_flush_receiver(fd);
	netagg_close(fd);
}
static void process_sender(struct netagg_data *data){
	switch(data->command){
		case 0:
			if(isValid(&data->rule))
				append_sender(data);
			else
				printUsage();
			break;
		case 1:
			flush_sender();
			break;
		default:
			printf("Invalid command!\n");
			printf("\t -A <sender|receiver>, Append new rule to chain sender or receiver\n");
			printf("\t -F <sender|receiver>, Flush sender or receiver rule chain\n");
			break;
	}
}
static void process_receiver(struct netagg_data *data){
	switch(data->command){
		case 1:
			append_receiver(data);
			break;
		case 2:
			flush_receiver(data);
			break;
		default:
			printUsage();
			break;
	}
}
int main(int argc, char *argv[]){
	struct netagg_data data;
	process_opt(&data);

	switch(type){
		case 1:
			process_sender(&data);
			break;
		case 2:
			process_receiver(&data);
		default:
			printUsage();
		exit(EXIT_FAILURE);
	}
		switch(command){
			case 0:
				if(pri_addr != 0 && sec_addr != 0 && pri_port != 0 && sec_port != 0){
					vector = generateSendVector(wPri,wSec);
					fd = my_netfilter_open();
					my_netfilter_append(fd,vector,pri_addr,sec_addr,pri_port, sec_port);
					my_netfilter_close(fd);
				}
				break;
			case 1:
				fd = my_netfilter_open();
				my_netfilter_flush(fd);
				my_netfilter_close(fd);
				break;
			case 2:
				break;
			default:
				break;
		}
	}
return EXIT_SUCCESS;
}
