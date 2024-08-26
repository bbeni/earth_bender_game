// Copyright (C) 2024 Benjamin Froelich
// This file is part of https://github.com/bbeni/earth_bender_game
// For conditions of distribution and use, see copyright notice in project root.

#pragma once
#include <stdbool.h>
#include "data_stuff.h"

//
// read files from disk (or resources -> will be implemented later) 
//

bool read_entire_file(const char* file_path, String_Builder* sb);
bool load_resource_from_file(const char* file_path, int* out_size, char** out_data);
bool load_resource(const char* file_path, int* out_size, char** out_data);

struct Image;
Image load_image_resource(char* file_path);