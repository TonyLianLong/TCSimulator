#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "self_string.h"

const unsigned char version[] = "1.0";
const unsigned char helptext[] = "-h Help\n-m Select machine [TC0]\n-f Load codes from file";
const unsigned char describe[] = "TLL's CPU Simulator";

#define debug(str) printf("%s",str)
#define debug2(str,str2) printf(str,str2)
unsigned char *filedata;
unsigned int filesize = 0;
unsigned char *programname = NULL;
unsigned char fromfile = 0;

void help(){
	if(programname != NULL){
		printf("%s:%s\n",programname,describe);
	}else{
		printf("TCS:%s\n",describe);
	}
	printf("%s\n",helptext);
}
unsigned int get_file(char *filename){
	FILE *fp=fopen(filename,"rb");//Read only + Binary file
	if(fp == NULL){
		perror("Error while opening file");
		return 1;
	}
	debug2("FP address:%lu\n",(unsigned long)fp);
	fseek(fp,0L,SEEK_END);//0L is a Long 0
	filesize = ftell(fp);
	fseek(fp,0L,SEEK_SET);
	debug2("Filesize:%d\n",filesize);
	if(filesize > 128L*1024L){
		fprintf(stderr,"The file is too big(larger than 128 Kbytes)!\n");
	}
	filedata = (unsigned char*)malloc(128L*1024L);//Not filesize
	size_t size=fread(filedata,1,filesize,fp);
	unsigned int close=fclose(fp);
	debug2("Load Size:%lu\n",(unsigned long)size);
	if(size == 0 || close != 0){
		perror("Error");
		exit(2);//return 1;
	}else{
		fromfile = 1;
	}
}
unsigned int runcode(unsigned char *data,unsigned long size){
	//debug2("Filedata[0]:%c\n",*data);
	uint8_t opcode;
	//Note that it's a unsigned char(8-bit int)!
	//4-bit Opcode(but C just have at least 8-bit)
	unsigned long long i=0;
	for(i=0;i<size;i++){//Warning: There is a i++.
		opcode = data[i]>>4;
		//Data: X  X  X  X  Y  Y  Y  Y
		//X=Opcode Y=REG/NOTHING
		switch(opcode){
			case ADD:
			break;
		}
	}
	return 0;
}
int main(int argc,char **argv){
	unsigned int backcode = 0;
	if(argc >= 1)programname = argv[0];
	opterr = 0;
	unsigned char ch = 0;
        while ((ch = getopt(argc,argv,"hf:m:v"))!=255)  
        {  
                switch(ch)  
                {  
			case 'm':
				if(strbcmp(optarg,"TC0") == 0){
					debug("Using machine TC0\n");
				}else{
					fprintf(stderr,"Unknown machine:%s!\n",optarg);
					return 1;
				}
				break;
                        case 'f':  
                                debug2("Filename:%s\n",optarg);  
				get_file(optarg);
				debug("File loaded\n");
                                break;  
                        case 'h':  
                                help();
				return 0;  
			case 'v':
				printf("Version: %s\n",version);
				printf("Built date: %s %s\n",__DATE__,__TIME__);
				return 0;
                        default:  
                                fprintf(stderr,"Unknown option :%c(%d)\n",ch,ch);  
				help();
				return 1;
                }  
        }
	if(!fromfile){
		fprintf(stderr,"No file input!\n");
		help();
		return 1;
	}else{
		backcode = runcode(filedata,filesize);
		free(filedata);
		return backcode;
	}
	return 0;
}