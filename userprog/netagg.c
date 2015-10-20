#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <linux/netagg.h>
#include <netagg.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <string.h>

struct netagg_data {
	int type;
	int command;
	__be32 wPri;
	__be32 wSec;
	struct rule rule;
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
static void process_opt(struct netagg_data *data, int argc, char *argv[]){
	int options;
	while((options = getopt(argc,argv,"A:F:P:S:R:")) != -1){
		switch(options){
			case 'A':
				if(strcmp(optarg,"sender") == 0)data->type = 1;
				else if(strcmp(optarg, "receiver") == 0)data->type = 2;
				else {printf("Invalid command in append rule: -A, use -? for more information\n");exit(EXIT_FAILURE);}
				data->command = 1;
				break;
			case 'F':
				if(strcmp(optarg,"sender") == 0)data->type = 1;
				else if(strcmp(optarg, "receiver") == 0)data->type = 2;
				else {printf("Invalid command in flush rule: -F, use -? for more information\n");exit(EXIT_FAILURE);}
				data->command = 2;
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
	data->rule.binary_vector = generateSendVector(data->wPri,data->wSec);
	int fd = netagg_open();
	netagg_append_sender(fd,&data->rule);
	netagg_close(fd);
}
void append_receiver(struct netagg_data *data){
	int fd = netagg_open();
	netagg_append_receiver(fd,&data->rule);
	netagg_close(fd);
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
	printf("process_sender\n");
	switch(data->command){
		case 1:
			if(isValid(&data->rule))
				append_sender(data);
			else
				printUsage();
			break;
		case 2:
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
	memset((void *)&data,0,sizeof(struct netagg_data));
	process_opt(&data, argc, argv);
	switch(data.type){
		case 1:
			process_sender(&data);
			break;
		case 2:
			process_receiver(&data);
			break;
	}
	return EXIT_SUCCESS;
}
