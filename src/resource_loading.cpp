#pragma warning(disable:4996) // ty microsoft -> to be able to use fopen
#include "resource_loading.hpp"

#include <cassert>
//#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <errno.h>

// adapted function from nob.h github.com/tsoding/musializer 
// returns false on failure, should print the error
bool read_entire_file(const char* path, String_Builder* sb)
{
    bool result = true;
    long m;
    size_t new_count;
    FILE* f = fopen(path, "rb");

    if (f == NULL) {
        result = false;
        goto error;
    }
    if (fseek(f, 0, SEEK_END) < 0) {
        result = false;
        goto error;
    }
    m = ftell(f);
    if (m < 0) {
        result = false;
        goto error;
    }
    if (fseek(f, 0, SEEK_SET) < 0) {
        result = false;
        goto error;
    }

    new_count = sb->count + m;
    if (new_count > sb->capacity) {
        sb->data = (char*)realloc(sb->data, new_count);

        if (sb->data == NULL) {
            printf("Error: not enough RAM to allocate for file in read_entire_file().\n");
        }

        sb->capacity = new_count;
    }

    fread(sb->data + sb->count, m, 1, f);
    if (ferror(f)) {
        // TODO: Afaik, ferror does not set errno. So the error reporting in defer is not correct in this case.
        assert(false);
    }
    sb->count = new_count;

error:
    if (!result) {
        printf("Error: Could not read file %s: errno 0x%x\n", path, errno);
    }
    if (f) fclose(f);
    return result;
}

bool load_resource_from_file(const char* file_path, int* out_size, char** out_data) {
    String_Builder sb = { 0 };
    if (!read_entire_file(file_path, &sb)) {
        printf("ERROR: couldn't read the file (%s)\n", file_path);
        return false;
    }
    *out_data = sb.data;
    *out_size = sb.count;
    return true;
}

bool load_resource(const char* file_path, int* out_size, char** out_data) {
    // TODO implement package resources system
    return load_resource_from_file(file_path, out_size, out_data); // in case we didn't find it
}
