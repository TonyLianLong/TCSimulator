#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "self_string.h"
#include "TCS.h"

//Unblock this if you don't want to show debug
/*
#define debug(str) 
#define debug2(str,str2) 
*/

#ifndef debug
#define debug(str) printf("%s",str)
#endif
#ifndef debug2
#define debug2(str,str2) printf(str,str2)
#endif
const unsigned char version[] = "1.0";
const unsigned char helptext[] = "-h Help\n-m Select machine [TC0]\n-f Load codes from file";
const unsigned char describe[] = "TLL's CPU Simulator";
const unsigned char shell_help[] = "h  Help\nn  NextStep\nq  Quit\ns  Show Registers And Address\nr [addr]  Run until [addr]";

uint8_t ns = 0;
uint8_t nsaddr = 0;
uint8_t not_exit = 0;

unsigned int term_option;

unsigned char RAM[32UL*1024UL];

//uint8_t Index;
/*uint8_t R1;
uint8_t R2;
uint8_t R3;
uint8_t R4;
uint8_t R5;
uint8_t R6;
uint8_t R7;*/
uint8_t SimpleRegister[8];
//Index,R1 to R7

unsigned char *filedata;
unsigned int filesize = 0;
unsigned char *programname = NULL;
unsigned char fromfile = 0;

unsigned long long addr = 0;

void exitp(uint8_t return_code){
	debug("Exit.\n");
	//fcntl(STDIN_FILENO,F_SETFL,term_option);//Back
	exit(return_code);
}
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
		exitp(2);
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
		exitp(2);//return 1;
	}else{
		fromfile = 1;
	}
}
void shell(){
	not_exit = 0;
	printf(":");
	char char_input;
	while(!(char_input = getchar()));
	if(char_input == 'h'){
		printf("%s\n",shell_help);
		not_exit = 1;
	}else if(char_input == 'n'){
		printf("Next Step\n");
		ns = 0;//No Stopping
	}else if(char_input == 'q'){
		exitp(0);
	}else if(char_input == 'e'){
		exitp(0);
	}else if(char_input == 's'){
		printf("Show Registers And Address\n");
		printf("Reg|Binary  Code   |Hex\n");
		unsigned int n=0;
		for(n=0;n<8;n++){
			unsigned int j=SimpleRegister[n];
			printf("R%d |%d %d %d %d %d %d %d %d|%x %x\n",n,((j>>7)&0x1),((j>>6)&0x1),((j>>5)&0x1),((j>>4)&0x1),((j>>3)&0x1),((j>>2)&0x1),((j>>1)&0x1),((j>>0)&0x1),j>>4,j&0xf);
		}
		printf("Memory Address:%llu\n",addr);
	}else if(char_input == 'r'){
		uint8_t getaspace = getchar();
		//if(getaspace != ' '){
			//printf("Unknown argument,type \"h\" for help.\n");
			//not_exit = 1;
			while(1){
				if(getaspace == '\n'){
					not_exit = 1;
					fprintf(stderr,"Unknown argument,type \"h\" for help.\n");
					return;	
				}else{
					if(getaspace == ' '){
						unsigned int rarg1 = 0;
						scanf("%i",&rarg1);
						ns = 1;//No Stopping
						nsaddr = rarg1;
						printf("Will not stop until 0x%x\n",nsaddr);
						break;
					}
				}
				getaspace = getchar();
			}
		/*}else{
			unsigned int rarg1 = 0;
			scanf("%i",&rarg1);
			ns = 1;//No Stopping
			nsaddr = rarg1;
			printf("Will not stop until 0x%x\n",nsaddr);
		}*/
	}else{
		fprintf(stderr,"Unknown code,type \"h\" for help.\n");
		not_exit = 1;
	}
	while(getchar() != '\n');
}
unsigned int runcode(unsigned char *data,unsigned long size){
	debug("Code is running!\n");
	//debug2("Filedata[0]:%c\n",*data);
	uint8_t opcode;
	//Note that it's a unsigned char(8-bit int)!
	//4-bit Opcode(but C just have at least 8-bit)
	uint8_t back;
	uint8_t arg1 = 0;
	uint8_t setback;
	uint8_t barg2 = 0;//For Special IMM Command
	uint8_t arg2 = 0;
	uint8_t jump;
	uint8_t error = 0;
	char char_input;
	unsigned long long n=0;
	for(n=0;n<size;n++){
		RAM[n] = data[n];
	}//Load 1K data from SPI Flash to RAM
	while(n<1024){
		RAM[n] = 0;//Fill 0
		n++;
	}
	n = 0;
	printf("Press <Enter> Key to pause the simulator.\n");
	for(addr=0;addr<sizeof(RAM);){
		if(ns == 1){//No Stopping
			//ns = 0;
			//shell();
			if(addr > nsaddr){
				ns = 0;
				if(error != 1){
					printf("OK,stop at 0x%llx(%llu)\n",addr,addr);
					do{
						shell();
					}while(not_exit);
					error = 0;				
				}
			}
		}else{
			if(error != 1){
				do{
					shell();
				}while(not_exit);
			}
		}
		error = 0;
		opcode = RAM[addr]>>4;
		arg1 = RAM[addr] & 0xF;
		setback = 1;
		jump = 0;
		//Data: X  X  X  X  Y  Y  Y  Y
		//X=Opcode Y=REG/NOTHING
		switch(opcode){
			case ADD:
				debug2("ADD at %llu\n",addr);
				back = SimpleRegister[0] + SimpleRegister[arg1];
			break;
			case AND:
				debug2("AND at %llu\n",addr);
				back = SimpleRegister[0] & SimpleRegister[arg1];
			break;
			case OR:
				debug2("OR at %llu\n",addr);
				back = SimpleRegister[0] | SimpleRegister[arg1];
			break;
			case NOT:
				debug2("NOT at %llu\n",addr);
				back = ~SimpleRegister[0];
			break;
			case GETR:
				debug2("GETR at %llu\n",addr);
				back = SimpleRegister[arg1];
			break;
			case SETR:
				debug2("SETR at %llu\n",addr);
				back = 0;
				setback = 0;
				SimpleRegister[arg1] = SimpleRegister[0];
			break;
			case SETI:
				debug2("SETI at %llu\n",addr);
				back = 0;
				setback = 0;
				addr++;
				barg2 = RAM[addr];
				//X X X X X X X X
				//8 - B I T I M M
				SimpleRegister[arg1] = barg2;
			break;
			case GETM:
				debug2("GETM at %llu\n",addr);
				back = RAM[(uint16_t)((SimpleRegister[arg1]<<8)&SimpleRegister[0])];
			break;
			case SETM:
				debug2("STM at %llu\n",addr);
				back = 0;
				setback = 0;
				addr++;
				arg2 = RAM[addr]>>4;
				//X X X X 0 0 0 0
				//A R G 2|Z E R O
				RAM[(uint16_t)((SimpleRegister[arg1]<<8)&SimpleRegister[0])] = arg2;
			break;
			case JE:
				debug2("JE at %llu\n",addr);
				back = 0;
				setback = 0;
				addr++;
				arg2 = RAM[addr]>>4;
				//X X X X 0 0 0 0
				//A R G 2|Z E R O
				if(SimpleRegister[0] == 0){
					addr = SimpleRegister[arg1]<<8&SimpleRegister[arg2];	
					jump = 1;				
				}
			break;
			case JS:
				back = 0;
				setback = 0;
				addr++;
				arg2 = RAM[addr]>>4;
				//X X X X 0 0 0 0
				//A R G 2|Z E R O
				if(((SimpleRegister[0]>>7)&0xf == 0) && SimpleRegister[0] != 0){
					addr = SimpleRegister[arg1]<<8&SimpleRegister[arg2];
					jump = 1;
				}
			break;
			case JMP:
				debug2("JMP at %llu\n",addr);
				back = 0;
				setback = 0;
				addr++;
				arg2 = RAM[addr]>>4;
				//X X X X 0 0 0 0
				//A R G 2|Z E R O
				jump = 1;
				addr = SimpleRegister[arg1]<<8&SimpleRegister[arg2];
			break;
			case RMV:
				back = 0;
				setback = 0;
				SimpleRegister[0] >>= SimpleRegister[arg1]&0x7;
			break;
			case GETF://Will not work in real TC0v01
				addr++;
				arg2 = RAM[addr]>>4;
				//X X X X 0 0 0 0
				//A R G 2|Z E R O
				back = data[SimpleRegister[arg1]<<16&SimpleRegister[arg2]<<8&SimpleRegister[0]];
			break;
			case JL:
				back = 0;
				setback = 0;
				addr++;
				arg2 = RAM[addr]>>4;
				//X X X X 0 0 0 0
				//A R G 2|Z E R O
				if((SimpleRegister[0]>>7)&0xf == 1){
					addr = SimpleRegister[arg1]<<8&SimpleRegister[arg2];
					jump = 1;
				}
			break;
			case NOP://Maybe will remove in TC0v01 or newer version?
				back = 0;
				setback = 0;
				//Nothing,you can add your self code and use this opcode to have a test,e.g. show registers.
			break;
			default:
				fprintf(stderr,"Unknown opcode:0x%x!\n",opcode);
				error = 1;
			break;
		}
		if(setback != 0){
			//Set SimpleRegister[0] to back
			SimpleRegister[0] = back;
		}
		if(!jump){//If not jump,add 1 to addr
			//debug2("Before add i: %d\n",addr);
			addr++;
			//debug2("After add i: %d\n",addr);
		}
		//char_input = getchar();
		//read(0, &char_input, 1);
		/*if(char_input > 0){
			if(char_input == 10){
				//Enter key
			do{
			shell();
		}while(not_exit);
			}
			printf("You press the %c(%d) Key.\n",char_input,char_input);
		}else{
			debug("No key input.\n");
		}*/
		if(error == 1){
			do{
				shell();
			}while(not_exit);
		}
		debug2("Address:%llu\n",addr);
	}
	printf("Finished!\n");
	return 0;
}
int main(int argc,char **argv){
	//term_option = fcntl(STDIN_FILENO,F_GETFL,0);
	/*if(fcntl(STDIN_FILENO,F_SETFL,term_option|O_NONBLOCK) < 0){
		fcntl(STDIN_FILENO,F_SETFL,term_option);//Back
	}*/
	unsigned int backcode = 0;
	unsigned int machine = TC0;
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
					machine = TC0;
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
			case '?':
				fprintf(stderr,"Bad option.\n");  
				help();
				return 1;
                        default:  
                                fprintf(stderr,"Unknown option :%c(%d).\n",ch,ch);  
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
	exitp(0);
	return 0;
	//Will not run
}
