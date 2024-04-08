#include "ealloc.h"
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_CHUNKS 4

static const int arr_size = PAGESIZE/MINALLOC;
char* pages[NUM_CHUNKS] = {NULL};
int start[NUM_CHUNKS][arr_size];
int end[NUM_CHUNKS][arr_size];

void init_alloc(){
    for (int i = 0; i < NUM_CHUNKS; i++) {
        for (int j = 0; j < arr_size; j++) {
            start[i][j] = 0;
            end[i][j] = 0;
        }
    }
}

void cleanup() {
    init_alloc();
}

char *alloc(int size) {
    if (size % MINALLOC != 0){
        return NULL;
    }
    int size_units = size / MINALLOC;

    for (int k = 0; k < NUM_CHUNKS; k++) {
        if (pages[k] == NULL) {
            pages[k] = (char *) mmap(0, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

            if (pages[k] == (void *) -1){
                return NULL;
            }
        }

        int state = 0;
        int probable_start = 0;
        int free_count = 0;
        for (int i = 0; i < arr_size; i++) {
            state += start[k][i];

            if (state == 0) {
                free_count += 1;
            } else{
                probable_start = i+1;
                free_count = 0;
            }

            if (free_count == size_units) {
                for (int j = probable_start; j < probable_start + size_units; j++) {
                    start[k][j] = 1;
                }
                for (int j = i; j > i - size_units; j--) {
                    end[k][j] = 1;
                }
                return pages[k] + probable_start * MINALLOC;
            }
            state -= end[k][i];
        }
    }
    return NULL;
}

void dealloc(char * addr){
    for (int k = 0; k < NUM_CHUNKS; k++) {
        if (pages[k] == NULL) {
            return;
        }
        char * temp = pages[k];
        int found = 0;
        for (int i = 0; i < arr_size; i++,temp += MINALLOC) {
            if (temp == addr) {
                start[k][i] = 0;
                found = 1;
            }

            if (found == 1 && end[k][i] == 1){
                end[k][i] = 0;
                return;
            }
        }
    }
}
