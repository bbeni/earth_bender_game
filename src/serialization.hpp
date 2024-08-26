// Copyright (C) 2024 Benjamin Froelich
// This file is part of https://github.com/bbeni/earth_bender_game
// For conditions of distribution and use, see copyright notice in project root.

#pragma once
#include "game.hpp"

const bool SERIALIZATION_ENABLED = true;

struct Code_Builder {
	char*  data;
	size_t count;
	size_t capacity;
};

void serialize_new_room_call(uint32_t depth, uint32_t width, uint32_t height);
void serialize_set_tile_call(uint32_t room_id, uint32_t i, uint32_t j, uint32_t elevation, Tile_Type type, Ramp_Orientation ramp);
void serialize_remove_tile_call(uint32_t room_id, uint32_t i, uint32_t j, uint32_t elevation);

void build_and_save_rooms_c_file(Room_Set* rooms, const char* filename);
void deserialize_rooms(Room_Set* roooms);