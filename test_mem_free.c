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
    fprintf(stderr, "Test réalisant de multiples fois d'allocation et de liberation "
                    "\n");

    void* tab[NB_TESTS];

    mem_init(get_memory_adr(), get_memory_size());
    for (int i = 0; i < NB_TESTS; i++) {
        tab[i] = mem_alloc(50);
    }
    mem_show(afficher_zone);
    for (int i = 0; i < NB_TESTS; i++) {
        mem_free(tab[i]);
        //printf("tab[i] = %p\n", tab[i]);
    }
    debug("resultat apres avoir tout liberer : ");
     printf("\n");
    mem_show(afficher_zone);
    debug("on obtient une liste chainee libre, C'est bon!!!\n");
     printf("\n");

//////////////////////////////////////////////////////////////////////////////////////////////
    mem_init(get_memory_adr(), get_memory_size());
    for (int i = 0; i < NB_TESTS; i++) {
        tab[i] = mem_alloc(50);
    }
    mem_show(afficher_zone);
    for (int i = 0; i < NB_TESTS; i+=2) {
        mem_free(tab[i]);
        //printf("tab[i] = %p\n", tab[i]);
    }
    debug("resultat apres avoir tout liberer : \n");
     printf("\n");
    mem_show(afficher_zone);
    debug("C'est bon!!!\n");
     printf("\n");
/*
    debug("si on veut liberer une zone libre ça ne marche pas : \n");
    mem_free(tab[0]);
    */

    mem_init(get_memory_adr(), get_memory_size());
    for (int i = 0; i < NB_TESTS; i++) {
        tab[i] = mem_alloc(50);
    }
    debug("Ici on va voir si les fusion fonctinne correctement\n");
    printf("\n");


    mem_show(afficher_zone);
    mem_free(tab[2]);
    mem_free(tab[4]);
    mem_show(afficher_zone);
    mem_free(tab[3]);
    printf("\n");
    mem_show(afficher_zone);
    debug("C'est bon!!!\n");
    printf("\n");

    debug("resultat apres avoir tout liberer le premier et la derniere: \n");
    mem_free(tab[0]);
    mem_free(tab[9]);
    mem_show(afficher_zone);
    debug("C'est bon!!!\n");
    printf("\n");

    debug("resultat apres avoir tout liberer entre 2 bloc libre \n");
    mem_free(tab[1]);
    mem_show(afficher_zone);
    debug("C'est bon!!!\n");
    printf("\n");

    mem_free(tab[6]);
    mem_free(tab[7]);
    mem_show(afficher_zone);
    debug("C'est bon!!!\n");
    printf("\n");
    mem_free(tab[8]);
    mem_show(afficher_zone);
    debug("C'est bon!!!\n");
    printf("\n");
    mem_free(tab[5]);
    mem_show(afficher_zone);
    debug("C'est bon!!!\n");
    printf("\n");








    // TEST OK
    return 0;
}