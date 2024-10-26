#include "world_init.hpp"
#include <iostream>

Entity createBullet(RenderSystem* renderer, float entity_speed, vec2 entity_position, float rotation_angle, vec2 direction, float bullet_speed, bool is_player_bullet, BulletPattern* bullet_pattern, bool is_aimbot_bullet)
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
	collidable.size = abs(motion.scale / 2.f);
	// Create and (empty) bullet component to be able to refer to all bullets
	if (is_player_bullet) {
		stats.bullets_fired++;
		auto& playerBullet = registry.playerBullets.emplace(entity);
		Player& player = registry.players.components[0];
		playerBullet.damage = player.bullet_damage;

		TEXTURE_ASSET_ID texture_asset = TEXTURE_ASSET_ID::BULLET; // default

		switch (player.ammo_type) {
		case AMMO_TYPE::NORMAL: {
			registry.normalBullets.emplace(entity);
			break;
		}
		case AMMO_TYPE::AIMBOT: {
			registry.aimbotBullets.emplace(entity);
			EntityAnimation ani;
			ani.frame_rate_ms = 100;
			ani.full_rate_ms = 100;
			ani.spritesheet_scale = { 1.f / 6.f, 1.f };
			ani.render_pos = { 1.f / 6.f, 1.f };
			ani.is_active = true;
			// aimbot bullet will stop playing after a while
			registry.alwaysplayAni.insert(entity, ani);

			// update scale to be larger
			motion.scale = 1.5f * vec2({ BULLET_BB_WIDTH, BULLET_BB_HEIGHT });
			collidable.size = abs(motion.scale / 2.f);

			texture_asset = TEXTURE_ASSET_ID::AIMBOT_AMMO_BULLET;
			break;
		}
		case AMMO_TYPE::AIMBOT1BULLET: {
			// is_aimbot_bullet is just to separate aimbot bullet and normal bullet texture
			if (is_aimbot_bullet) {
				registry.aimbotBullets.emplace(entity);
				EntityAnimation ani;
				ani.frame_rate_ms = 100;
				ani.full_rate_ms = 100;
				ani.spritesheet_scale = { 1.f / 6.f, 1.f };
				ani.render_pos = { 1.f / 6.f, 1.f };
				ani.is_active = true;
				// aimbot bullet will stop playing after a while
				registry.alwaysplayAni.insert(entity, ani);

				// update scale to be larger
				motion.scale = 1.5f * vec2({ BULLET_BB_WIDTH, BULLET_BB_HEIGHT });
				collidable.size = abs(motion.scale / 2.f);

				texture_asset = TEXTURE_ASSET_ID::AIMBOT_AMMO_BULLET;
			}
			else {
				registry.normalBullets.emplace(entity);
			}
			break;
		}
		case AMMO_TYPE::AOE: {
			registry.aoeBullets.emplace(entity);
			EntityAnimation ani;
			ani.frame_rate_ms = 1000.f / 10.f;
			ani.full_rate_ms = 1000.f / 10.f;
			ani.spritesheet_scale = { 1.f / 4.f, 1.f };
			ani.render_pos = { 1.f / 4.f, 1.f };
			ani.is_active = true;
			registry.alwaysplayAni.insert(entity, ani);

			// update scale to be larger
			motion.scale = 3.f * vec2({ BULLET_BB_WIDTH, BULLET_BB_HEIGHT });
			collidable.size = abs(motion.scale / 2.f);

			texture_asset = TEXTURE_ASSET_ID::AOE_AMMO_BULLET;
			break;
		}
		case AMMO_TYPE::TRIPLE: {
			registry.normalBullets.emplace(entity);
			break;
		}
		default:
			break;
		}

		// TODO: change bullet texture
		registry.renderRequests.insert(
			entity,
			{ texture_asset,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		// enemy bullets do not have render requests since they are instance rendered
		registry.enemyBullets.emplace(entity);
	}

	if (bullet_pattern) {
		registry.bulletPatterns.insert(entity, *bullet_pattern);
	}

	return entity;
}

Entity createBulletDisappear(RenderSystem* renderer, vec2 entity_position, float rotation_angle, bool is_player_bullet)
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

	// Set the collision box
	//auto& collidable = registry.collidables.emplace(entity);
	//collidable.size = abs(motion.scale);

	// Create and (empty) bullet component to be able to refer to all bullets
	if (is_player_bullet) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::REIMU_BULLET_DISAPPEAR,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::ENEMY_BULLET,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}
	EntityAnimation ani;
	ani.isCursor = false;
	ani.offset = 0;
	ani.frame_rate_ms = 50;
	ani.full_rate_ms = 50;
	ani.spritesheet_scale = { 0.25, 1 };
	ani.render_pos = { 0.25, 1 };
	registry.alwaysplayAni.insert(entity, ani);
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
	registry.kinematics.emplace(entity);
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = abs(motion.scale);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distrib(0, 1);
	double number = distrib(gen);
	double number_y = distrib(gen) / 2;
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
	vec2 dir = { 60 * (number - 0.5), 50 * (number_y) };
	BezierCurve curve;
	curve.bezier_pts.push_back(position);
	curve.bezier_pts.push_back(position + vec2(0, -20));
	curve.bezier_pts.push_back(position + dir);
	registry.bezierCurves.insert(entity, curve);
	return entity;
}

Entity createPurchasableHealth(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = 0.f;
	motion.scale = vec2({ HEALTH_WIDTH, HEALTH_HEIGHT });
	registry.kinematics.emplace(entity);
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = abs(motion.scale);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distrib(0, 1);
	double number = distrib(gen);
	auto& purchasable = registry.purchasableables.emplace(entity);
	registry.colors.insert(entity, { 1,1,1 });
	if (number <= 0.6) {
		purchasable.cost = 6;
		purchasable.effect_strength = 1;
		purchasable.effect_type = EFFECT_TYPE::INCR_CURRENT_HP;
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::HEALTH_1, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });

	}
	else if (number <= 0.9) {
		purchasable.cost = 10;
		purchasable.effect_strength = 2;
		purchasable.effect_type = EFFECT_TYPE::INCR_CURRENT_HP;
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::HEALTH_2, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		purchasable.cost = 20;
		purchasable.effect_strength = 100;
		purchasable.effect_type = EFFECT_TYPE::INCR_CURRENT_HP;
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::REGENERATE_HEALTH, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	return entity;
}

// We will only use cost in struct purchasable for ammo
Entity createPurchasableAmmo(RenderSystem* renderer, vec2 position, AMMO_TYPE ammo_type) {
	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = 0.f;
	motion.scale = 0.75f * vec2({ ITEM_WIDTH, ITEM_HEIGHT });
	registry.kinematics.emplace(entity);
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = abs(motion.scale);

	auto& purchasable = registry.purchasableables.emplace(entity);
	registry.colors.insert(entity, { 1,1,1 });

	// set default values in case ammo_type is invalid
	TEXTURE_ASSET_ID texture_asset = TEXTURE_ASSET_ID::HEALTH_1;
	purchasable.cost = 9999;

	switch (ammo_type) {
	case AMMO_TYPE::AIMBOT: {
		purchasable.cost = 100;
		purchasable.effect_type = EFFECT_TYPE::AIMBOT_AMMO;
		texture_asset = TEXTURE_ASSET_ID::AIMBOT_AMMO_ITEM;
		break;
	}
	case AMMO_TYPE::AOE: {
		purchasable.cost = 50;
		purchasable.effect_type = EFFECT_TYPE::AOE_AMMO;
		texture_asset = TEXTURE_ASSET_ID::AOE_AMMO_ITEM;
		break;
	}
	case AMMO_TYPE::TRIPLE: {
		purchasable.cost = 50;
		purchasable.effect_type = EFFECT_TYPE::TRIPLE_AMMO;
		texture_asset = TEXTURE_ASSET_ID::TRIPLE_AMMO_ITEM;
		break;
	}
	case AMMO_TYPE::AIMBOT1BULLET: {
		purchasable.cost = 25;
		purchasable.effect_type = EFFECT_TYPE::AIMBOT1BULLET_AMMO;
		texture_asset = TEXTURE_ASSET_ID::AIMBOT1BULLET_AMMO_ITEM;
		break;
	}
	case AMMO_TYPE::NORMAL: {
		purchasable.effect_type = EFFECT_TYPE::NORMAL_AMMO;
		purchasable.cost = 0;
		break;
	}
	default:
		break;
	}

	registry.renderRequests.insert(
		entity,
		{ texture_asset,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distrib(0, 1);
	double number = distrib(gen);
	double number_y = distrib(gen) / 2;
	vec2 dir = { 60 * (number - 0.5), 50 * (number_y) };
	BezierCurve curve;
	curve.bezier_pts.push_back(position);
	curve.bezier_pts.push_back(position + vec2(0, -20));
	curve.bezier_pts.push_back(position + dir);
	registry.bezierCurves.insert(entity, curve);
	return entity;
}

// creat purchase able items (boost/buff)
Entity createTreasure(RenderSystem* renderer, vec2 position, bool is_bezier)
{
	auto entity = Entity();
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = 0.f;
	motion.scale = vec2({ ITEM_WIDTH, ITEM_HEIGHT });
	registry.kinematics.emplace(entity);
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = abs(motion.scale);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distrib(0, 1);
	double number = distrib(gen);
	double number_y = distrib(gen) / 2;

	// Generate a random number between 1 and 6 for type
	// 1=bullet_damage, 2=fire_rate, 3=critical_hit, 4=max_health, 5=critical_dmg, 6=bomb

	std::uniform_int_distribution<> typeDistrib(EFFECT_TYPE::EFFECT_TYPE_SEPARATOR_0 + 1, EFFECT_TYPE::EFFECT_TYPE_SEPARATOR_1 - 2); // bomb is last, excludes bomb
	EFFECT_TYPE type = static_cast<EFFECT_TYPE>(typeDistrib(gen));

	Purchasableable& purchasableable = registry.purchasableables.emplace(entity);
	registry.colors.insert(entity, { 1,1,1 });

	if (number < 0.1) {
		type = EFFECT_TYPE::BOMB;
		purchasableable.effect_type = EFFECT_TYPE::BOMB;
		purchasableable.cost = 10;
		motion.scale = vec2({ 32, 32 });
	}
	else {
		purchasableable.cost = number * 10;
		purchasableable.effect_strength = number * 10;

		if (purchasableable.cost == 0) {
			purchasableable.cost = 1;
			purchasableable.effect_strength = 1;
		}
		purchasableable.cost *= 3;
		purchasableable.effect_type = type;
	}

	if (type == EFFECT_TYPE::BULLET_DAMAGE) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::ITEM_R, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });

	}
	else if (type == EFFECT_TYPE::FIRE_RATE) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::ITEM_B, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (type == EFFECT_TYPE::CRITICAL_HIT) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::ITEM_G, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (type == EFFECT_TYPE::INCR_MAXIMUM_HP) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::ITEM_Y, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (type == EFFECT_TYPE::CRITICAL_DAMAGE) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::ITEM_P, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (type == EFFECT_TYPE::BOMB) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BOMB, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		assert(false && "Unsupported effect type");
	}

	vec2 dir = { 60 * (number - 0.5), 50 * (number_y) };
	if (is_bezier) {
		BezierCurve curve;
		curve.bezier_pts.push_back(position);
		curve.bezier_pts.push_back(position + vec2(0, -20));
		curve.bezier_pts.push_back(position + dir);
		registry.bezierCurves.insert(entity, curve);
	}
	return entity;
}

Entity createParralex(RenderSystem* renderer, vec2 position) {
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
	motion.scale = vec2({ 4152, 2886 });
	Parralex& parra = registry.parrallaxes.emplace(entity);
	parra.parrallax_value = 0.82;
	parra.position = position;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PARRALEX,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createCloud(RenderSystem* renderer, vec2 position, float parrallax_value, float scale) {
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
	motion.scale = vec2({ 384, 128 }) * scale;
	Parralex& parra = registry.parrallaxes.emplace(entity);
	parra.parrallax_value = parrallax_value;
	parra.position = position;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CLOUDS,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}

Entity createCoin(RenderSystem* renderer, vec2 position, int value, float bezier_start, float bezier_end, vec2 bezier_up, float bezier_x_rand)
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

	/*auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 50.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 1 };*/

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distrib(bezier_start, bezier_end);
	std::uniform_real_distribution<> x_distrib(0.0, 1.0);

	// Generate a positive number
	double number = distrib(gen);
	double x_number = x_distrib(gen);

	// collidable
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.f;

	// value
	Coin& coin = registry.coins.emplace(entity);
	coin.coin_amount = value;
	vec2 dir = { bezier_x_rand * (x_number - 0.5f), 50 * (number) };
	BezierCurve curve;
	curve.bezier_pts.push_back(position);
	curve.bezier_pts.push_back(position + bezier_up);
	curve.bezier_pts.push_back(position + dir);
	registry.bezierCurves.insert(entity, curve);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::COIN,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });


	EntityAnimation ani;
	ani.isCursor = false;
	ani.offset = 0;
	ani.frame_rate_ms = 200;
	ani.full_rate_ms = 200;
	ani.spritesheet_scale = { 0.2, 1 };
	ani.render_pos = { 0.2, 1 };
	registry.alwaysplayAni.insert(entity, ani);

	return entity;
}

Entity createMaxHPIncrease(RenderSystem* renderer, vec2 position)
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

	/*need new scale*/
	motion.scale = vec2({ BULLET_BB_WIDTH, BULLET_BB_HEIGHT });

	/*auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 50.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 1 };*/

	// collidable
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.f;

	MaxHPIncrease& maxhpIncrease = registry.maxhpIncreases.emplace(entity);
	maxhpIncrease.max_health_increase = 1;

	// value
	Product& price = registry.products.emplace(entity);
	price.price = 60;

	/*need new texture*/
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BULLET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createAttackUp(RenderSystem* renderer, vec2 position)
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

	/*need new scale*/
	motion.scale = vec2({ BULLET_BB_WIDTH, BULLET_BB_HEIGHT });

	/*auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 50.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 1 };*/

	// collidable
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.f;

	AttackUp& attackUp = registry.attackUps.emplace(entity);
	attackUp.damageUp = 1;

	// value
	Product& price = registry.products.emplace(entity);
	price.price = 40;

	/*need new texture*/
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BULLET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createChest(RenderSystem* renderer, vec2 position)
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
	motion.scale = vec2({ world_tile_size, world_tile_size });

	// collidable
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = vec2(motion.scale.x * 2 / 3.f, motion.scale.y / 2.f);
	Chest& chest = registry.chests.emplace(entity);

	/*need new texture*/
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CHEST_CLOSE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createKey(RenderSystem* renderer, vec2 position)
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

	/*need new scale*/
	motion.scale = vec2({ BULLET_BB_WIDTH, BULLET_BB_HEIGHT });

	/*auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 50.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 1 };*/

	// collidable
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.f;

	Key& key = registry.keys.emplace(entity);

	/*need new texture*/
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BULLET,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

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
	kinematic.speed_base = 300.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	// Set player collision box at the feet of the player
	collidable.size = { motion.scale.x / 32 * 24, motion.scale.y / 2.f };
	collidable.shift = { 0, motion.scale.y / 4.f };

	// Set the collision circle
	auto& collidable_circle = registry.circleCollidables.emplace(entity);
	collidable_circle.radius = 6.f;
	collidable_circle.shift = { 0, motion.scale.y / 6.f };

	// HP
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
			EFFECT_ASSET_ID::PLAYER,
			GEOMETRY_BUFFER_ID::SPRITE });

	BulletSpawner bs;
	bs.fire_rate = 3;
	bs.is_firing = false;
	bs.bullet_initial_speed = 200;
	bs.initial_bullet_cooldown = 0;

	registry.bulletSpawners.insert(entity, bs);
	registry.colors.insert(entity, { 1,1,1 });

	return entity;
}

Entity createFocusDot(RenderSystem* renderer, vec2 pos, vec2 size)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = size;

	registry.focusdots.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::FOCUS_DOT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createAimbotCursor(RenderSystem* renderer, vec2 pos, float scale)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = vec2(ENEMY_BB_WIDTH_128, ENEMY_BB_HEIGHT_128);

	EntityAnimation ani;
	ani.isCursor = false;
	ani.spritesheet_scale = { 1.f / 4.f, 1.f };
	ani.render_pos = { 1.f / 4.f, 1.f };
	ani.frame_rate_ms = 200;
	ani.full_rate_ms = 200;
	ani.is_active = true;
	registry.alwaysplayAni.insert(entity, ani);

	registry.aimbotCursors.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::AIMBOT_CURSOR,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createCombo(RenderSystem* renderer)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2(window_px_half.x, -window_px_half.y) - vec2(150, -150);
	motion.scale = vec2(160, 160);

	registry.UIUX.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::C, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::COMBO,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createWin(RenderSystem* renderer) {
	// Pause menu background
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2(0, 0);
	motion.scale = vec2(1000, 700);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::WINDEATH_SCREEN,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.winMenus.emplace(entity);
	registry.winMenus.emplace(createText(vec2(0, -200), vec2(2, 2), "You WIN !!!", vec3(0, 0, 0), true, false));

	return entity;

}

void createStats(RenderSystem* renderer, std::chrono::steady_clock::time_point start_time) {
	auto end = Clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start_time);

	int hours = duration.count() / 3600;
	int minutes = (duration.count() % 3600) / 60;
	int seconds = duration.count() % 60;

	stats.time_taken_to_win = std::to_string(hours) + " hrs, " + std::to_string(minutes) + " mins, " + std::to_string(seconds) + " secs";
	registry.winMenus.emplace(createText(vec2(0, -100), vec2(1, 1), "Enemies hit: " + std::to_string(stats.enemies_hit), vec3(0, 0, 0), true, false));
	registry.winMenus.emplace(createText(vec2(0, -50), vec2(1, 1), "Enemies killed: " + std::to_string(stats.enemies_killed), vec3(0, 0, 0), true, false));
	registry.winMenus.emplace(createText(vec2(0, 0), vec2(1, 1), "Bullets fired: " + std::to_string(stats.bullets_fired), vec3(0, 0, 0), true, false));
	stats.accuracy = stats.enemies_hit / (float)stats.bullets_fired;
	std::string accuracy = std::to_string(stats.accuracy);
	registry.winMenus.emplace(createText(vec2(0, 50), vec2(1, 1), "Accuracy: " + accuracy.substr(0, accuracy.find(".") + 3), vec3(0, 0, 0), true, false));
	registry.winMenus.emplace(createText(vec2(0, 100), vec2(1, 1), "Time spent in total: " + stats.time_taken_to_win, vec3(0, 0, 0), true, false));
}

Entity createInfographic(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2(0, 0);
	motion.scale = vec2(1100, 690);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::WINDEATH_SCREEN,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.infographicsMenus.emplace(entity);

	//Picture
	auto entity2 = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh2 = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity2, &mesh2);

	Motion& motion2 = registry.motions.emplace(entity2);
	motion2.position = vec2(0, -30);
	motion2.scale = vec2(887, 417);

	registry.renderRequests.insert(
		entity2,
		{ TEXTURE_ASSET_ID::INFOGRAPHIC,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.infographicsMenus.emplace(entity2);

	return entity;
}

Entity createOptions(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2(0, 0);
	motion.scale = vec2(1100, 690);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::WINDEATH_SCREEN,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.optionMenus.emplace(entity);

	return entity;
}

Entity createLose(RenderSystem* renderer) {
	// Pause menu background
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2(0, 0);
	motion.scale = vec2(1000, 700);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::WINDEATH_SCREEN,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.loseMenus.emplace(entity);
	auto entity_reimu = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh_reimu = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity_reimu, &mesh_reimu);

	Motion& motion_reimu = registry.motions.emplace(entity_reimu);
	motion_reimu.position = vec2(0, 0);
	motion_reimu.scale = vec2(256, 256);

	registry.renderRequests.insert(
		entity_reimu,
		{ TEXTURE_ASSET_ID::REIMUCRY,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.loseMenus.emplace(entity_reimu);
	registry.loseMenus.emplace(createText(vec2(0, -200), vec2(1.5, 1.5), "Game Over!!!", vec3(0, 0, 0), true, false));

	return entity;

}

void createDialogue(CHARACTER character, std::string sentence, CHARACTER talk_2, EMOTION emotion) {
	auto reimu_entity = Entity();

	// Setting initial motion values
	Motion& motion_reimu = registry.motions.emplace(reimu_entity);
	motion_reimu.position = vec2(-250, 0);
	motion_reimu.angle = 0.f;
	motion_reimu.scale = vec2({ 550.f, 600.f });
	EntityAnimation& ani_reimu = registry.alwaysplayAni.emplace(reimu_entity);
	ani_reimu.spritesheet_scale = { 1 / 6.f, 1 };
	registry.dialogueMenus.emplace(reimu_entity);
	if (character == CHARACTER::REIMU) {
		ani_reimu.render_pos = { 1 / 6.f * (1 + (int)emotion), 1 };
		registry.renderRequests.insert(
			reimu_entity,
			{ TEXTURE_ASSET_ID::REIMU_PORTRAIT, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::UI,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		ani_reimu.render_pos = { 1 / 6.f * (1 + (int)EMOTION::NORMAL), 1 };
		registry.renderRequests.insert(
			reimu_entity,
			{ TEXTURE_ASSET_ID::REIMU_PORTRAIT, // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::GREY,
				GEOMETRY_BUFFER_ID::SPRITE });
	}

	if (talk_2 != CHARACTER::NONE) {
		auto other_entity = Entity();

		// Setting initial motion values
		Motion& motion_other = registry.motions.emplace(other_entity);
		motion_other.position = vec2(250, 0);
		motion_other.angle = 0.f;
		motion_other.scale = vec2({ 550.f, 600.f });
		if (talk_2 == CHARACTER::REMILIA) motion_other.scale = vec2({ 800.f, 600.f });
		EntityAnimation& ani_other = registry.alwaysplayAni.emplace(other_entity);
		ani_other.spritesheet_scale = { 1 / 6.f, 1 };

		registry.dialogueMenus.emplace(other_entity);
		if (character == talk_2) {
			ani_other.render_pos = { 1 / 6.f * (1 + (int)emotion), 1 };
			registry.renderRequests.insert(
				other_entity,
				{ static_cast<TEXTURE_ASSET_ID>((int)TEXTURE_ASSET_ID::REIMU_PORTRAIT + (int)talk_2), // TEXTURE_COUNT indicates that no txture is needed
					EFFECT_ASSET_ID::UI,
					GEOMETRY_BUFFER_ID::SPRITE });
		}
		else {
			ani_other.render_pos = { 1 / 6.f * (1 + (int)EMOTION::NORMAL), 1 };
			registry.renderRequests.insert(
				other_entity,
				{ static_cast<TEXTURE_ASSET_ID>((int)TEXTURE_ASSET_ID::REIMU_PORTRAIT + (int)talk_2), // TEXTURE_COUNT indicates that no txture is needed
					EFFECT_ASSET_ID::GREY,
					GEOMETRY_BUFFER_ID::SPRITE });
		}
	}

	auto dialogue_entity = Entity();
	// Setting initial motion values
	Motion& motion_dialogue = registry.motions.emplace(dialogue_entity);
	motion_dialogue.position = vec2(0, window_px_half.y - 130);
	motion_dialogue.angle = 0.f;
	motion_dialogue.scale = vec2({ 1.6 * 688.f, 1.2 * 224.f });

	registry.dialogueMenus.emplace(dialogue_entity);
	registry.renderRequests.insert(
		dialogue_entity,
		{ TEXTURE_ASSET_ID::DIALOGUE_BOX, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });

	createText({ 0,window_px_half.y - 170 }, { 0.8,0.8 }, sentence, vec3(0, 0, 0), false, false);
}

Entity createKey(vec2 pos, vec2 size, KEYS key, bool is_on_ui, bool is_active, float frame_rate)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = size;

	EntityAnimation key_ani;
	key_ani.isCursor = false;
	//std::cout << static_cast<int>(key) << std::endl;
	key_ani.spritesheet_scale = { 1.f / 2.f, 1.f / 18.0f };
	key_ani.render_pos = { 1.f / 2.f, 1.f / 18.0f * static_cast<int>(key) };
	key_ani.frame_rate_ms = frame_rate;
	key_ani.full_rate_ms = frame_rate;
	key_ani.is_active = is_active;
	registry.alwaysplayAni.insert(entity, key_ani);
	if (is_on_ui) registry.UIUX.emplace(entity); else registry.UIUXWorld.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::KEYS, // TEXTURE_COUNT indicates that no txture is needed
			is_on_ui ? EFFECT_ASSET_ID::UI : EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createVFX(RenderSystem* renderer, vec2 pos, vec2 scale, float angle, VFX_TYPE type) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = angle;
	motion.scale = scale;

	EntityAnimation playonce_ani;
	// Set explosion as default, could change in switch statement below
	TEXTURE_ASSET_ID texture_asset = TEXTURE_ASSET_ID::AOE_AMMO_EXPLOSION;

	switch (type) {
	case VFX_TYPE::AOE_AMMO_EXPLOSION: {
		texture_asset = TEXTURE_ASSET_ID::AOE_AMMO_EXPLOSION;
		playonce_ani.spritesheet_scale = { 1.f / 82.f, 1.f };
		playonce_ani.render_pos = { 1 / 82.f, 1.f };
		playonce_ani.frame_rate_ms = 1000.f / 60.f;
		playonce_ani.full_rate_ms = 1000.f / 60.f;
		playonce_ani.is_active = true;
		break;
	}
	case VFX_TYPE::AOE_AMMO_DISAPPEAR: {
		texture_asset = TEXTURE_ASSET_ID::AOE_AMMO_BULLET_DISAPPEAR;
		playonce_ani.spritesheet_scale = { 1.f / 4.f, 1.f };
		playonce_ani.render_pos = { 1 / 4.f, 1.f };
		playonce_ani.frame_rate_ms = 1000.f / 10.f;
		playonce_ani.full_rate_ms = 1000.f / 10.f;
		playonce_ani.is_active = true;
		break;
	}
	case VFX_TYPE::AIMBOT_AMMO_DISAPPEAR: {
		texture_asset = TEXTURE_ASSET_ID::AIMBOT_AMMO_BULLET_DISAPPEAR;
		playonce_ani.spritesheet_scale = { 1.f / 4.f, 1.f };
		playonce_ani.render_pos = { 1 / 4.f, 1.f };
		playonce_ani.frame_rate_ms = 1000.f / 10.f;
		playonce_ani.full_rate_ms = 1000.f / 10.f;
		playonce_ani.is_active = true;
		break;
	}
	case VFX_TYPE::HIT_SPARK: {
		texture_asset = TEXTURE_ASSET_ID::HIT_SPARK;
		playonce_ani.spritesheet_scale = { 1.f / 7.f, 1.f };
		playonce_ani.render_pos = { 1 / 7.f, 1.f };
		playonce_ani.frame_rate_ms = 1000.f / 10.f;
		playonce_ani.full_rate_ms = 1000.f / 10.f;
		playonce_ani.is_active = true;
		break;
	}
	default:
		break;
	}

	registry.playonceAni.insert(entity, playonce_ani);
	registry.renderRequests.insert(
		entity,
		{ texture_asset,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createCriHit(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = M_PI;
	motion.scale = { 128 * 0.4, 128 * 0.4 };

	Kinematic& kin = registry.kinematics.emplace(entity);
	kin.velocity = { 0, -300 };
	kin.direction = { 0,-1 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CRTHITICON, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createBossHealthBarUI(RenderSystem* renderer, Entity boss, std::string boss_name, vec3 name_color) {
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	vec2 padding = { 0, -60 };
	motion.position = vec2(0, window_px_half.y) + padding;
	motion.scale = vec2({ BOSS_HEALTH_BAR_WIDTH, BOSS_HEALTH_BAR_HEIGHT });

	BossHealthBarUI& hb = registry.bossHealthBarUIs.emplace(entity);
	hb.is_visible = false;
	hb.name_color = name_color;
	hb.boss_name = boss_name;
	registry.bossHealthBarLink.emplace(entity, boss);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BOSS_HEALTH_BAR,
			EFFECT_ASSET_ID::BOSSHEALTHBAR,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createHealthUI(RenderSystem* renderer)
{
	auto entity_head = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh_head = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity_head, &mesh_head);

	// Setting initial motion values
	Motion& motion_head = registry.motions.emplace(entity_head);
	motion_head.position = vec2(0, 0) - window_px_half + vec2(70, 70);
	motion_head.scale = vec2({ 128 * 1.3, 128 * 1.3 });
	registry.UIUX.emplace(entity_head);
	registry.renderRequests.insert(
		entity_head,
		{ TEXTURE_ASSET_ID::REIMU_HEAD, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.colors.insert(entity_head, { 1,1,1 });


	auto entity_inv = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh_inv = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity_inv, &mesh_inv);

	// Setting initial motion values
	Motion& motion_inv = registry.motions.emplace(entity_inv);
	motion_inv.position = vec2(0, 0) - window_px_half + vec2(128 * 1.3 + 110 + 20, 105);
	motion_inv.scale = vec2({ VP_BB_WIDTH, VP_BB_HEIGHT });
	registry.UIUX.emplace(entity_inv);
	registry.renderRequests.insert(
		entity_inv,
		{ TEXTURE_ASSET_ID::FOCUS_BAR, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::PLAYER_HB,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.colors.insert(entity_inv, { 1,1,1 });

	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2(0, 0) - window_px_half + vec2(128 * 1.3 + 70 + 20, 70);
	motion.scale = vec2({ HP_BB_WIDTH, HP_BB_HEIGHT });
	registry.UIUX.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::REIMU_HEALTH, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::PLAYER_HB,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.colors.insert(entity, { 1,1,1 });
	return entity;
}

std::vector<Entity> createAttributeUI(RenderSystem* renderer)
{

	std::vector<Entity> entity_array;
	auto entity_coin = Entity();
	Mesh& mesh_coin = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity_coin, &mesh_coin);
	Motion& motion_coin = registry.motions.emplace(entity_coin);
	motion_coin.position = vec2(0, 0) - window_px_half + vec2(30, 200 - 50);
	motion_coin.scale = vec2({ 128 * 0.2, 128 * 0.2 });
	registry.UIUX.emplace(entity_coin);
	registry.renderRequests.insert(
		entity_coin,
		{ TEXTURE_ASSET_ID::COIN_STATIC, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.colors.insert(entity_coin, { 1,1,1 });
	entity_array.push_back(entity_coin);


	for (int i = 0; i < 5; i++) {
		auto entity = Entity();

		// Store a reference to the potentially re-used mesh object
		Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(entity, &mesh);

		// Setting initial motion values
		Motion& motion = registry.motions.emplace(entity);
		motion.position = vec2(0, 0) - window_px_half + vec2(30, 200 + 50 * i);
		motion.scale = vec2({ 128 * 0.2, 128 * 0.2 });
		registry.UIUX.emplace(entity);
		registry.renderRequests.insert(
			entity,
			{ static_cast<TEXTURE_ASSET_ID>((int)TEXTURE_ASSET_ID::ATTACKDMG + i), // TEXTURE_COUNT indicates that no txture is needed
				EFFECT_ASSET_ID::UI,
				GEOMETRY_BUFFER_ID::SPRITE });
		registry.colors.insert(entity, { 1,1,1 });
		entity_array.push_back(entity);
	}
	return entity_array;
}

Entity createInvisible(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	registry.kinematics.emplace(entity);
	return entity;
}

Entity createDummyEnemySpawner(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	registry.dummyenemyspawners.emplace(entity);
	return entity;
}

Entity createDummyEnemy(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ ENEMY_BB_WIDTH * 2.f, ENEMY_BB_HEIGHT * 2.f });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.f;

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = 100;
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;

	registry.dummyenemies.emplace(entity);
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 1.f / 6.f, 1.f / 12.f };
	enemy_ani.render_pos = { 1.f / 6.f, 5.f / 12.f };
	registry.animation.insert(entity, enemy_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_WOLF,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.colors.insert(entity, { 1,1,1 });

	return entity;
}

Entity createRoomSignifier(RenderSystem* renderer, vec2 position, ROOM_TYPE room_type) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ world_tile_size * 3 / 4, world_tile_size * 3 / 4 });
	registry.renderRequests.insert(
		entity,
		{ static_cast<TEXTURE_ASSET_ID>((int)TEXTURE_ASSET_ID::NORMAL_SIGN + (int)room_type),
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.roomSignifiers.emplace(entity);
	registry.colors.insert(entity, { 1,1,1 });

	return entity;
}

Entity createBoss(RenderSystem* renderer, vec2 position, std::string boss_name, BOSS_ID boss_id, vec3 name_color)
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
	motion.scale = vec2({ ENEMY_BB_HEIGHT_128 / 1.8f, ENEMY_BB_HEIGHT_128 / 1.8f });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = abs(vec2(motion.scale.x / 1.4f, motion.scale.y / 1.2f));

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;

	// Animation
	EntityAnimation& enemy_ani = registry.animation.emplace(entity);
	enemy_ani.spritesheet_scale = { 1.f / 4.f, 1.f / 4.f };
	enemy_ani.render_pos = { 1.f / 4.f, 1.f / 4.f };

	// HP
	HP& hp = registry.hps.emplace(entity);

	// Boss
	Boss& boss = registry.bosses.emplace(entity);
	boss.boss_id = boss_id;
	boss.duration = 10000; // duration for each pattern
	boss.phase_change_time = 1500;

	if (boss_id == BOSS_ID::CIRNO) {
		hp.max_hp = 5020;
		hp.curr_hp = hp.max_hp;

		//boss.health_phase_thresholds = { 5, 4, 3, 2, -1 }; // -1 for end of phase
		boss.health_phase_thresholds = { 5000, 3750, 2500, 1250, -1 }; // -1 for end of phase

		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BOSS_CIRNO,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (boss_id == BOSS_ID::FLANDRE) {
		hp.max_hp = 10020;
		hp.curr_hp = hp.max_hp;

		//boss.health_phase_thresholds = { 5, 4, 3, 2, -1 }; // -1 for end of phase
		boss.health_phase_thresholds = { 10000, 7500, 5000, 2500, -1 }; // -1 for end of phase

		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BOSS_FLANDRE,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (boss_id == BOSS_ID::SAKUYA) {
		hp.max_hp = 20020;
		//hp.max_hp = 120;
		hp.curr_hp = hp.max_hp;

		//boss.health_phase_thresholds = { 100, 90, 80, 70, -1 }; // -1 for end of phase
		boss.health_phase_thresholds = { 20000, 15000, 10000, 5000, -1 }; // -1 for end of phase

		enemy_ani.spritesheet_scale = { 1.f / 4.f, 1.f / 8.f };
		enemy_ani.render_pos = { 1.f / 4.f, 1.f / 8.f };

		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BOSS_SAKUYA,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (boss_id == BOSS_ID::REMILIA) {
		hp.max_hp = 40020;
		//hp.max_hp = 120;
		hp.curr_hp = hp.max_hp;

		//boss.health_phase_thresholds = { 100, 90, 80, 70, -1 }; // -1 for end of phase
		boss.health_phase_thresholds = { 40000, 30000, 20000, 10000, -1 }; // -1 for end of phase

		enemy_ani.spritesheet_scale = { 1.f / 4.f, 1.f / 8.f };
		enemy_ani.render_pos = { 1.f / 4.f, 1.f / 8.f };

		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BOSS_REMILIA,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}

	Entity invis_entity = createInvisible(renderer, position);
	boss.invis_spawner = invis_entity;
	registry.bossInvisibles.emplace(invis_entity, entity);

	registry.colors.insert(entity, { 1,1,1 });
	// Boss health bar ui
	Entity ui_entity = createBossHealthBarUI(renderer, entity, boss_name, name_color);
	registry.bossHealthBarLink.emplace(entity, ui_entity);

	// Add invulnerability
	registry.invulnerableTimers.emplace(entity).invulnerable_counter_ms = 3600000;

	// Decision tree ai
	AiTimer& ai_timer = registry.aitimers.emplace(entity);
	ai_timer.update_base = 1000;

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
	hp.max_hp = static_cast<int>(80 * combo_mode.combo_meter);
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
	bs.fire_rate = 14 * 1.f / combo_mode.combo_meter;
	bs.is_firing = false;
	bs.bullet_initial_speed = 200;

	registry.bulletSpawners.insert(entity, bs);
	registry.colors.insert(entity, { 1,1,1 });

	registry.aitimers.emplace(entity);

	return entity;
}

Entity createText(vec2 pos, vec2 scale, std::string text_content, vec3 color, bool is_perm, bool in_world) {
	auto entity = Entity();
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.scale = scale; // Only x is used for scaling both x & y
	//registry.kinematics.emplace(entity);
	if (is_perm) {
		if (in_world) {
			registry.textsPermWorld.emplace(entity).content = text_content;
		}
		else {
			registry.textsPerm.emplace(entity).content = text_content;
		}
	}
	else {
		if (in_world) {
			registry.textsWorld.emplace(entity).content = text_content;
		}
		else {
			registry.texts.emplace(entity).content = text_content;
		}
	}
	registry.colors.emplace(entity) = color;
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
	kinematic.speed_base = 240.f * combo_mode.combo_meter;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.f;

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = static_cast<int>(20 * combo_mode.combo_meter);
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = static_cast<int>(1 * combo_mode.combo_meter);

	registry.bomberEnemies.emplace(entity);
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 1.f / 6.f, 1.f / 12.f };
	enemy_ani.render_pos = { 1.f / 6.f, 1.f / 12.f };
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
	hp.max_hp = static_cast<int>(40 * combo_mode.combo_meter);
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
	bs.fire_rate = 40 * 1 / combo_mode.combo_meter;
	bs.is_firing = false;
	bs.bullet_initial_speed = 160;
	bs.bullets_per_array = static_cast<int>(3 * combo_mode.combo_meter);
	bs.spread_within_array = 30;

	registry.bulletSpawners.insert(entity, bs);
	registry.colors.insert(entity, { 1,1,1 });

	AiTimer& aitimer = registry.aitimers.emplace(entity);
	aitimer.update_base = 500; // updates decision tree every second
	aitimer.update_timer_ms = 500;

	return entity;
}

Entity createLizardEnemy(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ ENEMY_BB_WIDTH_96, ENEMY_BB_HEIGHT_96 });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.f;

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = static_cast<int>(50 * combo_mode.combo_meter);
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;


	registry.lizardEnemies.emplace(entity);
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 1.f / 6.f, 1.f / 12.f };
	enemy_ani.render_pos = { 1.f / 6.f, 5.f / 12.f };
	registry.animation.insert(entity, enemy_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_LIZARD,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.idleMoveActions.emplace(entity);

	BulletSpawner bs;
	//bs.fire_rate = 2;
	//bs.is_firing = false;
	//bs.bullet_initial_speed = 100;
	//bs.number_to_fire = 5;
	//bs.cooldown_rate = 50; // every 5 seconds
	//bs.is_cooldown = true;

	bs.fire_rate = 2 * 1 / combo_mode.combo_meter;
	bs.is_firing = false;
	bs.bullets_per_array = 6;
	bs.spread_within_array = static_cast<int>(60 * combo_mode.combo_meter);
	bs.bullet_initial_speed = 100;
	bs.number_to_fire = 1;
	bs.is_cooldown = true;
	bs.cooldown_rate = 50; // every 5 seconds

	registry.bulletSpawners.insert(entity, bs);

	deadly.has_bullet_pattern = true;
	deadly.bullet_pattern.commands = {
		{ BULLET_ACTION::DELAY, 500.f },
		{ BULLET_ACTION::DIRECTION, vec2(0,0)},
		{ BULLET_ACTION::DELAY, 1000.f },
		{ BULLET_ACTION::PLAYER_DIRECTION, 0 },
	};

	registry.colors.insert(entity, { 1,1,1 });

	AiTimer& aitimer = registry.aitimers.emplace(entity);
	aitimer.update_base = 500; // updates decision tree every second
	aitimer.update_timer_ms = 500;

	return entity;
}

Entity createWormEnemy(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ ENEMY_BB_WIDTH_128, ENEMY_BB_HEIGHT_128 });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.f;

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = static_cast<int>(50 * combo_mode.combo_meter);
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;

	registry.wormEnemies.emplace(entity);
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 1.f / 6.f, 1.f / 12.f };
	enemy_ani.render_pos = { 1.f / 6.f, 5.f / 12.f };
	registry.animation.insert(entity, enemy_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_WORM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.idleMoveActions.emplace(entity);

	BulletSpawner bs;
	bs.fire_rate = 1.5 * 1 / combo_mode.combo_meter;
	bs.is_firing = false;
	bs.bullet_initial_speed = static_cast<int>(100 * combo_mode.combo_meter);
	bs.number_to_fire = 14;
	bs.is_cooldown = true;
	bs.cooldown_rate = 60;

	registry.bulletSpawners.insert(entity, bs);
	registry.colors.insert(entity, { 1,1,1 });

	AiTimer& aitimer = registry.aitimers.emplace(entity);
	aitimer.update_base = 500; // updates decision tree every second
	aitimer.update_timer_ms = 500;

	return entity;
}

Entity createBee2Enemy(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = 1.3f * vec2({ ENEMY_BB_WIDTH_96, ENEMY_BB_HEIGHT_96 });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.f;

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = 50;
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;

	registry.bee2Enemies.emplace(entity);
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 1.f / 6.f, 1.f / 8.f };
	enemy_ani.render_pos = { 1.f / 6.f, 1.f / 8.f };
	registry.animation.insert(entity, enemy_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_BEE2,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.idleMoveActions.emplace(entity);

	BulletSpawner bs;
	bs.fire_rate = 20;
	bs.is_firing = false;
	bs.bullet_initial_speed = 100;

	deadly.has_bullet_pattern = true;
	deadly.bullet_pattern.commands = {
		{ BULLET_ACTION::ROTATE, 45},
		{ BULLET_ACTION::DELAY, 200},
		{ BULLET_ACTION::ROTATE, -90},
		{ BULLET_ACTION::DELAY, 200},
		{ BULLET_ACTION::ROTATE, 90},
		{ BULLET_ACTION::LOOP, vec2(10, 1)},
	};

	registry.bulletSpawners.insert(entity, bs);
	registry.colors.insert(entity, { 1,1,1 });

	AiTimer& aitimer = registry.aitimers.emplace(entity);
	aitimer.update_base = 500; // updates decision tree every second
	aitimer.update_timer_ms = 500;

	return entity;
}

Entity createGargoyleEnemy(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = 1.8f * vec2({ ENEMY_BB_WIDTH_96, ENEMY_BB_HEIGHT_96 });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 3.f;

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = static_cast<int>(60 * combo_mode.combo_meter);
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;

	registry.gargoyleEnemies.emplace(entity);
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 1.f / 6.f, 1.f / 8.f };
	enemy_ani.render_pos = { 1.f / 6.f, 1.f / 8.f };
	registry.animation.insert(entity, enemy_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_GARGOYLE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.idleMoveActions.emplace(entity);

	BulletSpawner bs;
	bs.fire_rate = 50 * 1 / combo_mode.combo_meter;
	bs.is_firing = false;
	bs.bullet_initial_speed = 100;

	registry.bulletSpawners.insert(entity, bs);
	registry.colors.insert(entity, { 1,1,1 });

	AiTimer& aitimer = registry.aitimers.emplace(entity);
	aitimer.update_base = 500; // updates decision tree every second
	aitimer.update_timer_ms = 500;

	return entity;
}

Entity createTurtleEnemy(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = 0.8f * vec2({ ENEMY_BB_WIDTH_128, ENEMY_BB_HEIGHT_128 });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 80.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.2f;

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = static_cast<int> (60 * combo_mode.combo_meter);
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;

	registry.turtleEnemies.emplace(entity);
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 1.f / 6.f, 1.f / 12.f };
	enemy_ani.render_pos = { 1.f / 6.f, 5.f / 12.f };
	registry.animation.insert(entity, enemy_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_TURTLE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.idleMoveActions.emplace(entity);

	BulletSpawner bs;
	bs.fire_rate = 2 * 1 / combo_mode.combo_meter;
	bs.is_firing = false;
	bs.bullets_per_array = static_cast<int>(12 * combo_mode.combo_meter);
	bs.spread_within_array = 30;
	bs.bullet_initial_speed = 10;
	bs.number_to_fire = 1;
	bs.is_cooldown = true;
	bs.cooldown_rate = 50; // every 5 seconds

	registry.bulletSpawners.insert(entity, bs);

	registry.colors.insert(entity, { 1,1,1 });

	AiTimer& aitimer = registry.aitimers.emplace(entity);
	aitimer.update_base = 500; // updates decision tree every second
	aitimer.update_timer_ms = 500;

	return entity;
}

Entity createSkeletonEnemy(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = 0.7f * vec2({ ENEMY_BB_WIDTH_128, ENEMY_BB_HEIGHT_128 });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 100.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 2.f;

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = static_cast<int>(60 * combo_mode.combo_meter);
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;

	registry.skeletonEnemies.emplace(entity);
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 1.f / 6.f, 1.f / 12.f };
	enemy_ani.render_pos = { 1.f / 6.f, 5.f / 12.f };
	registry.animation.insert(entity, enemy_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_SKELETON,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.idleMoveActions.emplace(entity);

	BulletSpawner bs;
	bs.fire_rate = 60 * 1 / combo_mode.combo_meter;
	bs.is_firing = false;
	bs.bullet_initial_speed = 400;

	registry.bulletSpawners.insert(entity, bs);
	registry.colors.insert(entity, { 1,1,1 });

	AiTimer& aitimer = registry.aitimers.emplace(entity);
	aitimer.update_base = 500; // updates decision tree every second
	aitimer.update_timer_ms = 500;

	return entity;
}

Entity createSeagullEnemy(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;
	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ ENEMY_BB_WIDTH_128, ENEMY_BB_HEIGHT_128 });

	auto& kinematic = registry.kinematics.emplace(entity);
	kinematic.speed_base = 200.f;
	kinematic.speed_modified = 1.f * kinematic.speed_base;
	kinematic.direction = { 0, 0 };

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = motion.scale / 4.f;

	// HP
	HP& hp = registry.hps.emplace(entity);
	hp.max_hp = static_cast<int> (60 * combo_mode.combo_meter);
	hp.curr_hp = hp.max_hp;

	// Collision damage
	Deadly& deadly = registry.deadlys.emplace(entity);
	deadly.damage = 1;

	registry.seagullEnemies.emplace(entity);
	EntityAnimation enemy_ani;
	enemy_ani.spritesheet_scale = { 1.f / 6.f, 1.f / 8.f };
	enemy_ani.render_pos = { 1.f / 6.f, 1.f / 8.f };
	registry.animation.insert(entity, enemy_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ENEMY_SEAGULL,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	registry.idleMoveActions.emplace(entity);

	BulletSpawner bs;
	bs.fire_rate = 5 * 1 / combo_mode.combo_meter;
	bs.is_firing = false;
	bs.bullet_initial_speed = -kinematic.speed_base + 10;

	deadly.has_bullet_pattern = true;
	deadly.bullet_pattern.commands = {
		{ BULLET_ACTION::ROTATE, 5.f },
		{ BULLET_ACTION::DELAY, 30.f },
		{ BULLET_ACTION::LOOP, vec2(80, 0)},
		{ BULLET_ACTION::DEL, -1.f },
	};

	registry.bulletSpawners.insert(entity, bs);
	registry.colors.insert(entity, { 1,1,1 });

	AiTimer& aitimer = registry.aitimers.emplace(entity);
	aitimer.update_base = 500; // updates decision tree every second
	aitimer.update_timer_ms = 500;

	return entity;
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
		collidable.size = { motion.scale.x, motion.scale.y };
		collidable.shift = { 0, 0 };

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

Entity createObstacle(RenderSystem* renderer, vec2 grid_position) {
	auto entity = Entity();

	// Initializes the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = convert_grid_to_world(grid_position);
	motion.scale = vec2(world_tile_size, world_tile_size);

	// Rocks are collidable
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = { motion.scale.x, motion.scale.y };
	collidable.shift = { 0, 0 };

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distrib(0, 1);
	double number = distrib(gen);

	// Placeholder texure
	if (map_info.level == MAP_LEVEL::LEVEL1) {
		if (number < 0.1) {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			collidable.size = vec2(motion.scale.x, motion.scale.y) / 1.5f;
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::CRATES_SMALL,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (number < 0.2) {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::CRATES,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (number < 0.4) {
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::SKELETON,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (number < 0.6) {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::POTTERY,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (number < 0.8) {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::BARREL,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			EntityAnimation& animation = registry.alwaysplayAni.emplace(entity);
			animation.spritesheet_scale = { 1 / 8.f, 1.0f };
			animation.render_pos = { 1 / 8.f, 1.0f };
			animation.full_rate_ms = 100.f;
			animation.frame_rate_ms = 100.f;
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::FIREPLACE,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
	}
	else if (map_info.level == MAP_LEVEL::LEVEL2) {
		if (number < 0.6) {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::ROCK,
					EFFECT_ASSET_ID::TEXTURED,
					GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (number < 0.8) {
			registry.remove_all_components_of(entity);
			createRuinsPillar(renderer, grid_position, false);
		}
		else {
			registry.remove_all_components_of(entity);
			if (world_map[grid_position.y][grid_position.x + 1] == (int)TILE_TYPE::WALL) {
				createRuinsPillar(renderer, grid_position, false);
			}
			else {
				createRuinsPillar(renderer, grid_position, true);
			}
		}
	}
	else if (map_info.level == MAP_LEVEL::LEVEL3) {
		collidable.size = vec2(motion.scale.x, motion.scale.y) / 1.2f;
		if (number < 0.2) {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::ROCK_MOSS_0,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (number < 0.4) {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::ROCK_MOSS_1,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (number < 0.6) {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::ROCK_MOSS_2,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (number < 0.8) {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::LOG,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::LOG_MUSHROOM,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
	}
	else if (map_info.level == MAP_LEVEL::LEVEL4) {
		if (number < 0.2) {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::CYLINDER_1,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (number < 0.4) {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::CYLINDER_2,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (number < 0.6) {
			world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{
				TEXTURE_ASSET_ID::TOMBSTONE_BROKEN,
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else if (number < 0.8) {
			registry.remove_all_components_of(entity);
			entity = createPillar(renderer, grid_position, true);
		}
		else if (number < 0.95) {
			registry.remove_all_components_of(entity);
			entity = createPillar(renderer, grid_position, false);
		}
		else {
			registry.remove_all_components_of(entity);
			entity = createSkyTree(renderer, grid_position);
		}
	}
	return entity;
}

Entity createRuinsPillar(RenderSystem* renderer, vec2 grid_position, bool is_horizontal) {
	auto entity = Entity();
	world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;

	if (is_horizontal) {
		// horizontal ruins pillar
		auto right_entity = Entity();

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(entity, &mesh);
		Mesh& right_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(right_entity, &right_mesh);

		auto& left_motion = registry.motions.emplace(entity);
		left_motion.position = convert_grid_to_world(grid_position);
		left_motion.scale = vec2(world_tile_size, world_tile_size);
		auto& right_motion = registry.motions.emplace(right_entity);
		right_motion.position = convert_grid_to_world(grid_position + vec2(1, 0));
		right_motion.scale = vec2(world_tile_size, world_tile_size);

		// Set the collision box
		auto& left_collidable = registry.collidables.emplace(entity);
		left_collidable.size = { left_motion.scale.x, left_motion.scale.y };
		left_collidable.shift = { 0, 0 };
		registry.walls.emplace(entity);

		// Set the collision box
		auto& right_collidable = registry.collidables.emplace(right_entity);
		right_collidable.size = { right_motion.scale.x, right_motion.scale.y };
		right_collidable.shift = { 0, 0 };
		registry.walls.emplace(right_entity);

		world_map[grid_position.y][grid_position.x + 1] = (int)TILE_TYPE::WALL;

		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::RUINS_PILLAR_LEFT,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
		registry.renderRequests.insert(
			right_entity,
			{ TEXTURE_ASSET_ID::RUINS_PILLAR_RIGHT,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		// vertical ruins pillar
		auto top_entity = Entity();

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(entity, &mesh);
		Mesh& top_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(top_entity, &top_mesh);

		auto& bottom_motion = registry.motions.emplace(entity);
		bottom_motion.position = convert_grid_to_world(grid_position);
		bottom_motion.scale = vec2(world_tile_size, world_tile_size);
		auto& top_motion = registry.motions.emplace(top_entity);
		top_motion.position = convert_grid_to_world(grid_position + vec2(0, -1));
		top_motion.scale = vec2(world_tile_size, world_tile_size);

		// Set the collision box
		auto& bottom_collidable = registry.collidables.emplace(entity);
		bottom_collidable.size = { bottom_motion.scale.x, bottom_motion.scale.y };
		bottom_collidable.shift = { 0, 0 };

		registry.walls.emplace(entity);
		registry.floors.emplace(top_entity);

		// ordered from bottom to top
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::RUINS_PILLAR_BOTTOM,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
		registry.renderRequestsForeground.insert(
			top_entity,
			{ TEXTURE_ASSET_ID::RUINS_PILLAR_TOP,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}

	return entity;
}

// IMPORTANT: createDoor takes in grid coordinates
Entity createDoor(RenderSystem* renderer, vec2 grid_position, DIRECTION dir, int room_index) {
	auto entity = Entity();

	// Initializes the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = convert_grid_to_world(grid_position);
	motion.scale = vec2(world_tile_size, world_tile_size);

	// Creates door
	auto& door = registry.doors.emplace(entity);
	door.dir = dir;
	door.room_index = room_index;

	game_info.room_index[room_index].doors.push_back(entity);
	game_info.room_index[room_index].door_locations.push_back(grid_position);

	// Locked doors are collidable
	auto& collidable = registry.collidables.emplace(entity);
	collidable.size = { motion.scale.x, motion.scale.y };
	collidable.shift = { 0, 0 };

	if (dir == DIRECTION::LEFT || dir == DIRECTION::RIGHT) {
		door.top_texture = createDoorUpTexture(renderer, grid_position + vec2(0, -1));
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::DOOR_VERTICAL_CLOSE_DOWN,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::DOOR_HORIZONTAL_CLOSE,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}

	return entity;
}

// for vertical doors, aesthetic effect
Entity createDoorUpTexture(RenderSystem* renderer, vec2 grid_position) {
	auto entity = Entity();

	// Initializes the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = convert_grid_to_world(grid_position);
	motion.scale = vec2(world_tile_size, world_tile_size);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::DOOR_VERTICAL_CLOSE_UP,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createTile(RenderSystem* renderer, VisibilitySystem* visibility_system, vec2 grid_position, TILE_NAME tile_name, bool is_wall, bool is_placebo_wall) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	//Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	//registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = convert_grid_to_world(grid_position);
	motion.scale = vec2(world_tile_size, world_tile_size);

	// Create wall or floor entity for physics collision
	if (is_wall) {
		registry.walls.emplace(entity);
		// Set the collision box
		auto& collidable = registry.collidables.emplace(entity);
		collidable.size = { motion.scale.x, motion.scale.y };
		collidable.shift = { 0, 0 };
	}
	else if (is_placebo_wall) {
		registry.placeboWalls.emplace(entity);
		// Set the collision box
		auto& collidable = registry.collidables.emplace(entity);
		collidable.size = { motion.scale.x, motion.scale.y };
		collidable.shift = { 0, 0 };
	}
	else {
		registry.floors.emplace(entity);
	}

	// Set up instance data
	Transform t;
	t.translate(motion.position);
	t.scale(motion.scale);
	registry.tileInstanceData.emplace(entity) = {
		renderer->get_spriteloc(tile_name),
		t.mat
	};

	// Add visibility tile
	// We do it here bcause we have already calculated the transform matrix
	/*
	This entity has:

	TEXTURE_ASSET_ID::TEXTURE_COUNT,
	EFFECT_ASSET_ID::EGG,
	GEOMETRY_BUFFER_ID::DEBUG_LINE2
	*/

	// Return early here to exclude adding visibility tiles
	if (visibility_info.excluded.find(map_info.level) != visibility_info.excluded.end()) return entity;

	auto entity2 = Entity();
	registry.visibilityTileInstanceData.emplace(entity2) = {
		t.mat,
		1.0
	};
	// add reference to entity in 2d array
	// when removing visibility tile entities, we set it's corresponding grid position in reference map to -1
	visibility_system->reference_map[grid_position.y][grid_position.x] = entity2;

	return entity;
}

// world_map at grid_position has been set as wall before calling
// returns tile that is a wall
Entity createPillar(RenderSystem* renderer, vec2 grid_position, bool is_broken) {
	world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;

	auto bottom_entity = Entity();
	auto top_entity = Entity();
	if (is_broken) {
		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		Mesh& bottom_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(bottom_entity, &bottom_mesh);
		Mesh& top_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(top_entity, &top_mesh);

		auto& bottom_motion = registry.motions.emplace(bottom_entity);
		bottom_motion.position = convert_grid_to_world(grid_position);
		bottom_motion.scale = vec2(world_tile_size, world_tile_size);
		auto& top_motion = registry.motions.emplace(top_entity);
		top_motion.position = convert_grid_to_world(grid_position + vec2(0, -1));
		top_motion.scale = vec2(world_tile_size, world_tile_size);

		// Set the collision box
		auto& bottom_collidable = registry.collidables.emplace(bottom_entity);
		bottom_collidable.size = { bottom_motion.scale.x, bottom_motion.scale.y };
		bottom_collidable.shift = { 0, 0 };

		registry.walls.emplace(bottom_entity);
		registry.floors.emplace(top_entity);

		// ordered from bottom to top
		registry.renderRequests.insert(
			bottom_entity,
			{ TEXTURE_ASSET_ID::PILLAR_BOTTOM,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
		registry.renderRequestsForeground.insert(
			top_entity,
			{ TEXTURE_ASSET_ID::PILLAR_TOP_BROKEN,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		auto middle_entity = Entity();

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		Mesh& bottom_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(bottom_entity, &bottom_mesh);
		Mesh& middle_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(middle_entity, &middle_mesh);
		Mesh& top_mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(top_entity, &top_mesh);

		auto& bottom_motion = registry.motions.emplace(bottom_entity);
		bottom_motion.position = convert_grid_to_world(grid_position);
		bottom_motion.scale = vec2(world_tile_size, world_tile_size);
		auto& middle_motion = registry.motions.emplace(middle_entity);
		middle_motion.position = convert_grid_to_world(grid_position + vec2(0, -1));
		middle_motion.scale = vec2(world_tile_size, world_tile_size);
		auto& top_motion = registry.motions.emplace(top_entity);
		top_motion.position = convert_grid_to_world(grid_position + vec2(0, -2));
		top_motion.scale = vec2(world_tile_size, world_tile_size);

		// Set the collision box
		auto& bottom_collidable = registry.collidables.emplace(bottom_entity);
		bottom_collidable.size = { bottom_motion.scale.x, bottom_motion.scale.y };
		bottom_collidable.shift = { 0, 0 };

		registry.walls.emplace(bottom_entity);
		registry.floors.emplace(top_entity);
		registry.floors.emplace(middle_entity);

		// ordered from bottom to top
		registry.renderRequests.insert(
			bottom_entity,
			{ TEXTURE_ASSET_ID::PILLAR_BOTTOM,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
		registry.renderRequestsForeground.insert(
			middle_entity,
			{ TEXTURE_ASSET_ID::PILLAR_MIDDLE,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
		registry.renderRequestsForeground.insert(
			top_entity,
			{ TEXTURE_ASSET_ID::PILLAR_TOP,
				EFFECT_ASSET_ID::TEXTURED,
				GEOMETRY_BUFFER_ID::SPRITE });
	}

	return bottom_entity;
}

// world_map at grid_position has been set as wall before calling
// returns tile that is a wall
Entity createSkyTree(RenderSystem* renderer, vec2 grid_position) {
	world_map[grid_position.y][grid_position.x] = (int)TILE_TYPE::WALL;
	Entity wall_entity;
	int start = (int)TEXTURE_ASSET_ID::SKY_TREE_0;

	for (int i = 0; i < 9; ++i) {
		auto entity = Entity();

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(entity, &mesh);

		// Calculate shift
		vec2 shift = { -1 + i % 3, -2 + i / 3 };

		auto& motion = registry.motions.emplace(entity);
		motion.position = convert_grid_to_world(grid_position + shift);
		motion.scale = vec2(world_tile_size, world_tile_size);

		// Set the collision box
		auto& bottom_collidable = registry.collidables.emplace(entity);
		bottom_collidable.size = { motion.scale.x, motion.scale.y };
		bottom_collidable.shift = { 0, 0 };

		if (i == 7) {
			wall_entity = entity;
			registry.walls.emplace(entity);
			registry.renderRequests.insert(
				entity,
				{ static_cast<TEXTURE_ASSET_ID>(start + i),
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
		else {
			registry.floors.emplace(entity);
			registry.renderRequestsForeground.insert(
				entity,
				{ static_cast<TEXTURE_ASSET_ID>(start + i),
				 EFFECT_ASSET_ID::TEXTURED,
				 GEOMETRY_BUFFER_ID::SPRITE });
		}
	}

	return wall_entity;
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

Entity createEgg(vec2 pos, vec2 size)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.scale = size;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::EGG,
			GEOMETRY_BUFFER_ID::EGG });

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createNPC(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.scale = vec2({ PLAYER_BB_WIDTH, PLAYER_BB_HEIGHT });

	// Set the collision box
	auto& collidable = registry.collidables.emplace(entity);
	// Set player collision box at the feet of the player
	collidable.size = { motion.scale.x * 5, motion.scale.y * 5 };
	collidable.shift = { 0, 0 };

	registry.npcs.emplace(entity);
	EntityAnimation npc_ani;
	npc_ani.isCursor = true;
	npc_ani.spritesheet_scale = { 0.25, 1.0 };
	npc_ani.render_pos = { 0.25, 1.0 };
	registry.alwaysplayAni.insert(entity, npc_ani);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::NPC_MARISA, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.colors.insert(entity, { 1,1,1 });

	return entity;
}

Entity createButton(RenderSystem* renderer, vec2 pos, float scale,
	MENU_STATE menu_state, std::string button_text, float text_scale, std::function<void()> func) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = scale * vec2(BUTTON_HOVER_WIDTH, BUTTON_HOVER_HEIGHT);

	registry.colors.emplace(entity, vec3(0.f));

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BUTTON,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });

	Button& button = registry.buttons.emplace(entity);
	button.state = menu_state;
	button.text = button_text;
	button.text_scale = text_scale;
	button.func = func;

	return entity;
}

Entity createMainMenu(RenderSystem* renderer, vec2 title_pos, float title_scale, vec2 background_pos, float background_scale) {
	// Main menu background
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 0, 0 };
	// background will always be largest size and centered
	vec2 background_size = { MENU_BACKGROUND_WIDTH, MENU_BACKGROUND_HEIGHT };
	vec2 ratio = vec2(window_width_px, window_height_px) / background_size;
	motion.scale = (ratio.x + ratio.y) / 2.f * background_size;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::MENU_BACKGROUND,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.mainMenus.emplace(entity);

	// Main menu title 
	auto entity2 = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh2 = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity2, &mesh2);

	Motion& motion2 = registry.motions.emplace(entity2);
	motion2.position = title_pos;
	motion2.scale = title_scale * vec2(MENU_TITLE_WIDTH, MENU_TITLE_HEIGHT);

	registry.renderRequests.insert(
		entity2,
		{ TEXTURE_ASSET_ID::MENU_TITLE,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.mainMenus.emplace(entity2);

	// Paper on buttons for visibility
	auto entity3 = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh3 = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity3, &mesh3);

	Motion& motion3 = registry.motions.emplace(entity3);
	motion3.position = background_pos - vec2(0, 10);
	motion3.scale = background_scale * vec2(PAUSE_BACKGROUND_WIDTH, PAUSE_BACKGROUND_HEIGHT) * vec2(1.f, 1.2f);

	registry.renderRequests.insert(
		entity3,
		{ TEXTURE_ASSET_ID::PAUSE_BACKGROUND,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.mainMenus.emplace(entity3);

	return entity;
}

Entity createPauseMenu(RenderSystem* renderer, vec2 background_pos, float background_scale) {
	// Pause menu background
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	Motion& motion = registry.motions.emplace(entity);
	motion.position = background_pos;
	motion.scale = background_scale * vec2(PAUSE_BACKGROUND_WIDTH, PAUSE_BACKGROUND_HEIGHT);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PAUSE_BACKGROUND,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.pauseMenus.emplace(entity);

	return entity;
}

Entity createTeleporter(RenderSystem* renderer, vec2 pos, vec2 scale, vec2 collidable_size,
	MAP_LEVEL destination, EntityAnimation& ani, TEXTURE_ASSET_ID texture_asset, float teleport_time, std::string teleporter_text, std::string optional_text_above_teleporter) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = scale;

	Collidable& collidable = registry.collidables.emplace(entity);
	collidable.size = collidable_size;
	collidable.shift = { 0, -10 };

	registry.alwaysplayAni.insert(entity, ani);
	registry.renderRequests.insert(
		entity,
		{ texture_asset,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE });
	Teleporter& t = registry.teleporters.emplace(entity);
	t.destination = destination;
	t.teleport_time = teleport_time;
	t.teleporter_text = teleporter_text;
	t.optional_text_above_teleporter = optional_text_above_teleporter;

	return entity;
}

Entity createAura(RenderSystem* renderer, vec2 pos, float scale, Entity entity_to_link, float spritesheet_x_scale, TEXTURE_ASSET_ID texture_asset, float framerate) {
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = scale * vec2(ENEMY_BB_WIDTH_100, ENEMY_BB_HEIGHT_100);

	EntityAnimation ani;
	ani.spritesheet_scale = { spritesheet_x_scale, 1.f }; // 1/61
	ani.render_pos = { spritesheet_x_scale, 1.f };
	ani.frame_rate_ms = framerate;
	ani.full_rate_ms = framerate;
	ani.is_active = true;
	registry.alwaysplayAni.insert(entity, ani);

	registry.auras.emplace(entity);
	registry.auraLinks.emplace(entity, entity_to_link);
	registry.auraLinks.emplace(entity_to_link, entity);

	registry.renderRequests.insert(
		entity,
		{ texture_asset,
		EFFECT_ASSET_ID::TEXTURED,
		GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}
