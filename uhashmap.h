#pragma once

//this hash map implementation is based at double hashing principle
//the size of the hash map is 2^k and the hash function return odd numbers
struct uhash_map;

static const size_t UHMENF = -0xF;

struct uhash_map *new_uhmap(const size_t initial_quantity, const size_t is_multi_set);
struct uhash_map *uhmap_readbf(const char *binary_file);
size_t uhmap_search(struct uhash_map *hmap, const void *ser_elem, const size_t size_elem);
void   uhmap_delelm(struct uhash_map *hmap, const void *del_elem, const size_t size_elem);
void   uhmap_savetb(struct uhash_map *hmap, const char *binary_file);
void   uhmap_addelm(struct uhash_map * *hmap, const void *new_elem, const size_t size_elem);
void   uhmap_resize(struct uhash_map * *hmap, const size_t new_size);
void   uhmap_delmap(struct uhash_map * *hmap);

size_t uhmap_mpsize(struct uhash_map *hmap);
size_t uhmap_numelm(struct uhash_map *hmap);
size_t uhmap_ismult(struct uhash_map *hmap);

size_t set_num_bit_in_byte(size_t num_bit_in_byte);
