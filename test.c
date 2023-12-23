#include <stdio.h>
#include "uhashmap.h"
#include "readstrings.h"

int main() {
    struct uhmap *hmap = new_uhmap(5, 0);

    uhmap_addelm(&hmap, "blablabla", 9);
    uhmap_addelm(&hmap, "abcdefghi", 9);
    uhmap_addelm(&hmap, "cheesetrp", 9);
    uhmap_addelm(&hmap, "kfmkfwioq", 9);
    uhmap_addelm(&hmap, "16oieks96", 9);

    printf("s = %lu\n", uhmap_search(hmap, "blablabla", 9));

    uhmap_savetb(hmap, "test.bin");

    uhmap_delmap(&hmap);

    hmap = uhmap_readbf("test.bin");

    printf("s = %lu\n", uhmap_search(hmap, "blablabla", 9));
    printf("s = %lu\n", uhmap_search(hmap, "blabbla",   7));
    printf("s = %lu\n", uhmap_search(hmap, "abcdefghi", 9));
    printf("s = %lu\n", uhmap_search(hmap, "kfmkfwioq", 9));

    uhmap_addelm(&hmap, "carrot",    6);
    uhmap_addelm(&hmap, "pen",       3);
    uhmap_addelm(&hmap, "apple",     5);
    uhmap_addelm(&hmap, "blue",      4);
    uhmap_addelm(&hmap, "pineapple", 9);
    uhmap_addelm(&hmap, "pineapple", 9);
    uhmap_addelm(&hmap, "pineapple", 9);
    uhmap_addelm(&hmap, "pineapple", 9);


    uhmap_delelm(hmap, "4", 1);
    uhmap_delelm(hmap, "kfmkfwioq", 9);

    uhmap_savetb(hmap, "test1.bin");

    uhmap_delmap(&hmap);

    return 0;
}
