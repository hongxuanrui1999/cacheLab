/*
*Part A:
*In PartA you will write a cache simulator in csim.c that takes a valgrind memory trace as input,
*simulates the hit/miss behavior of a cache memory on this trace,
*and outputs the total number of hits, misses, and evictions.

*StudentNumber:517030910227 
*StudentName:洪瑄锐
*/


#include "cachelab.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

#define address_length  64//输入内存地址长度为64

/*cache组成*/
typedef struct cache_line //定义cache每组的一行，有效位，标志位和用于替换的use变量
{
	char valid;//有效位
	unsigned long long int tag;//标志位
	unsigned long long int use;//用于LRU替换策略
} cache_e;

typedef cache_e* cache_s;//每组有很多行
typedef cache_s* cache_all;//每个cache有很多组

cache_all cache;

/*命令行各个参数*/
int verbose = 0;//是否要输出相关信息
int s = 0;//cache组数
int E = 0;//每组行数
int b = 0;//2^b为每行块的大小（字节）
char *input_file = NULL;//输入文件

/*cache信息*/
int S;//2^s
int B;//2^b

/*需要记录的信息*/
int miss_number = 0;
int hit_number = 0;
int eviction_number = 0;
int num = 0;//为了针对M的输出设置的

/*用于提取标志位和组索引*/
unsigned long long int set_mask = 0;
unsigned long long int tag_mask = 0;


/*初始化cache，主要为cache按输入分配内存空间*/
void init_cache()
{
	cache = (cache_s*)malloc(sizeof(cache_s) * S);
	for (int i = 0; i < S; i++)
	{
		cache[i] = (cache_e*)malloc(sizeof(cache_e) * E);
		for (int j = 0; j < E; j++)
		{
			cache[i][j].valid = 0;
			cache[i][j].tag = 0;
			cache[i][j].use = 0;
		}
	}

	set_mask = (unsigned long long int)(pow(2, s));
	set_mask--;
	int t = address_length - s - b;
	tag_mask = (unsigned long long int)(pow(2, t));
	tag_mask--;
}

void free_cache()
{
	for (int i = 0; i < S; i++)
		free(cache[i]);
	free(cache);
}

void cache_work(unsigned long long int addr)
{
	//获取标志位和组索引
	unsigned long long int set = (addr >> b) & set_mask;
	unsigned long long int tag = (addr >> (b + s)) & tag_mask;

	

	int hit = 0;//标记是否命中

	for (int i = 0; i < E; i++)
	{
		if (cache[set][i].valid == 1 && cache[set][i].tag == tag)
		{
			hit = 1;
			cache[set][i].use = 0;
		}
		else
			cache[set][i].use++;
	}

	if (hit)
	{
		if (verbose)
			printf("hit");
		hit_number++;
		return;
	}

	if (verbose)
		printf("miss ");
	miss_number++;

	//替换
	//找到LRU
	int max = 0;
	int lru_index = 0;

	for (int i = 0; i < E; i++)
	{
		if (cache[set][i].use > max)
		{
			max = cache[set][i].use;
			lru_index = i;
		}
	}

	if (cache[set][lru_index].valid == 1)//如果该块不是空的
	{
		if (verbose)
			printf("eviction ");
		eviction_number++;
	}
		cache[set][lru_index].valid = 1;
		cache[set][lru_index].tag = tag;
		cache[set][lru_index].use = 0;
	
}

/*将trace文件输入*/
void input_trace(char *file)
{
	char tmp[500];
	unsigned long long int addr;
	int len;

	FILE* trace = fopen(file, "r");

	while (fgets(tmp, 500, trace) != NULL)
	{
		if (tmp[1] == 'S' || tmp[1] == 'L' || tmp[1] == 'M')
		{
			sscanf(tmp+3, "%llx,%u", &addr,&len);
			if (verbose)
			{
				printf("%c ", tmp[1]);
				printf("%llx,%u ", addr, len);
			}
			cache_work(addr);

			//M要访问两次
			if (tmp[1] == 'M')
			{
				cache_work(addr);
			}
			if (verbose)
				printf("\n");
		}

	}

	fclose(trace);
}

void printUsage(char* argv[])
{
	printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
	printf("  -h  Optional help flag that prints usage info\n");
	printf("  -v  Optional verbose flag that displays trace info\n");
	printf("  -s <s>: Number of set index bits(S=2^s is the number of sets\n");
	printf("  -E <E>: Associativity(number of lines per set)\n");
	printf("  -b <b>: Number of block bits(B=2^b is the block size)\n");
	printf("  -t <tracefile>: Name of the valgrind trace to replay\n");
	printf("\nFor examples:\n");
	printf("  linux> ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
	printf("  linux> ./csim-ref -v -s 4 -E 4 -b 4 -t traces/yi.trace\n");
}

int main(int argc,char* argv[])
{
	char tmp;

	while ((tmp = getopt(argc, argv, "s:E:b:t:vh")) != -1)
	{
		switch (tmp)
		{
		case 'h':
			printUsage(argv);
			break;
		case 'v':
			verbose = 1;
			break;
		case 's':
			s = atoi(optarg);
			break;
		case 'E':
			E = atoi(optarg);
			break;
		case 'b':
			b = atoi(optarg);
			break;
		case 't':
			input_file = optarg;
			break;
		}
	}

	S = (unsigned int)(pow(2, s));
	B = (unsigned int)(pow(2, b));

	init_cache();

	input_trace(input_file);

	free_cache();

	printSummary(hit_number, miss_number, eviction_number);
	return 0;
	
}
