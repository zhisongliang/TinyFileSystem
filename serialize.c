#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_FILE_NAME_LEN 8

typedef struct
{
    int inode_idx;
    uint8_t filename[MAX_FILE_NAME_LEN];
} Name_Inode_Pair;

void serialize_Name_Inode_Pair(const Name_Inode_Pair *pair, uint8_t *buffer)
{
    memcpy(buffer, pair, sizeof(Name_Inode_Pair));
}

void deserialize_Name_Inode_Pair(const uint8_t *buffer, Name_Inode_Pair *pair)
{
    memcpy(pair, buffer, sizeof(Name_Inode_Pair));
}

int main()
{
    Name_Inode_Pair original;
    original.inode_idx = 123;
    strncpy((char *)original.filename, "example", MAX_FILE_NAME_LEN);

    uint8_t buffer[sizeof(Name_Inode_Pair)];
    serialize_Name_Inode_Pair(&original, buffer);

    Name_Inode_Pair restored;
    deserialize_Name_Inode_Pair(buffer, &restored);

    printf("Restored: inode_idx = %d, filename = %s\n", restored.inode_idx, restored.filename);

    return 0;
}