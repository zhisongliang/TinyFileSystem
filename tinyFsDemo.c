#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include "libTinyFS.h"

int main(int argc, char *argv[])
{
    // open root directory
    DIR *dp = opendir(".");
    assert(dp != NULL);
    struct dirent *d;
    while ((d = readdir(dp)) != NULL)
    {
        printf("%lu %s\n", d->d_ino, d->d_name);
    }
    closedir(dp);
    return 0;
}