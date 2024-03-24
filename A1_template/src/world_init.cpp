#include "world_init.hpp"

Entity createBullet(RenderSystem* renderer, float entity_speed, vec2 entity_position, float rotation_angle, vec2 direction, float bullet_speed, bool is_player_bullet, BulletPattern* bullet_pattern)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = rotation_angle;
	motion.position = entity_position; // bullet spawns from entity's center position
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ BULLET_BB_WIDTH, BULLET_BB_HEIGHT });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = bullet_speed;
	kinematic.speed_modified = 1.f * kinematic.speed_base + entity_speed; // bullet speed takes into account of entity's speed
	kinematic.direction = direction;

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = abs(motion.scale);

	// Create and (empty) bullet component to be able to refer to all bullets
	if (is_player_bullet) {
		registry.playerBullets.emplace(entity);
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BULLET,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		registry.enemyBullets.emplace(entity);
	}

	if (bullet_pattern) {
		registry.bulletPatterns.insert(entity, *bullet_pattern);
	}

	return entity;
}

Entity createHealth(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = 0.f;
	motion.scale = vec2({ HEALTH_WIDTH, HEALTH_HEIGHT });

	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = abs(motion.scale);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distrib(0, 1);
	double number = distrib(gen);
	auto& pickupable = registry.pickupables.emplace(entity);
	registry.colors.insert(entity, { 1,1,1 });
	if (number <= 0.6) {
		pickupable.health_change = 1;
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::HEALTH_1, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });

	}
	else if (number <= 0.9) {
		pickupable.health_change = 2;
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::HEALTH_2, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		pickupable.health_change = 100;
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::REGENERATE_HEALTH, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	return entity;
}

Entity createPlayer(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::REIMU_FRONT);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.scale = vec2({ PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 3.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = abs(motion.scale);

	// Set player collision box at the feet of the player
	collidable.size = { motion.scale.x / 32 * 24, motion.scale.y / 2.f };
	collidable.shift = { 0, motion.scale.y / 4.f };

	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = 6;
	hp.curr_hp = hp.max_hp;

	registry.players.emplace(entity);
	EntityAnimation player_ani;
	player_ani.isCursor = true;
	player_ani.spritesheet_scale = { 0.25, 0.125 };
	player_ani.render_pos = { 0.5, 0.125 };
	registry.animation.insert(entity, player_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLAYER, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	BulletSpawner bs;
	bs.fire_rate = 3;
	bs.is_firing = false;
	bs.bullet_initial_speed = 200;

	registry.bulletSpawners.insert(entity, bs);
	registry.colors.insert(entity, { 1,1,1 });

	return entity;
}

Entity createPlayerHeartUI(RenderSystem* renderer)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.scale = vec2({ HP_BB_WIDTH, HP_BB_HEIGHT });

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FULL_HEART,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.playerHearts.emplace(entity);
	registry.colors.insert(entity, { 1,1,1 });

	return entity;
}

Entity createBossHealthBarUI(RenderSystem* renderer, Entity boss) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.scale = vec2({ BOSS_HEALTH_BAR_WIDTH, BOSS_HEALTH_BAR_HEIGHT });

	BossHealthBarUI& hb = registry.bossHealthBarUIs.emplace(entity);
	hb.is_visible = false;
	registry.bossHealthBarLink.emplace(entity, boss);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BOSS_HEALTH_BAR,
			EFFECT_ASSET_ID::BOSSHEALTHBAR,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createCoin(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ BULLET_BB_WIDTH, BULLET_BB_HEIGHT });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 50.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 1 };

	registry.pickupables.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BULLET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createBoss(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ ENEMY_BB_WIDTH / 1.2f, ENEMY_BB_HEIGHT / 1.2f });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = abs(vec2(motion.scale.x / 1.4f, motion.scale.y / 1.2f));

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = 102;
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;

	// Animation
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 1.f / 4.f, 1.f / 4.f };
	enemy_ani.render_pos = { 1.f / 4.f, 1.f / 4.f };
	registry.animation.insert(entity, enemy_ani);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BOSS,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	Boss& boss = registry.bosses.emplace(entity);
	// Boss bullet patterns
	boss.health_phase_thresholds = { 100, 75, 50, 25, -1 }; // -1 for end of phase
	boss.duration = 10000; // duration for each pattern
	registry.colors.insert(entity, { 1,1,1 });

	// Boss health bar ui
	Entity ui_entity = createBossHealthBarUI(renderer, entity);
	registry.bossHealthBarLink.emplace(entity, ui_entity);

	return entity;
}

Entity createBeeEnemy(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ ENEMY_BB_WIDTH, ENEMY_BB_HEIGHT });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = abs(motion.scale / 2.f);

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = 6;
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;
	registry.beeEnemies.emplace(entity);
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 0.166, 0.125 };
	enemy_ani.render_pos = { 0.166, 0.125 };
	registry.animation.insert(entity, enemy_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_BEE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.idleMoveActions.emplace(entity);

	BulletSpawner bs;
	bs.fire_rate = 35;
	bs.is_firing = true;
	bs.bullet_initial_speed = 150;

	registry.bulletSpawners.insert(entity, bs);
	registry.colors.insert(entity, { 1,1,1 });

	registry.aitimers.emplace(entity);

	return entity;
}

Entity createBomberEnemy(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ ENEMY_BB_WIDTH, ENEMY_BB_HEIGHT });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.f;

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = 2;
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 2;

	registry.bomberEnemies.emplace(entity);
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 1.f / 6.f, 1.f / 11.f };
	enemy_ani.render_pos = { 1.f / 6.f, 1.f / 11.f };
	registry.animation.insert(entity, enemy_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_BOMBER,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.idleMoveActions.emplace(entity);
	registry.colors.insert(entity, { 1,1,1 });

	AiTimer& aitimer = registry.aitimers.emplace(entity);
	aitimer.update_base = 1000; // updates decision tree every second
	aitimer.update_timer_ms = 1000;

	return entity;
}

Entity createWolfEnemy(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ ENEMY_BB_WIDTH, ENEMY_BB_HEIGHT });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.f;

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = 3;
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;

	registry.wolfEnemies.emplace(entity);
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 1.f / 6.f, 1.f / 12.f };
	enemy_ani.render_pos = { 1.f / 6.f, 5.f / 12.f };
	registry.animation.insert(entity, enemy_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_WOLF,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.idleMoveActions.emplace(entity);

	BulletSpawner bs;
	bs.fire_rate = 60;
	bs.is_firing = true;
	bs.bullet_initial_speed = 150;
	bs.bullets_per_array = 3;
	bs.spread_within_array = 30;

	registry.bulletSpawners.insert(entity, bs);
	registry.colors.insert(entity, { 1,1,1 });

	AiTimer& aitimer = registry.aitimers.emplace(entity);
	aitimer.update_base = 500; // updates decision tree every second
	aitimer.update_timer_ms = 500;

	return entity;
}

Entity createSubmachineGunEnemy(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ ENEMY_BB_WIDTH, ENEMY_BB_HEIGHT });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale;

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = 6;
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;

	registry.submachineGunEnemies.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_BEE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.idleMoveActions.emplace(entity);
	BulletSpawner enemy_bullet_rate;
	enemy_bullet_rate.fire_rate = 5;
	enemy_bullet_rate.is_firing = true;
	registry.bulletSpawners.insert(entity, enemy_bullet_rate);
	registry.colors.insert(entity, { 1,1,1 });

	return entity;
}

std::vector<Entity> createFloor(RenderSystem* renderer, vec2 position, std::vector<TEXTURE_ASSET_ID> textureIDs) {
	std::vector<Entity> entities;
	for (int i = 0; i < textureIDs.size(); i++) {
		auto entity = Entity();

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(entity, &mesh);

		// Initialize the motion
		auto& motion = registry.motions.emplace(entity);
		motion.angle = 0.f;
		motion.position = position;
		motion.scale = vec2(world_tile_size, world_tile_size);

		// TODO: remove this, used for testing ai can see player
		if (textureIDs[i] == TEXTURE_ASSET_ID::PILLAR_TOP) {
			registry.floors.emplace(entity);
			registry.renderRequestsForeground.insert(
				entity,
				{ textureIDs[i],
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
			entities.push_back(entity);
			continue;
		}

		// Create and (empty) Tile component to be able to refer to all decoration tiles
		registry.floors.emplace(entity);
		registry.renderRequests.insert( // TODO Change to ground texture
			entity,
			{ textureIDs[i],
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
		entities.push_back(entity);
	}
	return entities;
}

std::vector<Entity> createWall(RenderSystem* renderer, vec2 position, std::vector<TEXTURE_ASSET_ID> textureIDs) {
	std::vector<Entity> entities;
	for (int i = 0; i < textureIDs.size(); i++) {
		auto entity = Entity();

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(entity, &mesh);

		// Initialize the motion
		auto& motion = registry.motions.emplace(entity);
		motion.angle = 0.f;
		motion.position = position;
		motion.scale = vec2(world_tile_size, world_tile_size);

		// Set the collision box
		auto& collidable = registry.collidables.emplace(entity);

		if (textureIDs[i] == TEXTURE_ASSET_ID::LEFT_WALL) {
			collidable.size = { motion.scale.x, motion.scale.y };
			collidable.shift = { 0, 0 };
		}
		else if (textureIDs[i] == TEXTURE_ASSET_ID::RIGHT_WALL) {
			collidable.size = { motion.scale.x, motion.scale.y };
			collidable.shift = { 0, 0 };
		}
		else if (textureIDs[i] == TEXTURE_ASSET_ID::BOTTOM_WALL) {
			collidable.size = { motion.scale.x, motion.scale.y };
			collidable.shift = { 0, 0 };
		}
		else if (textureIDs[i] == TEXTURE_ASSET_ID::WALL_SURFACE) {
			collidable.size = { motion.scale.x, motion.scale.y };
			collidable.shift = { 0, 0 };
		}
		else if (textureIDs[i] == TEXTURE_ASSET_ID::LEFT_TOP_CORNER_WALL ||
			textureIDs[i] == TEXTURE_ASSET_ID::LEFT_BOTTOM_CORNER_WALL ||
			textureIDs[i] == TEXTURE_ASSET_ID::RIGHT_TOP_CORNER_WALL ||
			textureIDs[i] == TEXTURE_ASSET_ID::RIGHT_BOTTOM_CORNER_WALL) {
			collidable.size = { motion.scale.x, motion.scale.y };
			collidable.shift = { 0, 0 };
		}
		else
			// TODO: remove this, used for testing ai can see player
			if (textureIDs[i] == TEXTURE_ASSET_ID::PILLAR_BOTTOM) {
				collidable.size = { motion.scale.x, motion.scale.y / 2 };
				collidable.shift = { 0, -motion.scale.y / 4 };
			}
			else {
				// Temporary
				// TODO: Maybe change/refactor this since it's adding floors when its in createWall
				registry.collidables.remove(entity);
				registry.floors.emplace(entity);
				registry.renderRequests.insert(
					entity,
					{ textureIDs[i],
					 EFFECT_ASSET_ID::TEXTURED,
					 GEOMETRY_BUFFER_ID::SPRITE });
				entities.push_back(entity);
				continue;
			}

		// Create and (empty) Tile component to be able to refer to all physical tiles
		registry.walls.emplace(entity);
		registry.renderRequests.insert(
			entity,
			{ textureIDs[i],
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });

		entities.push_back(entity);
	}
	return entities;
}

// IMPORTANT: creates pillar using grid coordinates, NOT world coorindates
// textureIDs[0] == bottom, textureIDs[1] == top
std::vector<Entity> createPillar(RenderSystem* renderer, vec2 grid_position, std::vector<TEXTURE_ASSET_ID> textureIDs) {
	assert(textureIDs.size() == 2 && "textureIDs do not have size 2");
	assert((!is_valid_cell(grid_position.x, grid_position.y) ||
		!(grid_position.y < 0 ||
			grid_position.x < 0 ||
			grid_position.y >= world_height ||
			grid_position.x >= world_width)) && "Pillar position not valid");

	std::vector<Entity> entities;
	auto bottom_entity = Entity();
	auto top_entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& bottom_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(bottom_entity, &bottom_mesh);
	Mesh& top_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(top_entity, &top_mesh);

	// Initialize the motion
	vec2 bottom_world_position = convert_grid_to_world(grid_position);

	auto& bottom_motion = registry.motions.emplace(bottom_entity);
	bottom_motion.position = bottom_world_position;
	bottom_motion.scale = vec2(world_tile_size, world_tile_size);
	auto& top_motion = registry.motions.emplace(top_entity);
	top_motion.position = bottom_world_position + vec2{ 0, -world_tile_size };
	top_motion.scale = vec2(world_tile_size, world_tile_size);

	// Set the collision box
	auto& bottom_collidable = registry.collidables.emplace(bottom_entity);
	bottom_collidable.size = { bottom_motion.scale.x, bottom_motion.scale.y };
	bottom_collidable.shift = { 0, 0 };

	WorldSystem::world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;

	registry.walls.emplace(bottom_entity);
	registry.floors.emplace(top_entity); // TODO: maybe foreground.emplace

	registry.renderRequests.insert(
		bottom_entity,
		{ textureIDs[0],
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	registry.renderRequestsForeground.insert(
		top_entity,
		{ textureIDs[1],
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	entities.push_back(top_entity);
	entities.push_back(bottom_entity);

	return entities;
}

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::EGG,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

// TEMPORARY EGG (remove/refactor later)
Entity createEgg(vec2 pos, vec2 size)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.scale = size;

	registry.deadlys.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::EGG,
			GEOMETRY_BUFFER_ID::EGG });

	return entity;
}

