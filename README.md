# Main pionts
* [Description](#description)
* [Using](#using)
* [Links](#links)

# Description
## About program
This program is a fairly primitive implementation of hash tables, based on the double hashing method. It allows you to work with data hashing, as well as save a table to a binary file and read from it. The program is designed in such a way that it supports threading. But it should be noted that if you are working on a machine where there are 8 bits in one byte, and you have different numbers of bits in one byte on different cores, please ask me, I’m not ready for this yet. In this case, you will have to be content with one core.

## Implementation
As mentioned earlier, the algorithm is built on the principle of double hashing. This allows you to most efficiently distribute data across the table to comply with O(1) asymptotics for all commands such as include, exclude, and search for an element.

The table size is always a power of two. The values ​​produced by the static function hash_2 and the size of the table should be relatively prime. And the hash_2 function always produces odd values. If you provide a number that is not a power of two, the program itself will convert it to the nearest one from above, which is equal to the power of two. 

When the table is completely filled, a new one is automatically created, all the values from the old one are rewritten there, the old table is deleted, and the pointer to the old table is replaced by a pointer to the new one. This is a very long process, working in O(n) time.

# Using
## Start using
In order to start using this hash table, install the repository on your computer. If you want to make a dynamic library from it, type in Terminal (using gcc):
```
$ gcc -O2 -fPIC -DNDEBUG uhashmap.c -c -o uhashmap.o
$ gcc -shared uhashmap.o -o libuhashmap.so
```
Or, to create a static library: 
```
$ gcc -O2 -DNDEBUG uhashmap.c -c -o uhashmap.o
$ ar -cr libuhashmap.a uhashmap.o
```
## Commands
If you are working on a machine where there are not 8 bits in one byte, you can adjust this number before using the functions by calling the `size_t set_num_bit_in_byte(size_t num_bit_in_byte)` function. To work with the table, pay attention to the following commands:

* The table is created using the function:
```
struct uhash_map *new_uhmap(const size_t initial_quantity, const size_t is_multi_set); 
```
* The next function is used to read a table created by the same program from a binary file:
```
struct uhash_map *uhmap_readbf(const char *binary_file);
```
* The following function is used to search for a given element in a table:
```
size_t uhmap_search(struct uhash_map *hmap, const void *ser_elem, const size_t size_elem);
```
If the element is not found, the function will return UHMENF (UHASH MAP ELEMENT NOT FOUND = (size_t) -0xF).

* Removing an element:
```
void   uhmap_delelm(struct uhash_map *hmap, const void *del_elem, const size_t size_elem);
```
* Saving table to file:
```
void   uhmap_savetb(struct uhash_map *hmap, const char *binary_file);
```
* Adding an element:
```
void   uhmap_addelm(struct uhash_map * *hmap, const void *new_elem, const size_t size_elem);
```
* Resizing a table:
```
void   uhmap_resize(struct uhash_map * *hmap, const size_t new_size);
```
* Deleting a table:
```
void   uhmap_delmap(struct uhash_map * *hmap);
```
* Getting the table size:
```
size_t uhmap_mpsize(struct uhash_map *hmap);
```
* Getting the number of elements in a table:
```
size_t uhmap_numelm(struct uhash_map *hmap);
```
Errors that occur during program execution will be printed to stderr.

# Links
- [Hash table](https://en.wikipedia.org/wiki/Hash_table)
- [Hash map C++](https://en.cppreference.com/w/cpp/container/unordered_map)
