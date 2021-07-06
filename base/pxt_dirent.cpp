#ifdef _MSC_VER
#include "pxt.h"
#include <io.h>

DIR* opendir(const char* name)
{
    DIR* dir = 0;

    if (name && name[0])
    {
        size_t base_length = strlen(name);
        const char* all = strchr("/\\", name[base_length - 1]) ? "*" : "/*";

        if ((dir = (DIR*)malloc(sizeof(DIR))) != 0 &&
            (dir->name = (char*)malloc(base_length + strlen(all) + 1)) != 0)
        {
            strcat(strcpy(dir->name, name), all);

            if ((dir->handle = _findfirst(dir->name, &dir->info)) != -1)
            {
                dir->result.d_name = 0;
            }
            else /* rollback */
            {
                free(dir->name);
                free(dir);
                dir = 0;
            }
        }
        else
        {
            free(dir);
            dir = 0;
            errno = ENOMEM;
        }
    }
    else
    {
        errno = EINVAL;
    }

    return dir;
}

int closedir(DIR* dir)
{
    int result = -1;

    if (dir)
    {
        if (dir->handle != -1)
        {
            result = _findclose(dir->handle);
        }

        free(dir->name);
        free(dir);
    }

    if (result == -1)
    {
        errno = EBADF;
    }

    return result;
}

struct dirent* readdir(DIR* dir)
{
    struct dirent* result = 0;

    if (dir && dir->handle != -1)
    {
        if (!dir->result.d_name)
        {
            result = &dir->result;
            result->d_name = dir->info.name;
        }
        else
        {
            auto res = _findnext(dir->handle, &dir->info);
            if (res != -1)
            {
                result = &dir->result;
                result->d_name = dir->info.name;
            }
        }
    }
    else
    {
        errno = EBADF;
    }

    return result;
}

void rewinddir(DIR* dir)
{
    if (dir && dir->handle != -1)
    {
        _findclose(dir->handle);
        dir->handle = (long)_findfirst(dir->name, &dir->info);
        dir->result.d_name = 0;
    }
    else
    {
        errno = EBADF;
    }
}
#endif
