#pragma once

//this hash map implementation is based at double hashing principle
//the size of the hash map is 2^k and the hash function return odd numbers
struct uhmap;

static const size_t UHMENF = -0xF;

struct uhmap *new_uhmap(const size_t initial_quantity, const size_t is_multi_set);
struct uhmap *uhmap_readbf(const char *binary_file);
size_t uhmap_search(struct uhmap *hmap, const void *ser_elem, const size_t size_elem);
size_t uhmap_ctnelm(struct uhmap *hmap, const void *ser_elem, const size_t size_elem);
void   uhmap_delelm(struct uhmap *hmap, const void *del_elem, const size_t size_elem);
void   uhmap_savetb(struct uhmap *hmap, const char *binary_file);
void   uhmap_addelm(struct uhmap * *hmap, const void *new_elem, const size_t size_elem);
void   uhmap_resize(struct uhmap * *hmap, const size_t new_size);
void   uhmap_delmap(struct uhmap * *hmap);

size_t uhmap_mpsize(struct uhmap *hmap);
size_t uhmap_numelm(struct uhmap *hmap);
size_t uhmap_ismult(struct uhmap *hmap);

size_t set_num_bit_in_byte(size_t num_bit_in_byte);
