/* On inclut l'interface publique */
#include "mem.h"
/* ainsi que les détails d'implémentation locaux */
#include "common.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

/* Définition de l'alignement recherché
 * Avec gcc, on peut utiliser __BIGGEST_ALIGNMENT__
 * sinon, on utilise 16 qui conviendra aux plateformes qu'on cible
 */
#ifdef __BIGGEST_ALIGNMENT__
#define ALIGNMENT __BIGGEST_ALIGNMENT__
#else
#define ALIGNMENT 16
#endif

/* structure placée au début de la zone de l'allocateur

   Elle contient toutes les variables globales nécessaires au
   fonctionnement de l'allocateur

   Elle peut bien évidemment être complétée
*/
struct allocator_header {
    size_t memory_size;
    mem_fit_function_t *fit;
    /////////////////////
    struct fb* first;
};


/* La seule variable globale autorisée
 * On trouve à cette adresse le début de la zone à gérer
 * (et une structure 'struct allocator_header)
 */
static void *memory_addr;

static inline void *get_system_memory_addr() {
    return memory_addr;
}

static inline struct allocator_header *get_header() {
    struct allocator_header *h;
    h = get_system_memory_addr();
    return h;
}

static inline size_t get_system_memory_size() {
    return get_header()->memory_size;
}

struct fb {
    // Taille, entête compris
    size_t size;
    struct fb *next;
       /* ... */
};

//struct fb* zone_libre;


void mem_init(void *mem, size_t taille) {
    memory_addr = mem;
    /* On vérifie qu'on a bien enregistré les infos et qu'on
     * sera capable de les récupérer par la suite
     */
    assert(mem == get_system_memory_addr());
    /*
    ... = taille
    assert(taille == get_system_memory_size());
    */
    struct allocator_header *ptr_h = mem;
    ptr_h->memory_size = taille;
    ptr_h->fit = &mem_fit_first;
    assert(taille == get_system_memory_size());
    /* ... */

    struct fb* first = (struct fb*)(ptr_h + 1);
    first->size = ptr_h->memory_size - sizeof(struct allocator_header);
    first->next = NULL;
    ptr_h->first= first;


    /* On enregistre une fonction de recherche par défaut */
    mem_fit(&mem_fit_first);
}

void mem_show(void (*print)(void *, size_t, int)) {
    void* adr = (void*)(get_header()+1);
    struct fb* zone_libre = get_header()->first;
    void* MemoryAdr = get_system_memory_addr()+get_system_memory_size();
    while (adr < MemoryAdr) {
        if (adr == zone_libre){
            print(adr, zone_libre->size, 1);
            adr = adr + zone_libre->size;
            zone_libre = zone_libre->next;
        }
        else {
            struct fb* zone_occ = (struct fb*)adr;
            print(zone_occ,zone_occ->size,0);
            adr += zone_occ->size;
        }
    }
}

void mem_fit(mem_fit_function_t *f) {
    get_header()->fit = f;
}

void *mem_alloc(size_t taille) {

}


void mem_free(void *mem) {
}

struct fb *mem_fit_first(struct fb *list, size_t size) {
    while (list != NULL){
        if (size <= list->size){
            return list;
        } else {
            list= list->next;
            }
    }
    return NULL;
}

/* Fonction à faire dans un second temps
 * - utilisée par realloc() dans malloc_stub.c
 * - nécessaire pour remplacer l'allocateur de la libc
 * - donc nécessaire pour 'make test_ls'
 * Lire malloc_stub.c pour comprendre son utilisation
 * (ou en discuter avec l'enseignant)
 */
size_t mem_get_size(void *zone) {
    /* zone est une adresse qui a été retournée par mem_alloc() */

    /* la valeur retournée doit être la taille maximale que
     * l'utilisateur peut utiliser dans cette zone */
    return 0;
}

/* Fonctions facultatives
 * autres stratégies d'allocation
 */
struct fb *mem_fit_best(struct fb *list, size_t size) {
    return NULL;
}

struct fb *mem_fit_worst(struct fb *list, size_t size) {
    return NULL;
}
