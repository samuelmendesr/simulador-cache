//
//	Simulador de Memória Cache 1
//
//	by:	Samuel Mendes Rodrigues
//		samuelmendesr@gmail.com
//
//	Last updade: 26/10/2016 
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_SIZE 1012178
//#define FILE_SIZE 1000000

#define LINE_SIZE 16

struct CACHE{
	unsigned int *in;
	unsigned int *t; 
	short int substituition_mode;
	int size;
	int lines;
	int set;
};

short int find(struct CACHE cache, unsigned int adress); 

void get_memory(struct CACHE cache, unsigned int adress); 

void print_cache(struct CACHE cache); 

int main(int argc, char *argv[]){

	struct CACHE cache;
	FILE *processor; 
	unsigned int adress, line_adress, set_adress;
	short int hit = 0, error = 0;
	int i, hits = 0, errors = 0;
	float hit_, error_;

	//cache padrão
	cache.size = 4096;		// 4096 bytes
	cache.set = 4;			// 4-way
	cache.substituition_mode = 1;	// LRU

	//interface
	if(argc <= 4){

		if(argc >= 2){

			if(!strcmp(argv[1], "1024"))
				cache.size = 1024;
			else if(!strcmp(argv[1], "2048"))
				cache.size = 2048;
			else if(!strcmp(argv[1], "4096"))
				cache.size = 4096;
			else if(!strcmp(argv[1], "8192"))
				cache.size = 8192;
			else if(!strcmp(argv[1], "16384"))
				cache.size = 16384;
			else
				error = 1;
		}

		if(argc >= 3){

			if(!strcmp(argv[2], "direto"))
				cache.set = 1;
			else if(!strcmp(argv[2], "2-way"))
				cache.set = 2;
			else if(!strcmp(argv[2], "4-way"))
				cache.set = 4;
			else if(!strcmp(argv[2], "8-way"))
				cache.set = 8;
			else
				error = 1;
		}

		if(argc == 4){

			if(!strcmp(argv[3], "FIFO"))
				cache.substituition_mode = 0;
			else if(!strcmp(argv[3], "LRU"))
				cache.substituition_mode = 1;
			else
				error = 1;
		}
	}
	else
		error = 1;		
	
	if(!error){

		cache.lines = cache.size/LINE_SIZE;
		cache.in = calloc(cache.size, sizeof(unsigned int* ));
		cache.t = calloc(cache.lines, sizeof(unsigned int* ));

		processor = fopen("trace","r");

		if (processor == NULL)
			printf("Arquivo nao encontrado\n");
		else{
			 
			for(i = 0; i < FILE_SIZE; i++){
			
				fscanf(processor,"%x", &adress);

				hit = find(cache, adress);

				if(hit)
					hits++;
				else
				{		
					get_memory(cache, adress);
					errors++;
				}	
			}	
		}

		fclose(processor);

		hit_ = hits/(FILE_SIZE/100.0);
		error_ = errors/(FILE_SIZE/100.0);

		printf("\nHits:\t%d\t%.2f%\nErros:\t%d\t%.2f%\n", hits, errors, hit_, error_);

		free(cache.in);
		free(cache.t);
	}
	else
	{
		printf("\nOs argumentos devem ser:\n");
		printf("\n./simulador_1.2 [1024/2048/4096/8192/16384] [direto/2-way/4-way/8-way] [FIFO/LRU]\n");
	}
	
	return 0;
}

short int find(struct CACHE cache, unsigned int adress){
	int i, j;
	short int hit = 0;
	unsigned int line_adress, set_adress;

	set_adress = (adress >> 4)%(cache.lines/cache.set);

	for(i = 0; i < cache.set; i++){
		
		line_adress = set_adress*cache.set + i;

		j = 0;

		for(j = 0; j < LINE_SIZE; j++){
				
			if(*(cache.in + line_adress*LINE_SIZE + j) == adress){
				j == LINE_SIZE;
				hit = 1;
			}

			//timestamp é implementado para todas as linhas do conjunto
			if(j < cache.set)
				*(cache.t + set_adress*cache.set + j) = *(cache.t + set_adress*cache.set + j) + 1;
		}
	
		if(hit&&cache.substituition_mode)
			*(cache.t + line_adress) = 1;				

		if(hit)
			i = cache.set;
	}

	return hit;
}

void get_memory(struct CACHE cache, unsigned int adress){
	int i;
	unsigned int tag;
	unsigned int line_adress, set_adress;

	set_adress = (adress >> 4)%(cache.lines/cache.set);

	tag = (adress >> 4);
	tag = (tag << 4);

	line_adress = set_adress*cache.set;

	for(i = 0; i < cache.set; i++){
		
		if(*(cache.t + set_adress*cache.set + i) >= *(cache.t + line_adress))		
			line_adress = (set_adress*cache.set + i); 
	}

	for(i = 0; i < LINE_SIZE; i++){
		
		*(cache.in + line_adress*LINE_SIZE + i) = (tag + i);
	}

	*(cache.t + line_adress) = 1;
}

void print_cache(struct CACHE cache){
	int i;

	for(i = 0; i < cache.size; i++){
		
		printf("%x\n", cache.in[i]);
	}
}
