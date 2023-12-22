#include "common.h"
#include "mem.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define NB_TESTS 10


void afficher_zone(void *adresse, size_t taille, int free) {
    printf("Zone %s, Adresse : %lu, Taille : %lu\n", free ? "libre" : "occupee",
           adresse - get_memory_adr(), (unsigned long)taille);
}

int main(int argc, char *argv[]) {
    fprintf(stderr, "Test réalisant de multiples fois d'allocation"
                    "avec les memes tailles au début puis en augmentant et diminuant" 
                    "\n");
    for (int i = 0; i < NB_TESTS; i++) {
        debug("Initializing memory\n");
        mem_init(get_memory_adr(), get_memory_size());
        debug("Taille utilisable = 50\n");
        mem_alloc(50);
    }
    mem_show(afficher_zone);


    mem_init(get_memory_adr(), get_memory_size());
    for (int i = 0; i < NB_TESTS; i++) {
        //debug("Initializing memory\n");
        //mem_init(get_memory_adr(), get_memory_size());
        //debug("Taille utilisable = 50\n");
        mem_alloc(50);
    }
    mem_show(afficher_zone);

    mem_init(get_memory_adr(), get_memory_size());
    for (int i = 0; i < 20; i++) {
        //debug("Initializing memory\n");
        //mem_init(get_memory_adr(), get_memory_size());
        //debug("Taille utilisable = 50\n");
        mem_alloc(50*i);
    }
    mem_show(afficher_zone);

    mem_init(get_memory_adr(), get_memory_size());
    for (int i = 24; i >= 0; i--) {
        //debug("Initializing memory\n");
        //mem_init(get_memory_adr(), get_memory_size());
        //debug("Taille utilisable = 50\n");
        mem_alloc(50*i);
    }
    mem_show(afficher_zone);


    mem_init(get_memory_adr(), get_memory_size());
    debug("allocation avec un nombre negatif ou plus grand que 8192\n");
    mem_alloc(-1);
    mem_alloc(9000);
    mem_show(afficher_zone);
    debug("ça ne fait rien donc envoie null, c'est bon !\n");

    // TEST OK
    return 0;
}