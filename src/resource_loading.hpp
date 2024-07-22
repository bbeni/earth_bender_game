#pragma once
#include <stdbool.h>
#include "data_stuff.h"

bool read_entire_file(const char* path, String_Builder* sb);
bool load_resource_from_file(const char* file_path, int* out_size, char** out_data);
bool load_resource(const char* file_path, int* out_size, char** out_data);
