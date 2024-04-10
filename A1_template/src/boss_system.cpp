#include "boss_system.hpp"

BossSystem::BossSystem() {
	init_phases();
}

void BossSystem::step(float elapsed_ms) {
	for (Entity entity : registry.bosses.entities) {
		Boss& boss = registry.bosses.get(entity);
		if (!boss.is_active) continue;
		HP& hp = registry.hps.get(entity);
		// phase change
		if (boss.phase_index < boss.health_phase_thresholds.size() &&
			hp.curr_hp <= boss.health_phase_thresholds[boss.phase_index]) {
			boss.phase_index++;
			std::random_device ran;
			std::mt19937 gen(ran());
			set_random_phase(boss, gen, entity);

			// ignore index 0 phase, as boss does not attack in phase 0. this can be changed if index 0 is firing bullets.
			if (boss.phase_index - 1 > 0) {
				// set invulnerable and stop firing after amount of time
				// emplace with duplicates if a phase is close to each other, triggering phase change before time expires
				InvulnerableTimer& invulnerable_timer = registry.invulnerableTimers.emplace_with_duplicates(entity);
				invulnerable_timer.invulnerable_counter_ms = boss.phase_change_time;
				BulletStartFiringTimer& bullet_start_firing_timer = registry.bulletStartFiringTimers.emplace_with_duplicates(entity);
				BulletSpawner& bullet_spawner = registry.bulletSpawners.get(entity);
				bullet_spawner.is_firing = false;
				bullet_start_firing_timer.counter_ms = boss.phase_change_time;
				// remove all bullets
				while (registry.enemyBullets.entities.size() > 0)
					registry.remove_all_components_of(registry.enemyBullets.entities.back());
			}
		}

		if (boss.duration != -1) {
			boss.current_duration += elapsed_ms;
			if (boss.current_duration >= boss.duration) {
				std::random_device ran;
				std::mt19937 gen(ran());
				set_random_phase(boss, gen, entity);
				boss.current_duration = 0;
			}
		}
	}
}

void BossSystem::set_random_phase(Boss& boss, std::mt19937& gen, const Entity& entity)
{
	// check if phase index is in bounds
	if (boss.phase_index < bullet_phases.size()) {
		// given a random premade bullet phase,
		// if no bullet pattern -> uses bullet spawner only
		// if no bullet spawner -> uses previous spawner
		std::vector<BulletPhase>& phases = bullet_phases[boss.phase_index];
		if (phases.size() == 0) return; // no need to set phase if there are no phases
		std::uniform_int_distribution<> int_distrib(0, phases.size() - 1);
		int random_index = int_distrib(gen);
		BulletPhase& phase = phases[random_index];
		// if the new randomly generated phase is the same, don't bother setting phase
		if (boss.current_bullet_phase_id == phase.id) return;
		if (phase.bullet_spawner != BulletSpawner()) {
			if (!registry.bulletSpawners.has(entity)) registry.bulletSpawners.emplace(entity);
			BulletSpawner& boss_spawner = registry.bulletSpawners.get(entity);
			boss_spawner = phase.bullet_spawner;
		}
		boss.bullet_pattern = phase.bullet_pattern;
		boss.current_bullet_phase_id = phase.id;
	}
}

void BossSystem::init_phases() {
	bullet_phases.clear();
	BulletPhase b_phase;
	BulletPattern b_pattern;
	BulletSpawner bs;
	bullet_phases = std::vector<std::vector<BulletPhase>>(5);

	if (map_info.level == MAP_LEVEL::LEVEL1) {
		// currently assume we have 4 phases (index 0 phase empty)

		bs = BulletSpawner();
		bs.fire_rate = 2;
		bs.is_firing = true;
		bs.spin_rate = 1;
		bs.invert = false;
		bs.spin_delta = 0.f;
		bs.max_spin_rate = 20.f;
		bs.total_bullet_array = 1;
		bs.spread_between_array = 0;
		bs.bullets_per_array = 4;
		bs.spread_within_array = 90;
		bs.bullet_initial_speed = 100;
		b_phase.bullet_spawner = bs;
		b_phase.id = bullet_phase_id_count++;
		bullet_phases[1].push_back(b_phase);

		bs = BulletSpawner();
		bs.fire_rate = 2;
		bs.is_firing = true;
		bs.spin_rate = 13;
		bs.invert = false;
		bs.spin_delta = 0.f;
		bs.max_spin_rate = 20.f;
		bs.total_bullet_array = 3;
		bs.spread_between_array = 118;
		bs.bullets_per_array = 1;
		bs.spread_within_array = 21;
		bs.bullet_initial_speed = 50;
		b_phase.bullet_spawner = bs;
		b_phase.id = bullet_phase_id_count++;
		bullet_phases[2].push_back(b_phase);

		bs = BulletSpawner();
		bs.fire_rate = 1;
		bs.is_firing = true;
		bs.spin_rate = 20;
		bs.invert = false;
		bs.spin_delta = 0.f;
		bs.max_spin_rate = 20.f;
		bs.total_bullet_array = 3;
		bs.spread_between_array = 120;
		bs.bullets_per_array = 3;
		bs.spread_within_array = 30;
		bs.bullet_initial_speed = 50;
		b_phase.bullet_spawner = bs;
		b_phase.id = bullet_phase_id_count++;
		bullet_phases[3].push_back(b_phase);

		bs = BulletSpawner();
		b_pattern = BulletPattern();
		bs.fire_rate = 2;
		bs.is_firing = true;
		bs.spin_rate = 2;
		bs.invert = false;
		bs.spin_delta = 1.f;
		bs.max_spin_rate = 20.f;
		bs.total_bullet_array = 3;
		bs.spread_between_array = 30;
		bs.bullets_per_array = 4;
		bs.spread_within_array = 90;
		bs.bullet_initial_speed = 100;
		bs.cooldown_rate = 70;
		bs.number_to_fire = 10;
		b_pattern.commands = {
			{ BULLET_ACTION::DELAY, 1000.f },
			{ BULLET_ACTION::ROTATE, 20.f },
			{ BULLET_ACTION::DELAY, 100.f },
			{ BULLET_ACTION::LOOP, vec2(10, 1)},
			{ BULLET_ACTION::DELAY, 1500.f },
			{ BULLET_ACTION::ROTATE, 30.f },
			{ BULLET_ACTION::DELAY, 150.f },
			{ BULLET_ACTION::LOOP, vec2(10, 4)},
			{ BULLET_ACTION::DELAY, 5000.f },
			{ BULLET_ACTION::DEL, 0.f },
		};
		b_phase.bullet_pattern = b_pattern;
		b_phase.bullet_spawner = bs;
		b_phase.id = bullet_phase_id_count++;
		bullet_phases[4].push_back(b_phase);

		bs = BulletSpawner();
		b_pattern = BulletPattern();
		bs.fire_rate = 1;
		bs.is_firing = true;
		bs.spin_rate = 2;
		bs.invert = false;
		bs.spin_delta = 1.f;
		bs.max_spin_rate = 20.f;
		bs.total_bullet_array = 3;
		bs.spread_between_array = 30;
		bs.bullets_per_array = 4;
		bs.spread_within_array = 90;
		bs.bullet_initial_speed = 100;
		bs.cooldown_rate = 100;
		bs.number_to_fire = 5;
		b_pattern.commands = {
			{ BULLET_ACTION::DELAY, 500.f },
			{ BULLET_ACTION::DIRECTION, vec2(0,1) },
			{ BULLET_ACTION::DELAY, 1500.f },
			{ BULLET_ACTION::DIRECTION, vec2(0,0) },
			{ BULLET_ACTION::DELAY, 1000.f },
			{ BULLET_ACTION::DIRECTION, vec2(1,1) },
			{ BULLET_ACTION::SPLIT, vec3(10, 36, -100)},
		};
		b_phase.bullet_pattern = b_pattern;
		b_phase.bullet_spawner = bs;
		b_phase.id = bullet_phase_id_count++;
		bullet_phases[4].push_back(b_phase);
	}
	else if (map_info.level == MAP_LEVEL::LEVEL2) {
		// currently assume we have 4 phases (index 0 phase empty)
		bs = BulletSpawner();
		b_pattern = BulletPattern();
		bs.fire_rate = 2;
		bs.is_firing = true;
		bs.spin_rate = 2;
		bs.invert = true;
		bs.spin_delta = 0.5f;
		bs.max_spin_rate = 10.f;
		bs.total_bullet_array = 3;
		bs.spread_between_array = 41;
		bs.bullets_per_array = 2;
		bs.spread_within_array = 90;
		bs.bullet_initial_speed = 100;
		bs.cooldown_rate = 100;
		bs.number_to_fire = 20;
		b_pattern.commands = {
			{ BULLET_ACTION::DELAY, 500.f },
			{ BULLET_ACTION::SPEED, -100.f },
			{ BULLET_ACTION::DELAY, 200.f },
			{ BULLET_ACTION::SPEED, -50.f },
			{ BULLET_ACTION::DELAY, 200.f },
			{ BULLET_ACTION::SPEED, 0.f },
			{ BULLET_ACTION::DELAY, 1000.f },
			{ BULLET_ACTION::SPEED, 200.f },
			{ BULLET_ACTION::ROTATE, 45.f },
			{ BULLET_ACTION::DELAY, 1500.f },
			{ BULLET_ACTION::SPLIT, vec3(2, 20, 0)},
		};
		b_phase.bullet_pattern = b_pattern;
		b_phase.bullet_spawner = bs;
		b_phase.id = bullet_phase_id_count++;
		bullet_phases[1].push_back(b_phase);
	}
}

/*
REFERENCE:

Bullet actions:
Floats:
SPEED - float - change in velocity magnitude
ROTATE - float - change in bullet direction
DELAY - float - wait until executing next command
DEL - float - bullet death timer to remove bullet

Vec2s:
LOOP - vec2 - loop back to specified index
	- vec2[0] = number to loop (specify 1 - loops once)
	- vec2[1] = 0-indexed to loop to (should be index >= 0 && index < commands.size)
DIRECTION - vec2 - change direction to x,y

Vec3s:
SPLIT - vec3 - split one bullets into multiple bullets based on angle
	- vec2[0] = number of bullets to split into (<= 1 - won't do anything)
	- vec2[1] = angle for the bullet spread
	- vec2[2] = initial bullet speed

enum class BULLET_ACTION {
	SPEED,
	ROTATE,
	DELAY,
	LOOP,
	DEL,
	SPLIT,
	DIRECTION
};
*/
