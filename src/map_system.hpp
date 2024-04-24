#pragma once

#include <vector>
#include <random>
#include <algorithm>

#include "tiny_ecs_registry.hpp"
#include "render_system.hpp"
#include "common.hpp"
#include "physics_system.hpp"
#include "bsp_tree.hpp"
#include "global.hpp"
#include <visibility_system.hpp>

// Grid coordinates: x, y, size
struct Room {
	int id;
	int x;
	int y;
	vec2 size; // (x,y) = (width,height) of room
	std::vector<std::vector<int>> grid;
};

static int room_id = 0;

class MapSystem {
private:
	// game systems
	RenderSystem* renderer;
	VisibilitySystem* visibility_system;

	// old version
	void generate_rocks(std::vector<std::vector<int>>& map);

	std::vector<Room> rooms;

	// sandstone tile atlas
	TILE_NAME_SANDSTONE get_tile_name_sandstone(int x, int y, std::vector<std::vector<int>>& map);
	void generate_all_tiles(std::vector<std::vector<int>>& map);

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	// Binary space partitioning tree
	BSPTree bsptree;

	// Utility to check if map is valid
	// NOT VALID IF: FLOOR WALL FLOOR 
	// - this only applies immediately after generating map. you can place items, pillars, walls, after
	bool is_valid_map(std::vector<std::vector<int>>& map);
public:
	MapSystem();
	void restart_map();
	void init(RenderSystem* renderer, VisibilitySystem* visibility_arg);
	Room generateBossRoom();
	void generateRandomMap(float room_size);
	void generateTutorialMap();
	void generate_door_tiles(std::vector<std::vector<int>>& map);
	void printMap();
	void spawnEnemiesInRooms();
	// spawn enemies inside room
	void spawnEnemiesInRoom(Room_struct& room);
	std::vector<vec4> generateDoorInfo(std::vector<Room_struct>& rooms, std::vector<std::vector<int>>& map);
	void set_map_walls(std::vector<std::vector<int>>& map);
	void step(float elapsed_ms);

	// used only if there are rooms generated by bsp
	// spawn player in the middle of the specified room
	Entity spawnPlayerInRoom(int room_number);
	// spawn player in specified grid coordinate
	Entity spawnPlayer(coord grid_coord);
};