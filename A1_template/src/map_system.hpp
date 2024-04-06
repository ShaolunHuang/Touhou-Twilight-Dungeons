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

	void addTile(int x, int y, std::vector<TEXTURE_ASSET_ID>& textureIDs, std::vector<std::vector<int>>& map);
	void generateAllEntityTiles(std::vector<std::vector<int>>& map);
	std::vector<TEXTURE_ASSET_ID> getTileAssetID(int row, int col, std::vector<std::vector<int>>& map);

	std::vector<Room> rooms;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	// Binary space partitioning tree
	BSPTree bsptree;
public:
	MapSystem();
	void restart_map();
	void init(RenderSystem* renderer);
	std::vector<std::vector<int>> world_map; // world_map[Row][Col]
	void generateBasicMap();
	Room& generateBossRoom();
	void generateRandomMap();
	void generateTutorialMap();
	void printMap();
	void spawnEnemiesInRoom();
	// used only if there are rooms generated by bsp
	// spawn player in the middle of the specified room
	Entity spawnPlayerInRoom(int room_number);
	// spawn player in specified grid coordinate
	Entity spawnPlayer(coord grid_coord);
};