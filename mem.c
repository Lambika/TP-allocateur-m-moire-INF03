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
// typedef struct pointeur_allocator_header *allocator_header;
struct allocator_header {
    size_t memory_size;
    mem_fit_function_t *fit;
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
    struct fb *precedent;
       /* ... */
};


void mem_init(void *mem, size_t taille) {
    memory_addr = mem;
    /* On vérifie qu'on a bien enregistré les infos et qu'on
     * sera capable de les récupérer par la suite
     */
    assert(mem == get_system_memory_addr());

    struct allocator_header *ptr_h=mem;//c'est  pas  obligatoire   lors  d'un  void * de faire  un  cast .
    ptr_h->memory_size=taille;
    ptr_h->fit=&mem_fit_first;
    assert(taille == get_system_memory_size());
    struct fb* first=(struct fb*)(ptr_h +1);
    first->size=ptr_h->memory_size-sizeof(struct allocator_header);
    first->next=NULL;
    first->precedent = NULL;
    ptr_h->first=first;
    /* On enregistre une fonction de recherche par défaut */
    mem_fit(&mem_fit_first);
}
// void mem_show(void (*print)(void *, size_t, int)) {
    
//     while (/* ... */ 0) {
//         /* ... */
//         print(/* ... */ NULL, /* ... */ 0, /* ... */ 0);

//     }
// }
void mem_show(void (*print)(void *, size_t, int)) {
    void* adr = (void*)(get_header() + 1);
    struct fb* zone_libre = get_header()->first;
    while (adr < get_system_memory_size()+get_system_memory_addr()) {
        if (adr == zone_libre){
            print(adr, zone_libre->size, 1);
            adr = adr + zone_libre->size;
            zone_libre = zone_libre->next;
        }
        else{
            struct fb* zone_occ = (struct fb*)adr;
            print(zone_occ,zone_occ->size,0);
            adr += zone_occ->size;
        }
    }
}
void mem_fit(mem_fit_function_t *f) {
    get_header()->fit=f;
}


void* mem_alloc(size_t taille) {
	if (taille <= 0 || taille >= get_system_memory_size()) {return NULL;}
	struct allocator_header* head = get_header();
    //on ajoute la taille de fb dans la taille qui va etre alloué
	taille += sizeof(struct fb);
	/// ALIGNEMENT PAR 8
	int m = taille % 8;
	if (m != 0)
	{
	taille += 8-m;	
	}
	//printf("la taille de M : %i\n", m);
	//printf("la taille de struct fb : %li\n", sizeof(struct fb));
	
	// recherche d'un bloc libre pour la taille
	struct fb* fb2 = head->fit(head->first, taille);
    //struct fb* fb3 = fb2;
	if (fb2 == NULL) {
		return NULL;
	}
	/// PROCHAIN BLOC LIBRE
	struct fb tmp;
    tmp.next = fb2->next;
    tmp.precedent = fb2->precedent;
    tmp.size = fb2->size-taille;
	fb2->size = taille;
    
    void* res = (void*)fb2;

    /// ici on va utiliser la liste entierement
    if(tmp.size == 0){
        if (fb2->precedent != NULL) {
            // Le bloc libre n'est pas au début, on supprime ce bloc de la liste
            fb2->precedent->next = fb2->next;
            // on met à jour le pointeur next du bloc suivant
            if (fb2->next != NULL) {
                fb2->next->precedent = fb2->precedent;
            }
        } else {
            head->first = fb2->next;
            if (fb2->next != NULL) {
                fb2->next->precedent = NULL;
            }
        }
    /// sinon on va utiliser la partie qui reste de la liste
    // si le bloc libre n'est pas au début
    } else if(fb2->precedent != NULL) {
		// on va mettre le prochain fb au bon endroit
		void* Next = (void*)fb2->precedent->next;
		Next = Next + taille;
		fb2->precedent->next = (struct fb*) Next;
		struct fb* bloc = fb2;
		*(bloc->precedent->next) = tmp;
	} else {
		// on va mettre le premier fb au bon endroit
		void* Next = (void*)head->first;
		Next = Next + taille;
		head->first = (struct fb*)Next;
		// on met le nouveau bloc libre à sa place
		struct fb* first = head->first;
        first->precedent = NULL;
		*(first) = tmp;
    }
	return res;

}





void mem_free(void* mem) {
	// on transforme mem en un pointeur fb
    struct fb* bloc_a= (struct fb*)(mem);
    struct fb* nw_bloc = (struct fb*)bloc_a;
    nw_bloc->size = bloc_a->size;
    nw_bloc->next = NULL;

	// on récupere le ffb courrant 
    struct fb* ffb = get_header()->first;
    struct fb* prev = NULL;
    struct fb* cur = NULL;
    // cette boucle nous permet de trouver le bon emplacement
    while(ffb != NULL){
        if (ffb > nw_bloc){
            break ;
        }
        prev = ffb;
        ffb = ffb->next;
	}

    if(ffb == NULL){
        printf("impossible de liberer cette zonne car deja libre\n");
        exit(1);
    }
	// Plusieurs cas :
	// d'abord si c'est au debut
    if (prev == NULL) {
		nw_bloc->next = get_header()->first;
        get_header()->first = nw_bloc ;
    } else {
        // si le bloc à liberer se trouve entre deux bloc libre
        if((void*)prev + prev->size == (void*)nw_bloc && (void*)nw_bloc + nw_bloc->size == (void*)ffb) {
            cur = prev;
            cur->size = prev->size + nw_bloc->size + ffb->size;
            cur->next = ffb->next;
            cur->precedent = prev->precedent;
            ffb = cur;
        // si le bloc suivant existe et est libre et si l'adresse à laquelle elle commence
        // est egale a l'adresse de fin du bloc a liberer
        } else if (nw_bloc->next != NULL && (void*)nw_bloc+ nw_bloc->size == (void*)ffb) {
            cur = nw_bloc;
            cur->size = nw_bloc->size + ffb->size;
            cur->next = ffb->next;
            cur->precedent = ffb->precedent;
            prev->next = cur;
            ffb = cur;
        // si le bloc precedent existe et est libre et si son adresse à laquelle elle se termine
        // est egale a l'adresse de debut du bloc a liberer
        } else if((void*)prev + prev->size == (void*)nw_bloc) {
            cur = prev;
            cur->size = prev->size+nw_bloc->size;
            cur->next = prev->next;
            cur->precedent = prev->precedent;
            ffb = cur;
        // sinon
        } else {
            cur = nw_bloc;
            cur->size = nw_bloc->size;
            cur->next = ffb;
            cur->precedent = prev;
            prev->next = cur;
            ffb->precedent = cur;
            
        }
    }

	struct fb* fb = get_header()->first;
    // boucle qui parcourt fb pour fusionner les blocs libres qui restent
	while (fb->next != NULL) {
		if ((void*)fb - (void*)get_header()+ fb->size == (void*)fb->next - (void*)get_header()) {
			fb->size += fb->next->size;
            fb->next = fb->next->next;
		}
		else fb = fb->next;
	}
}


struct fb* mem_fit_first(struct fb *list, size_t size) {
    while (list != NULL){
        if (size <= list->size){//cherche le  premier qui a la bonne taille
            return list;
        }
        list= list->next;// on  passe  au suivant 
    }
    return NULL;// aucune place disponible avec  une  taille  supérieure  ou  égale à ce  qu'on  veut  faire  entrer
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
    struct fb* fb = (struct fb*)zone;
	return (void*)fb-(void*)get_header();

}

/* Fonctions facultatives
 * autres stratégies d'allocation
 */
struct fb *mem_fit_best(struct fb *list, size_t size) {
    struct fb* best_fit = NULL;
    while (list != NULL) {
        // on regarde si la taille de la liste est la plus petite
        if (list->size >= size && (best_fit == NULL || list->size < best_fit->size)) {
            best_fit = list;
        }
        list = list->next;
    }
    return best_fit;
}

struct fb *mem_fit_worst(struct fb *list, size_t size) {
    struct fb* worst_fit = NULL;
    while (list != NULL) {
        // on regarde si la taille est la plus grande 
        if (list->size >= size && (worst_fit == NULL || list->size > worst_fit->size)) {
            worst_fit = list;
        }
        list = list->next;
    }
    return worst_fit;
}
