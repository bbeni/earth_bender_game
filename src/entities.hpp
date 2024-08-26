// Copyright (C) 2024 Benjamin Froelich
// This file is part of https://github.com/bbeni/earth_bender_game
// For conditions of distribution and use, see copyright notice in project root.

/* 3 Approaches i am debating which one: trying to describe
* 
* 1. Approach: Have a tagged Entity struct containing a union of all possible
*	Types that owns basic data that is frequently used and not too big.
*	Then a big array of all entities is created at load time and all entities live there.
*	extra data get's stored in other places.
* 
*	Benefits:
*		- Convenient to write code that loops through all entities and renders them.
*		- Simple to manage destroying and don't have to think about allocating any space
			at playtime
		- We can just cast the entity to its true form and use all stuff after we checked
			the type. That is quite simple
	
	Drawbacks:
		- Waste of memory -> cache misses
		- looping through just one type of entity is wasteful, as we have to go through all
			or have some index ranges that know where each stuff lives, which is annoying
* 
* 2. Approach: Have a dynamic array for each type. an overloaded entity_add() function can 
	dispatch between arrays. The structs can be any size we want.

	Benefits:
		- Good memory locality and friendly -> decreased cache misses and memory footprint
		- Render firendly -> we render this type in this pass -> easy loop over all 'water_tiles'
		- more tunable and robust, we have type safty given!

	Drawbacks:
		- New types need to be explicitly added to rendering, but that's not so bad
		- rewind in editor might be harder to implement
		- deleting stuff is harder to manage, as we have to delete the appropriate array
		- need more mallocs for each array one, and have a good guess what might be the maximum.

  3. Approach: Hyprid of both. Have each thing have an entity pointer, that is managed by a list of entity_entries
	in the entitiy_manager. The manager is responible for all allocations and frees and therefore 
	also has the arrays of specific types. If a new entity is added it finds the first free spot in the 
	appropriate list (overload function). deletion should always have the arrrays stay contiguous.

	Benefits:
		- fast rendering
		- if enough space -> no mallocs!
		- feels good

	Drawbacks:
		- complicated
		- deletion
		- 
*	
*/

#pragma once
#include "mathematics.hpp"
#include <cstdint>

struct Entity;
struct New_Bender {
	Entity* entity;
	int strength = 10;
};

struct New_Tile {
	Entity* entity;
	bool is_ramp;
};

struct New_Monster{
	Entity* entity;
	uint16_t heads;
};

struct Entity {
	Vec3 postition;
	Vec3 orientation; // euler angles for now TODO: implement quaternions
	Vec3 scale;
	
	uint32_t id;

	union {
		New_Bender* new_bender;
		New_Tile* new_tile;
		New_Monster* new_monster;
	};
};

struct Entity_Array {
	size_t count;
	size_t capacity;
	Entity* data;
};


struct Entity_Organizer {
	bool initialized;
	Entity_Array entities;
};

static const size_t INITIAL_ENTITY_CAP = 1024;

Entity_Organizer g_entity_organizer;

Entity_Organizer* open_organizer() {
	if (!g_entity_organizer.initialized) {
		g_entity_organizer.initialized = true;
		// Allocate
	}
}

