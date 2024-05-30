// using https://www.online-utility.org/image/convert/to/XBM causes the bits to be flipped
// so I flipped back using this code I made:

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

const unsigned char bitmap[] = {
            /* BITMAP DATA FROM WEBSITE */ 0
};

#define pgm_read_byte(addr)   (*(const unsigned char *)(addr))

int main()
{
    std::cout<<"Hello World\n";

    printf("const unsigned char image[] = {\n    ");

    size_t drop = 0;
    for(auto& i : bitmap) {
        unsigned char inverted = 0;
        for(size_t off = 0; off < 8; ++off) {
            inverted |= (i & (1 << (7 - off))) ? (1 << off) : 0;
        }
        printf("0x%02X, ", inverted);
        if (++drop >= 12) {
            drop = 0;
            printf("\n    ");
        }
    }

    printf("}");

    return 0;
}
