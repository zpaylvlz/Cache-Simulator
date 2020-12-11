#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct cache_tab{
	 int valid;
	 long tag;
	 int dirty;
	 int used;
};
typedef struct cache_tab cache_tab;

int main(int argc, char* argv[]){
	int cache_s, block_s, assoc, policy;
	long fetch = 0, hit, miss = 0, rd = 0, wd = 0,btm = 0, bfm = 0;
    cache_s = atoi(argv[1]);
    block_s = atoi(argv[2]);
    if (!strcmp(argv[4], "FIFO")) {
        policy = 0;
    }

    else if (!strcmp(argv[4], "LRU")) {
        policy = 1;
    }

    if (!strncmp(argv[3], "1", 1)) {
        assoc = 1;
    }
    else if (!strncmp(argv[3], "2", 1)) {
        assoc = 2;
    }
    else if (!strncmp(argv[3], "4", 1)) {
        assoc = 4;
    }
    else if (!strncmp(argv[3], "8", 1)) {
        assoc = 8;
    }
    else if (!strncmp(argv[3], "f", 1)) {
        assoc = cache_s * 1024 / block_s;
    }

	int index = (cache_s * 1024) / (block_s * assoc);
	int offset = block_s;

//	printf("%d\n", assoc*index);

    char inputString[13];

	cache_tab ctb[assoc][index];
	int cur[index];
	for (int i = 0; i < assoc; i++){
		for (int j = 0; j < index; j++){
			ctb[i][j].valid = 0;
			ctb[i][j].tag = -1;
			ctb[i][j].dirty = 0;
			ctb[i][j].used = 0;
			cur[j] = 0;
		}
	}
	
	long hit_loc;
    FILE *input = fopen(argv[5], "r");
    while(fgets(inputString, 13, input)){
		
        int label = -1;
        long address;
        sscanf(inputString,"%d %x",&label,&address);

	if (label == 1){
		wd++;
		fetch++;
	}
	else if (label == 0){
		rd++;
		fetch++;
	}
	else if (label == 2){
		fetch++;
	}



	
	long noff = address / block_s;
	int add_idx = noff % index;
	long tag = noff / index;

	if (policy == 1 && label != -1){
		int flag = 1;
		for (int i = 0; i < assoc; i++){
			if (ctb[i][add_idx].tag == tag){
				flag = 0;
				hit_loc = i;
				break;
			}
			else if (ctb[i][add_idx].valid == 0)
				break;
			
		}
		for (int i = 0; i < assoc; i++){
			ctb[i][add_idx].used -= 1;
		}
		if (flag){
			miss++;
			bfm += block_s;
			int min_loc = 0;

			if (cur[add_idx] != assoc){
				min_loc = cur[add_idx];
				cur[add_idx]++;
			}
			else{

				for (int i = 0; i < assoc; i++){
					if (ctb[i][add_idx].used < ctb[min_loc][add_idx].used){
						min_loc = i;
					}
					if (ctb[i][add_idx].used == ctb[min_loc][add_idx].used){
						//printf("IMPOSSIBLE\n");
					}

				}
			}

			if (ctb[min_loc][add_idx].dirty == 1)
				btm += block_s;
 
			ctb[min_loc][add_idx].tag = tag;
			ctb[min_loc][add_idx].valid = 1;
			ctb[min_loc][add_idx].used = 0;

			if (label == 1)
				ctb[min_loc][add_idx].dirty = 1;
			else
				ctb[min_loc][add_idx].dirty = 0;

		}
		else{
			if (label == 1){
					ctb[hit_loc][add_idx].dirty = 1;
			}
			ctb[hit_loc][add_idx].used = 0;
		}
	}
	





	else if (policy == 0 && label != -1){
		int flag = 1;
		for (int i = 0; i < assoc; i++){
			if (ctb[i][add_idx].tag == tag){
				flag = 0;
				hit_loc = i;
				break;
			}
			else if (ctb[i][add_idx].valid == 0)
				
				break;
			
		}
		if (flag){
			miss++;
			bfm+=block_s;
			
			if (ctb[cur[add_idx]][add_idx].dirty == 1)
				btm += block_s;
 
			ctb[cur[add_idx]][add_idx].tag = tag;
			ctb[cur[add_idx]][add_idx].valid = 1;
			if (label == 1)
				ctb[cur[add_idx]][add_idx].dirty = 1;
			else
				ctb[cur[add_idx]][add_idx].dirty = 0;

			cur[add_idx]++;
			if (cur[add_idx] == assoc){
				cur[add_idx] = 0;
			}

		}
		else{
			if (label == 1)
					ctb[hit_loc][add_idx].dirty = 1;
		}
	}





    }
	for (int i = 0; i < assoc; i++){
		for (int j = 0; j < index; j++){
			if (ctb[i][j].dirty == 1)
				btm += block_s;
		}
	}

	printf("Input file = %s\n", argv[5]);
	printf("Demand Fetch = %d\n", fetch);
	printf("Cache hit = %d\n", fetch - miss);
	printf("Cache miss = %d\n", miss);
	printf("Miss rate = %.4f\n", ((double)miss / (double)fetch));
	printf("Read data = %d\n", rd);
	printf("Write data = %d\n", wd);
	printf("Bytes from memory = %d\n", bfm);
	printf("Bytes to memory = %d\n", btm);

	return 0;
}

