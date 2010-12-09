#include <iostream>
using namespace std;

#include "inode.h"

int main()
{
    struct IndexNode node;
    cout<<sizeof(IndexNode)<<endl;

    inode_init(&node);
    cout<<"done"<<endl;
}

