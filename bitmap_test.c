#include "bitmap.h"

int main()
{
    struct Bitmap bitmap;

    bitmap_initialize(&bitmap);
    bitmap_setatindex(-1, &bitmap);
    bitmap_setatindex(7, &bitmap);
    bitmap_setatindex(7, &bitmap);
    bitmap_removeatindex(7, &bitmap);
    bitmap_setatindex(7, &bitmap);

    int block_size = 400;
    int index = bitmap_findemptyblockofsize(block_size, &bitmap);
    bitmap_setblockofsize(block_size, &bitmap);
    _bitmap_print_raw(&bitmap);

    bitmap_removeblockofsize(index, block_size, &bitmap);
    _bitmap_print_raw(&bitmap);

    return 0;
}
