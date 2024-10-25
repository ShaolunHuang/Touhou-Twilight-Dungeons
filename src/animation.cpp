// internal
#include "animation.hpp"
#include <iostream>

void Animation::step(float elapsed_ms)
{
	for (Entity& animation_entity : registry.animation.entities) {
		EntityAnimation& animation = registry.animation.get(animation_entity);
		if (!animation.is_active) continue;
		Kinematic& kin = registry.kinematics.get(animation_entity);
		if (kin.direction != vec2(0)) {
			animation.state = State::MOVE;
			animation.offset = 0;
		}
		else {
			animation.state = State::IDLE;
			animation.offset = 4;
		}
		animation.frame_rate_ms -= elapsed_ms;

		if (animation.frame_rate_ms < 0) {
			animation.render_pos.x += animation.spritesheet_scale.x;
			if (animation.render_pos.x > 1.0) {
				animation.render_pos.x = animation.spritesheet_scale.x;
			}
			animation.frame_rate_ms = animation.full_rate_ms;
		}
	}
	for (EntityAnimation& animation : registry.alwaysplayAni.components) {
		if (!animation.is_active) continue;

		animation.frame_rate_ms -= elapsed_ms;

		if (animation.frame_rate_ms < 0) {
			animation.render_pos.x += animation.spritesheet_scale.x;
			if (animation.render_pos.x > 1.0) {
				animation.render_pos.x = animation.spritesheet_scale.x;
			}
			animation.frame_rate_ms = animation.full_rate_ms;
		}
	}

	ComponentContainer<EntityAnimation>& playonceAni_container = registry.playonceAni;
	int playonceAni_container_size = playonceAni_container.size();
	for (int i = playonceAni_container_size - 1; i >= 0; --i) {
		EntityAnimation& animation = playonceAni_container.components[i];
		if (!animation.is_active) continue;

		animation.frame_rate_ms -= elapsed_ms;

		if (animation.frame_rate_ms < 0) {
			animation.render_pos.x += animation.spritesheet_scale.x;
			animation.frame_rate_ms = animation.full_rate_ms;
			if (animation.render_pos.x > 1.0) {
				registry.remove_all_components_of(playonceAni_container.entities[i]);
			}
		}
	}

	double mouse_pos_x;
	double mouse_pos_y;
	glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);
	Motion player_motion;
	for (Entity& player : registry.players.entities) {
		EntityAnimation& player_ani = registry.animation.get(player);
		if (!player_ani.is_active) continue;
		Motion& motion = registry.motions.get(player);
		vec2 last_mouse_position = vec2(mouse_pos_x, mouse_pos_y) - window_px_half + motion.position;
		float x = last_mouse_position.x - motion.position.x;
		float y = last_mouse_position.y - motion.position.y;
		float mouse_angle_degree = (-atan2(x, y) + M_PI) * (180.0 / M_PI);
		if (mouse_angle_degree <= 45 || mouse_angle_degree >= 325) {
			Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::REIMU_FRONT);
			registry.meshPtrs.get(player) = &mesh;
			player_ani.render_pos.y = (4 + player_ani.offset) * player_ani.spritesheet_scale.y;
		}
		else if (mouse_angle_degree <= 135) {
			Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::REIMU_RIGHT);
			registry.meshPtrs.get(player) = &mesh;
			player_ani.render_pos.y = (3 + player_ani.offset) * player_ani.spritesheet_scale.y;
		}
		else if (mouse_angle_degree <= 225) {
			Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::REIMU_FRONT);
			registry.meshPtrs.get(player) = &mesh;
			player_ani.render_pos.y = (1 + player_ani.offset) * player_ani.spritesheet_scale.y;
		}
		else {
			Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::REIMU_LEFT);
			registry.meshPtrs.get(player) = &mesh;
			player_ani.render_pos.y = (2 + player_ani.offset) * player_ani.spritesheet_scale.y;
		}
		player_motion = motion;
	}
	for (Entity& enemy : registry.beeEnemies.entities) {
		EntityAnimation& enemy_ani = registry.animation.get(enemy);
		if (!enemy_ani.is_active) continue;
		if (registry.realDeathTimers.has(enemy)) {
			if (registry.realDeathTimers.get(enemy).first_animation_frame == false) {
				enemy_ani.render_pos.y = 4 * enemy_ani.spritesheet_scale.y + enemy_ani.render_pos.y;
				enemy_ani.render_pos.x = enemy_ani.spritesheet_scale.x;
				registry.realDeathTimers.get(enemy).first_animation_frame = true;
			}
		}
		else {
			Motion& enemy_motion = registry.motions.get(enemy);
			float x = player_motion.position.x - enemy_motion.position.x;
			float y = player_motion.position.y - enemy_motion.position.y;
			float facing_degree = (-atan2(x, y) + M_PI) * (180.0 / M_PI);
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = 4 * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = 3 * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = 1 * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = 2 * enemy_ani.spritesheet_scale.y;
			}
		}
	}
	for (Entity& enemy : registry.bomberEnemies.entities) {
		EntityAnimation& enemy_ani = registry.animation.get(enemy);
		if (!enemy_ani.is_active) continue;
		if (registry.realDeathTimers.has(enemy)) {
			if (registry.realDeathTimers.get(enemy).first_animation_frame == false) {
				float offset = registry.bomberEnemies.get(enemy).touch_player ? 8.f : 4.f;
				enemy_ani.render_pos.y = offset * enemy_ani.spritesheet_scale.y + enemy_ani.render_pos.y;
				enemy_ani.render_pos.x = enemy_ani.spritesheet_scale.x;
				registry.realDeathTimers.get(enemy).first_animation_frame = true;
			}
		}
		else {
			vec2 enemy_velocity = normalize(registry.kinematics.get(enemy).velocity);
			float facing_degree = (-atan2(enemy_velocity.x, enemy_velocity.y) + M_PI) * (180.0 / M_PI);
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = 4 * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = 3 * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = 1 * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = 2 * enemy_ani.spritesheet_scale.y;
			}
		}
	}
	for (Entity& enemy : registry.wolfEnemies.entities) {
		EntityAnimation& enemy_ani = registry.animation.get(enemy);
		if (!enemy_ani.is_active) continue;
		if (registry.realDeathTimers.has(enemy)) {
			if (registry.realDeathTimers.get(enemy).first_animation_frame == false) {
				float offset_death = 8;
				if (enemy_ani.render_pos.y > 4 * enemy_ani.spritesheet_scale.y) {
					offset_death /= 2;
				}
				enemy_ani.render_pos.y = offset_death * enemy_ani.spritesheet_scale.y + enemy_ani.render_pos.y;
				enemy_ani.render_pos.x = enemy_ani.spritesheet_scale.x;
				registry.realDeathTimers.get(enemy).first_animation_frame = true;
			}
		}
		else {
			vec2 enemy_velocity = normalize(registry.kinematics.get(enemy).velocity);
			float facing_degree = (-atan2(enemy_velocity.x, enemy_velocity.y) + M_PI) * (180.0 / M_PI);
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = (4 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = (3 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = (1 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = (2 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
		}
	}
	for (Entity& enemy : registry.lizardEnemies.entities) {
		EntityAnimation& enemy_ani = registry.animation.get(enemy);
		if (!enemy_ani.is_active) continue;
		if (registry.realDeathTimers.has(enemy)) {
			if (registry.realDeathTimers.get(enemy).first_animation_frame == false) {
				float offset_death = 8;
				if (enemy_ani.render_pos.y > 4 * enemy_ani.spritesheet_scale.y) {
					offset_death /= 2;
				}
				enemy_ani.render_pos.y = offset_death * enemy_ani.spritesheet_scale.y + enemy_ani.render_pos.y;
				enemy_ani.render_pos.x = enemy_ani.spritesheet_scale.x;
				registry.realDeathTimers.get(enemy).first_animation_frame = true;
			}
		}
		else {
			vec2 enemy_velocity = normalize(registry.kinematics.get(enemy).velocity);
			float facing_degree = (-atan2(enemy_velocity.x, enemy_velocity.y) + M_PI) * (180.0 / M_PI);
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = (4 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = (3 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = (1 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = (2 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
		}
	}
	for (Entity& enemy : registry.wormEnemies.entities) {
		EntityAnimation& enemy_ani = registry.animation.get(enemy);
		if (!enemy_ani.is_active) continue;
		if (registry.realDeathTimers.has(enemy)) {
			if (registry.realDeathTimers.get(enemy).first_animation_frame == false) {
				float offset_death = 8;
				if (enemy_ani.render_pos.y > 4 * enemy_ani.spritesheet_scale.y) {
					offset_death /= 2;
				}
				enemy_ani.render_pos.y = offset_death * enemy_ani.spritesheet_scale.y + enemy_ani.render_pos.y;
				enemy_ani.render_pos.x = enemy_ani.spritesheet_scale.x;
				registry.realDeathTimers.get(enemy).first_animation_frame = true;
			}
		}
		else {
			vec2 enemy_velocity = normalize(registry.kinematics.get(enemy).velocity);
			float facing_degree = (-atan2(enemy_velocity.x, enemy_velocity.y) + M_PI) * (180.0 / M_PI);
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = (4 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = (3 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = (1 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = (2 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
		}
	}
	for (Entity& enemy : registry.skeletonEnemies.entities) {
		EntityAnimation& enemy_ani = registry.animation.get(enemy);
		if (!enemy_ani.is_active) continue;
		if (registry.realDeathTimers.has(enemy)) {
			if (registry.realDeathTimers.get(enemy).first_animation_frame == false) {
				float offset_death = 8;
				if (enemy_ani.render_pos.y > 4 * enemy_ani.spritesheet_scale.y) {
					offset_death /= 2;
				}
				enemy_ani.render_pos.y = offset_death * enemy_ani.spritesheet_scale.y + enemy_ani.render_pos.y;
				enemy_ani.render_pos.x = enemy_ani.spritesheet_scale.x;
				registry.realDeathTimers.get(enemy).first_animation_frame = true;
			}
		}
		else {
			vec2 enemy_velocity = normalize(registry.kinematics.get(enemy).velocity);
			float facing_degree = (-atan2(enemy_velocity.x, enemy_velocity.y) + M_PI) * (180.0 / M_PI);
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = (4 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = (3 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = (1 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = (2 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
		}
	}
	for (Entity& enemy : registry.turtleEnemies.entities) {
		EntityAnimation& enemy_ani = registry.animation.get(enemy);
		if (!enemy_ani.is_active) continue;
		if (registry.realDeathTimers.has(enemy)) {
			if (registry.realDeathTimers.get(enemy).first_animation_frame == false) {
				float offset_death = 8;
				if (enemy_ani.render_pos.y > 4 * enemy_ani.spritesheet_scale.y) {
					offset_death /= 2;
				}
				enemy_ani.render_pos.y = offset_death * enemy_ani.spritesheet_scale.y + enemy_ani.render_pos.y;
				enemy_ani.render_pos.x = enemy_ani.spritesheet_scale.x;
				registry.realDeathTimers.get(enemy).first_animation_frame = true;
			}
		}
		else {
			vec2 enemy_velocity = normalize(registry.kinematics.get(enemy).velocity);
			float facing_degree = (-atan2(enemy_velocity.x, enemy_velocity.y) + M_PI) * (180.0 / M_PI);
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = (4 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = (3 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = (1 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = (2 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
		}
	}
	for (Entity& enemy : registry.bee2Enemies.entities) {
		EntityAnimation& enemy_ani = registry.animation.get(enemy);
		if (!enemy_ani.is_active) continue;
		if (registry.realDeathTimers.has(enemy)) {
			if (registry.realDeathTimers.get(enemy).first_animation_frame == false) {
				enemy_ani.render_pos.y = 4 * enemy_ani.spritesheet_scale.y + enemy_ani.render_pos.y;
				enemy_ani.render_pos.x = enemy_ani.spritesheet_scale.x;
				registry.realDeathTimers.get(enemy).first_animation_frame = true;
			}
		}
		else {
			Motion& enemy_motion = registry.motions.get(enemy);
			float x = player_motion.position.x - enemy_motion.position.x;
			float y = player_motion.position.y - enemy_motion.position.y;
			float facing_degree = (-atan2(x, y) + M_PI) * (180.0 / M_PI);
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = 4 * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = 3 * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = 1 * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = 2 * enemy_ani.spritesheet_scale.y;
			}
		}
	}
	for (Entity& enemy : registry.gargoyleEnemies.entities) {
		EntityAnimation& enemy_ani = registry.animation.get(enemy);
		if (!enemy_ani.is_active) continue;
		if (registry.realDeathTimers.has(enemy)) {
			if (registry.realDeathTimers.get(enemy).first_animation_frame == false) {
				enemy_ani.render_pos.y = 4 * enemy_ani.spritesheet_scale.y + enemy_ani.render_pos.y;
				enemy_ani.render_pos.x = enemy_ani.spritesheet_scale.x;
				registry.realDeathTimers.get(enemy).first_animation_frame = true;
			}
		}
		else {
			Motion& enemy_motion = registry.motions.get(enemy);
			float x = player_motion.position.x - enemy_motion.position.x;
			float y = player_motion.position.y - enemy_motion.position.y;
			float facing_degree = (-atan2(x, y) + M_PI) * (180.0 / M_PI);
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = 4 * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = 3 * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = 1 * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = 2 * enemy_ani.spritesheet_scale.y;
			}
		}
	}
	for (Entity& enemy : registry.seagullEnemies.entities) {
		EntityAnimation& enemy_ani = registry.animation.get(enemy);
		if (!enemy_ani.is_active) continue;
		if (registry.realDeathTimers.has(enemy)) {
			if (registry.realDeathTimers.get(enemy).first_animation_frame == false) {
				enemy_ani.render_pos.y = 4 * enemy_ani.spritesheet_scale.y + enemy_ani.render_pos.y;
				enemy_ani.render_pos.x = enemy_ani.spritesheet_scale.x;
				registry.realDeathTimers.get(enemy).first_animation_frame = true;
			}
		}
		else {
			Motion& enemy_motion = registry.motions.get(enemy);
			float x = player_motion.position.x - enemy_motion.position.x;
			float y = player_motion.position.y - enemy_motion.position.y;
			float facing_degree = (-atan2(x, y) + M_PI) * (180.0 / M_PI);
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = 4 * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = 3 * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = 1 * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = 2 * enemy_ani.spritesheet_scale.y;
			}
		}
	}
	// TODO: DUMMY ENEMY HAS THE SAME ANIMATION AS WOLF FOR NOW
	for (Entity& enemy : registry.dummyenemies.entities) {
		EntityAnimation& enemy_ani = registry.animation.get(enemy);
		if (!enemy_ani.is_active) continue;
		if (registry.realDeathTimers.has(enemy)) {
			if (registry.realDeathTimers.get(enemy).first_animation_frame == false) {
				float offset_death = 8;
				if (enemy_ani.render_pos.y > 4 * enemy_ani.spritesheet_scale.y) {
					offset_death /= 2;
				}
				enemy_ani.render_pos.y = offset_death * enemy_ani.spritesheet_scale.y + enemy_ani.render_pos.y;
				enemy_ani.render_pos.x = enemy_ani.spritesheet_scale.x;
				registry.realDeathTimers.get(enemy).first_animation_frame = true;
			}
		}
		else {
			vec2 enemy_velocity = normalize(registry.kinematics.get(enemy).velocity);
			float facing_degree = (-atan2(enemy_velocity.x, enemy_velocity.y) + M_PI) * (180.0 / M_PI);
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = (4 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = (3 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = (1 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = (2 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
		}
	}

	// for now, all bosses temporarily have cirno spritesheet
	for (Entity& enemy : registry.bosses.entities) {
		EntityAnimation& enemy_ani = registry.animation.get(enemy);
		if (!enemy_ani.is_active) continue;
		vec2 enemy_velocity = normalize(registry.kinematics.get(enemy).velocity);
		float facing_degree = (-atan2(enemy_velocity.x, enemy_velocity.y) + M_PI) * (180.0 / M_PI);
		Boss& boss = registry.bosses.get(enemy);
		if (boss.boss_id == BOSS_ID::SAKUYA || boss.boss_id == BOSS_ID::REMILIA) {
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = (4 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = (3 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = (1 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = (2 + enemy_ani.offset) * enemy_ani.spritesheet_scale.y;
			}
		}
		else {
			if (facing_degree <= 45 || facing_degree >= 325) {
				enemy_ani.render_pos.y = 4 * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 135) {
				enemy_ani.render_pos.y = 3 * enemy_ani.spritesheet_scale.y;
			}
			else if (facing_degree <= 225) {
				enemy_ani.render_pos.y = 1 * enemy_ani.spritesheet_scale.y;
			}
			else {
				enemy_ani.render_pos.y = 2 * enemy_ani.spritesheet_scale.y;
			}
		}
	}

	for (Entity entity : registry.teleporters.entities) {
		EntityAnimation& ani = registry.alwaysplayAni.get(entity);
		if (!ani.is_active) continue;
		if (ani.render_pos.x + ani.spritesheet_scale.x > 1.0) {
			ani.render_pos.x = ani.spritesheet_scale.x;
			ani.is_active = false;
		}
	}

	for (Entity entity : registry.aimbotBullets.entities) {
		EntityAnimation& ani = registry.alwaysplayAni.get(entity);
		if (!ani.is_active) continue;
		if (ani.render_pos.x + ani.spritesheet_scale.x > 1.0) {
			ani.is_active = false;
		}
	}
}

void Animation::init(RenderSystem* renderer, GLFWwindow* window) {
	this->window = window;
	this->renderer = renderer;
}