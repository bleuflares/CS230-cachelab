#include<stdlib.h>
#include<stdio.h>
#include<getopt.h>
#include<strings.h>
#include "cachelab.h"

typedef unsigned long long int addr;
typedef struct
{
	int s;
	int b;
	int E;
	int S;
	int B;
}par;
typedef struct
{
	int bit_set;
	int bit_block;
	int bit_tag;
}bits;
typedef struct
{
	char valid;
	int tag;
	int block;
	int latest_use;
}set_line;

typedef struct
{
	set_line *linearr;
}cache_set;

typedef struct
{
	cache_set *setarr;
}user_cache;

typedef struct
{
	int misses;
	int hits;
	int evicts;
}num;

bits input_to_bits(par input, addr address)
{
	printf("add %llx \n", address);
	bits parbits;
	parbits.bit_set=(address>>input.b)&((1<<input.s)-1);
	parbits.bit_tag=(address>>(input.b+input.s))&((1<<(63-input.b-input.s))-1);
	parbits.bit_block=address&((1<<input.b)-1);
	printf("bits %d %d %d \n",parbits.bit_set, parbits.bit_tag, parbits.bit_block);
	return parbits;
}

user_cache MakeCache(par input)
{
	user_cache cache;
	cache_set set;
	cache.setarr=(cache_set *)malloc(input.S*sizeof(cache_set));
	for(int set_pos=0; set_pos<(1<<input.s); set_pos++)
	{
		set.linearr=(set_line *)calloc(input.E,sizeof(set_line));
		cache.setarr[set_pos]=set;
	}
	return cache;
}
void cacheClear(user_cache cache, par input)
{
	for(int set_pos=0; set_pos<(1<<input.s); set_pos++)
	{
		free(cache.setarr[set_pos].linearr);
	}
	free(cache.setarr);
}
void update(par input, user_cache cache, bits adr, num *count)
{
	set_line *p=cache.setarr[adr.bit_set].linearr;
	int found=0;
	int empty=0;
	int empty_pos=0;
	int evict_pos=0;
	int latest_use=0;
	for(int line_pos=0; line_pos<input.E; line_pos++)
	{
		if((p[line_pos].tag==adr.bit_tag)&&p[line_pos].valid==1)
		{
			printf("hit\n");
			count->hits++;
			p[line_pos].latest_use=0;
			found=1;
		}
		if(p[line_pos].valid==0)
		{
			empty=1;
			empty_pos=line_pos;
		}
		else
		{
			p[line_pos].latest_use++;
		}
		if(p[line_pos].latest_use>=latest_use)
		{
			latest_use=p[line_pos].latest_use;
			evict_pos=line_pos;
		}
	}
	if(found!=1)
	{
		if(empty==1)
		{	printf("miss\n");
			count->misses++;
			p[empty_pos].valid=1;
			p[empty_pos].tag=adr.bit_tag;
			p[empty_pos].latest_use=0;
		}
		else
		{
			printf("miss\n");
			printf("eviction\n");
			count->misses++;
			count->evicts++;
			p[evict_pos].tag=adr.bit_tag;
			p[evict_pos].latest_use=0;
		}
	}
}
void printUsage(char* argv[])
{
	printf("Usage: %s [-hv] -s <num -E <num> -t <file>\n", argv[0]);
	printf("Options:\n");
	printf(" -h 	   Print this help message.\n");
	printf(" -v	   Optional verbose flag.\n");
	printf(" -s <num>  Number of set index bits \n");
	printf(" -E <num>  Nuumber of lines per set. \n");
	printf(" -b <num>  Number of block offset bits \n");
	printf(" -t <file> Trace file.\n");
	printf("\nExamples:\n");
	printf(" %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
	printf(" %s -v -s 8 -E 2 -b -t trace/yi.trace\n", argv[0]);
	exit(0);
}

int main(int argc, char **argv)
{
	char c;
	char *file;
	addr address;
	int size;
	par input;
	num count;
	bits Bit;
	user_cache cache;
	char cmd;
	FILE *trace;
	while((c=getopt(argc,argv,"s:E:b:t:vh"))!=-1)
	{
		switch(c){
		case 'v':
			printUsage(argv);
			exit(0);
		case 't':
			file=optarg;
			break;
		case 's':
			input.s=atoi(optarg);
			printf("s:%d \n",input.s);
			break;
		case 'E':
			input.E=atoi(optarg);
			printf("E:%d \n",input.E);
			break;
		case 'b':
			input.b=atoi(optarg);
			printf("b:%d \n",input.b);
			break;
		case 'h':
			printUsage(argv);
			exit(0);
		default:
			printUsage(argv);
			exit(1);
		}
	}
	if(input.s==0||input.E==0||input.b==0||file==NULL)
	{
		printf("%s: Command Line Argument Missing \n", argv[0]);
		printUsage(argv);
		exit(1);
	}
	input.S=1<<input.s;
	input.B=1<<input.b;
	count.hits=0;
	count.misses=0;
	count.evicts=0;
	cache = MakeCache(input);
	trace = fopen(file, "r");
	if(trace!=NULL)
	{
		
		while(fscanf(trace, " %c %llx,%d", &cmd, &address, &size)==3)
		{
			
			printf("%llx \n",address);	
			switch(cmd)
			{
				case 'I':
					break;
				case 'L':
					
					Bit=input_to_bits(input, address);
					update(input, cache, Bit, &count);
					break;
				case 'M':
					Bit = input_to_bits(input, address);
					update(input, cache, Bit, &count);
				case 'S':
					Bit=input_to_bits(input, address);
					update(input, cache, Bit, &count);
					break;
				default:
					break;
			}
		}
	}
	printSummary(count.hits, count.misses, count.evicts);
	cacheClear(cache, input);
	fclose(trace);
    return 0;
}
