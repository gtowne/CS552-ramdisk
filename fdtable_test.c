#include <stdio.h>
#include <stdlib.h>

#include "fdtable.h"

int main()
{
    struct Fdtable thetable;
    fdtable_initialize(1, &thetable);

    printf("Created fd: %d\n", fdtable_createentry(23, &thetable));
    printf("Created fd: %d\n", fdtable_createentry(54, &thetable));
    printf("Created fd: %d\n", fdtable_createentry(65, &thetable));

    int ii;

    for(ii = 0; ii < 20; ii++)
    {
        printf("Created fd: %d\n", fdtable_createentry(ii, &thetable));
    }

    printf("\nTABLE BEFORE:\n");
    _fdtable_print(&thetable);

    fdtable_seekwithfd(5, 20, &thetable);
    fdtable_seekwithinodenum(54, 77, &thetable);
    fdtable_removeatfd(7, &thetable);
    fdtable_removeatinodenum(6, &thetable);

    printf("\nTABLE AFTER:\n");
    printf("fdtable_seekwithfd(5, 20, &thetable);\n");
    printf("fdtable_seekwithinodenum(54, 77, &thetable);\n");
    printf("fdtable_removeatfd(7, &thetable);\n");
    printf("fdtable_removeatinodenum(6, &thetable); \n\n");
    _fdtable_print(&thetable);

    return 0;
}
