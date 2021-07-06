#ifndef __PXT_DIRENT_H__
#define __PXT_DIRENT_H__

#include <io.h>

struct dirent
{
    char* d_name;
};

struct DIR
{
    intptr_t handle;
    struct _finddata_t info;
    struct dirent result;
    char* name;
};

DIR* opendir(const char* name);
int closedir(DIR* dir);
struct dirent* readdir(DIR* dir);
void rewinddir(DIR* dir);


#endif