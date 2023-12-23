#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "uhashmap.h"

static size_t NUM_BIT_IN_BYTE = 8;
static const size_t DELETED   = -0xD;
static const size_t SIGNATURE = 0xBAFDAC; //12254636

#define PERROR_PREFIX  fprintf(stderr, "In func <%s> in file <%s> (line %d): ",                        __func__, __FILE__, __LINE__)
#define PERROR_FALLOC  fprintf(stderr, "In func <%s> in file <%s> (line %d): failed allocation\n",     __func__, __FILE__, __LINE__)
#define PERROR_NULLPTR fprintf(stderr, "In func <%s> in file <%s> (line %d): NULL ptr at the input\n", __func__, __FILE__, __LINE__)

struct uhash_cell {
    size_t size;
    void  *content;
};

struct uhmap {
    size_t map_size;
    size_t is_multi_set;
    struct uhash_cell *map;
    int    ah1;
    int    ah2;
    size_t num_elem;
};

static size_t next_number_pow_of_two(const size_t num);
static size_t uhash_1(struct uhmap *hmap, const void *new_elem, const size_t size_elem);
static size_t uhash_2(struct uhmap *hmap, const void *new_elem, const size_t size_elem);
static void   add_coeff(struct uhmap *hmap);

struct uhmap *new_uhmap(const size_t initial_quantity, const size_t is_multi_set) {
    struct uhmap *hmap = (struct uhmap *) calloc(1, sizeof(struct uhmap));

    if (hmap == NULL) {
        PERROR_FALLOC;
        return NULL;
    }

    hmap->map_size = next_number_pow_of_two(initial_quantity);

    hmap->map = (struct uhash_cell *) calloc(hmap->map_size, sizeof(struct uhash_cell));

    if (hmap->map == NULL) {
        PERROR_FALLOC;
        free(hmap);
        return NULL;
    }

    add_coeff(hmap);

    hmap->is_multi_set = is_multi_set;

    return hmap;
}

void uhmap_addelm(struct uhmap * *hmap, const void *new_elem, const size_t size_elem) {
    if ((hmap == NULL) || (*hmap == NULL) || (new_elem == NULL)) {
        PERROR_NULLPTR;
        return;
    }

    if (size_elem == 0)
        return;

    size_t hash_1 = uhash_1(*hmap, new_elem, size_elem);
    size_t hash_2 = uhash_2(*hmap, new_elem, size_elem);
    size_t hash   = hash_1;

    assert(hash_1 < (*hmap)->map_size);
    assert(hash_2 < (*hmap)->map_size);
    assert(hash_2 % 2 != 0);

    if ((*hmap)->is_multi_set != 0) {
        while ((((*hmap)->map + hash)->size != 0) && (((*hmap)->map + hash)->size != DELETED)) {
            hash = (hash + hash_2) % (*hmap)->map_size;
            
            if (hash == hash_1) {
                uhmap_resize(hmap, (*hmap)->map_size * 2);
                uhmap_addelm(hmap, new_elem, size_elem);
                return;
            }
        }

    } else {
        while ((((*hmap)->map + hash)->size != 0) && (((*hmap)->map + hash)->size != DELETED)) {
            if ((((*hmap)->map + hash)->size == size_elem) && 
                (!memcmp(((*hmap)->map + hash)->content, new_elem, size_elem)))
                    return;

            hash = (hash + hash_2) % (*hmap)->map_size;
            
            if (hash == hash_1) {
                uhmap_resize(hmap, (*hmap)->map_size * 2);
                uhmap_addelm(hmap, new_elem, size_elem);
                return;
            }
        }
    }

    ((*hmap)->map + hash)->content = calloc(1, size_elem);

    if (((*hmap)->map + hash)->content == NULL) {
        PERROR_FALLOC;
        return;
    }

    memcpy(((*hmap)->map + hash)->content, new_elem, size_elem);

    ((*hmap)->map + hash)->size = size_elem;

    (*hmap)->num_elem++;
}

void uhmap_delelm(struct uhmap *hmap, const void *del_elem, const size_t size_elem) {
    if ((hmap == NULL) || (del_elem == NULL)) {
        PERROR_NULLPTR;
        return;
    }

    if (size_elem == 0)
        return;

    size_t del_pos = uhmap_search(hmap, del_elem, size_elem);

    if (del_pos == UHMENF) {
        PERROR_PREFIX; fprintf(stderr, "del element not found\n");
        return;
    }

    free((hmap->map + del_pos)->content);

    (hmap->map + del_pos)->size = DELETED;

    hmap->num_elem--;
}

size_t uhmap_search(struct uhmap *hmap, const void *ser_elem, const size_t size_elem) {
    if ((hmap == NULL) || (ser_elem == NULL)) {
        PERROR_NULLPTR;
        return UHMENF;
    }

    if (size_elem == 0)
        return UHMENF;

    size_t hash_1 = uhash_1(hmap, ser_elem, size_elem);
    size_t hash_2 = uhash_2(hmap, ser_elem, size_elem);
    size_t hash   = hash_1;

    while ((hmap->map + hash)->size != 0) {
        if (((hmap->map + hash)->size == size_elem) && (!memcmp((hmap->map + hash)->content, ser_elem, size_elem)))
            return hash;

        hash = (hash + hash_2) % hmap->map_size;

        if (hash == hash_1)
            return UHMENF;
    }

    return UHMENF;
}

size_t uhmap_ctnelm(struct uhmap *hmap, const void *ser_elem, const size_t size_elem) {
    if ((hmap == NULL) || (ser_elem == NULL)) {
        PERROR_NULLPTR;
        return UHMENF;
    }

    if (size_elem == 0)
        return 0;

    size_t hash_1 = uhash_1(hmap, ser_elem, size_elem);
    size_t hash_2 = uhash_2(hmap, ser_elem, size_elem);
    size_t hash   = hash_1;
    size_t num    = 0;

    while ((hmap->map + hash)->size != 0) {
        if (((hmap->map + hash)->size == size_elem) && (!memcmp((hmap->map + hash)->content, ser_elem, size_elem)))
            num++;

        hash = (hash + hash_2) % hmap->map_size;

        if (hash == hash_1)
            return num;
    }

    return num;
}

void uhmap_delmap(struct uhmap * *hmap) {
    if ((hmap == NULL) || (*hmap == NULL)) {
        PERROR_NULLPTR;
        return;
    }

    for (size_t i = 0; i < (*hmap)->map_size; i++) 
        if ((((*hmap)->map + i)->size != 0) && (((*hmap)->map + i)->size != DELETED))
            free(((*hmap)->map + i)->content);

    free((*hmap)->map);
    free(*hmap);
}

void uhmap_savetb(struct uhmap *hmap, const char *binary_file) {
    if ((hmap == NULL) || (binary_file == NULL)) {
        PERROR_NULLPTR;
        return;
    }

    FILE *bf = fopen(binary_file, "wb");

    if (bf == NULL) {
        perror("Failure opening file: ");
        return;
    }

    fprintf(bf, "%.8lu", SIGNATURE);
    fprintf(bf, "%.8lu", hmap->map_size);
    fprintf(bf, "%.8lu", hmap->is_multi_set);
    
    for (size_t i = 0; i < hmap->map_size; i++) {
        fprintf(bf, "%.8lu", (hmap->map + i)->size);

        if (((hmap->map + i)->size != 0) && ((hmap->map + i)->size != DELETED))
            for (size_t k = 0; k < (hmap->map + i)->size; k++)
                fprintf(bf, "%c", *(((char *) (hmap->map + i)->content) + k));
    }

    fprintf(bf, "%.4d",  hmap->ah1);
    fprintf(bf, "%.4d",  hmap->ah2);
    fprintf(bf, "%.8lu", hmap->num_elem);

    fclose(bf);
}

struct uhmap *uhmap_readbf(const char *binary_file) {
    if (binary_file == NULL) {
        PERROR_NULLPTR;
        return NULL;
    }

    FILE *bf = fopen(binary_file, "rb");

    if (bf == NULL) {
        perror("Failure opening file: ");
        return NULL;
    }

    size_t signature = 0;
    size_t num_readed_elm = 0;

    num_readed_elm = fscanf(bf, "%8lu", &signature);

    if (signature != SIGNATURE) {
        PERROR_PREFIX; fprintf(stderr, "incorrect file signature\n");
        fclose(bf);
        return NULL;
    }

    size_t map_size = 0;
    size_t is_multi_set = 0;

    num_readed_elm  = fscanf(bf, "%8lu", &map_size);
    num_readed_elm += fscanf(bf, "%8lu", &is_multi_set);

    if (num_readed_elm != 2) {
        PERROR_PREFIX; fprintf(stderr, "failed fscanf\n");
        fclose(bf);
        return NULL;
    }

    struct uhmap *hmap = new_uhmap(map_size, is_multi_set);

    if (hmap == NULL) {
        PERROR_PREFIX; fprintf(stderr, "failed new_uhmap\n");
        fclose(bf);
        return NULL;
    }

    for (size_t i = 0; i < map_size; i++) {
        num_readed_elm = fscanf(bf, "%8lu", &((hmap->map + i)->size));

        if (num_readed_elm == 0) {
            PERROR_PREFIX; fprintf(stderr, "failed fscanf\n");
            uhmap_delmap(&hmap); fclose(bf);
            return NULL;
        }

        if (((hmap->map + i)->size != 0) && ((hmap->map + i)->size != DELETED)) {
            (hmap->map + i)->content = calloc(1, (hmap->map + i)->size);
            
            if ((hmap->map + i)->content == NULL) {
                PERROR_FALLOC;
                uhmap_delmap(&hmap); fclose(bf);
                return NULL;
            }

            num_readed_elm = fread((hmap->map + i)->content, sizeof(char), (hmap->map + i)->size, bf);

            if (num_readed_elm != (hmap->map + i)->size) {
                PERROR_PREFIX; fprintf(stderr, "failed fread ");
                fprintf(stderr, "((num_readed_elm == %lu) != (size == %lu))\n", num_readed_elm, (hmap->map + i)->size);
                uhmap_delmap(&hmap); fclose(bf);
                return NULL;
            }
        }
    }

    num_readed_elm  = fscanf(bf, "%4d",  &(hmap->ah1));
    num_readed_elm += fscanf(bf, "%4d",  &(hmap->ah2));
    num_readed_elm += fscanf(bf, "%8lu", &(hmap->num_elem));

    if (num_readed_elm != 3) {
        PERROR_PREFIX; fprintf(stderr, "failed fscanf\n");
        uhmap_delmap(&hmap); fclose(bf);
        return NULL;
    }

    fclose(bf);
    
    return hmap;
}

void uhmap_resize(struct uhmap * *hmap, const size_t new_size) {
    if ((hmap == NULL) || (*hmap == NULL)) {
        PERROR_NULLPTR;
        return;
    }

    if (new_size < (*hmap)->num_elem) {
        PERROR_PREFIX; fprintf(stderr, "new size is too small (%lu < %lu)\n", new_size, (*hmap)->num_elem);
        return;
    }

    struct uhmap *new_hmap = new_uhmap(new_size, (*hmap)->is_multi_set);

    for (size_t i = 0; i < (*hmap)->map_size; i++) {
        if ((((*hmap)->map + i)->size != 0) && (((*hmap)->map + i)->size != DELETED)) {
            uhmap_addelm(&new_hmap, ((*hmap)->map + i)->content, ((*hmap)->map + i)->size);
            free(((*hmap)->map + i)->content);
        }
    }

    free((*hmap)->map);
    free(*hmap);

    *hmap = new_hmap;
}

static size_t uhash_1(struct uhmap *hmap, const void *new_elem, const size_t size_elem) {
    assert(hmap);
    assert(new_elem);
    assert(size_elem > 0);

    size_t res = 0;

    for (size_t i = 0; i < size_elem; i++)
        res = (res * hmap->ah1 + *((char *) (new_elem + i))) % hmap->map_size;

    assert(res < hmap->map_size);

    return res;
}

static size_t uhash_2(struct uhmap *hmap, const void *new_elem, const size_t size_elem) {
    assert(hmap);
    assert(new_elem);
    assert(size_elem > 0);

    size_t res = 0;

    for (size_t i = 0; i < size_elem; i++)
        res = (res * hmap->ah2 + *((char *) (new_elem + i))) % hmap->map_size;

    if (res % 2 == 0)
        res = (res + 1) % hmap->map_size;

    assert(res < hmap->map_size);

    return res;
}

static size_t next_number_pow_of_two(const size_t num) {
    size_t res = 0;
    unsigned char num_bits = sizeof(size_t) * NUM_BIT_IN_BYTE;
    unsigned char pos_1 = num_bits - 1;
    unsigned char pos_2 = 0;

    if (num == 0)
        return 2;

    while (((num >> pos_1) & 1u) == 0)
        pos_1--;

    while (((num >> pos_2) & 1u) == 0)
        pos_2++;

    if (pos_1 == pos_2) {
        return num;
    }

    if (pos_1 == num_bits - 1) {
        res = res | (1u << pos_1);
        PERROR_PREFIX; fprintf(stderr, "num is too large (%lu > %lu)\n", num, res);
        return res;
    }
    
    res = res | (1u << (pos_1 + 1));

    assert(res >= num);

    return res;
}

static void add_coeff(struct uhmap *hmap) {
    assert(hmap);

    hmap->ah1 = random() % hmap->map_size;
    hmap->ah2 = random() % hmap->map_size;
}


size_t uhmap_mpsize(struct uhmap *hmap) {
    if (hmap == NULL) {
        PERROR_NULLPTR;
        return 0;
    }

    return hmap->map_size;
}

size_t uhmap_numelm(struct uhmap *hmap) {
    if (hmap == NULL) {
        PERROR_NULLPTR;
        return 0;
    }

    return hmap->num_elem;
}

size_t uhmap_ismult(struct uhmap *hmap) {
    if (hmap == NULL) {
        PERROR_NULLPTR;
        return 0;
    }

    return hmap->is_multi_set;
}

size_t set_num_bit_in_byte(size_t num_bit_in_byte) {
    if (num_bit_in_byte == 0) {
        PERROR_PREFIX; fprintf(stderr, "num_bit_in_byte == 0\n");
        return NUM_BIT_IN_BYTE;
    }

    size_t prev = NUM_BIT_IN_BYTE;
    NUM_BIT_IN_BYTE = num_bit_in_byte;
    return prev;
}

#undef PERROR_PREFIX
#undef PERROR_FALLOC
#undef PERROR_NULLPTR
