#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	ComponentContainer<HitTimer> hitTimers;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<RenderRequest> renderRequestsForeground;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<Pickupable> pickupables;
	ComponentContainer<Deadly> deadlys;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;
	ComponentContainer<Floor> floors;
	ComponentContainer<Wall> walls;
	ComponentContainer<Door> doors;
	ComponentContainer<EnemyBullet> enemyBullets;
	ComponentContainer<InvulnerableTimer> invulnerableTimers;
	ComponentContainer<HP> hps;
	ComponentContainer<PlayerBullet> playerBullets;
	ComponentContainer<IdleMoveAction> idleMoveActions;
	ComponentContainer<BulletSpawner> bulletSpawners;
	ComponentContainer<DeathTimer> realDeathTimers;
	ComponentContainer<Kinematic> kinematics;
	ComponentContainer<Collidable> collidables;
	ComponentContainer<AiTimer> aitimers;
	ComponentContainer<FollowPath> followpaths;
	ComponentContainer<EntityAnimation> animation;
	ComponentContainer<BeeEnemy> beeEnemies;
	ComponentContainer<BomberEnemy> bomberEnemies;
	ComponentContainer<WolfEnemy> wolfEnemies;
	ComponentContainer<Coin> coins;
	ComponentContainer<Product> products;
	ComponentContainer<MaxHPIncrease> maxhpIncreases;
	ComponentContainer<AttackUp> attackUps;
	ComponentContainer<Chest> chests;
	ComponentContainer<Key> keys;
	ComponentContainer<Boss> bosses;
	ComponentContainer<BulletPattern> bulletPatterns;
	ComponentContainer<BulletDelayTimer> bulletDelayTimers;
	ComponentContainer<BulletDeathTimer> bulletDeathTimers;
	ComponentContainer<BulletLoop> bulletLoops;
	ComponentContainer<PlayerHeart> playerHearts;
	ComponentContainer<BossHealthBarUI> bossHealthBarUIs;
	ComponentContainer<BossHealthBarLink> bossHealthBarLink;
	ComponentContainer<FollowFlowField> followFlowField;
	ComponentContainer<CircleCollidable> circleCollidables;
	ComponentContainer<EntityAnimation> alwaysplayAni;
	ComponentContainer<BezierCurve> bezierCurves;
	ComponentContainer<FocusDot> focusdots; // only for rendering dot for reimu
	ComponentContainer<BulletStartFiringTimer> bulletStartFiringTimers;
	ComponentContainer<RenderText> texts;
	ComponentContainer<RenderTextPermanent> textsPerm;	
	ComponentContainer<RenderTextWorld> textsWorld;
	ComponentContainer<RenderTextPermanentWorld> textsPermWorld;
	ComponentContainer<UIUX> UIUX;
	ComponentContainer<UIUXWorld> UIUXWorld;
	ComponentContainer<DummyEnemy> dummyenemies;
	ComponentContainer<DummyEnemySpawner> dummyenemyspawners;
	ComponentContainer<DummyEnemyLink> dummyEnemyLink;
	ComponentContainer<TileInstanceData> tileInstanceData;
	ComponentContainer<VisibilityTileInstanceData> visibilityTileInstanceData;
	ComponentContainer<Button> buttons;
	ComponentContainer<MainMenu> mainMenus;
	ComponentContainer<PauseMenu> pauseMenus;
	ComponentContainer<Purchasableable> purchasableables;
	ComponentContainer<Dialogue> dialogueMenus;
	ComponentContainer<Teleporter> teleporters;
	ComponentContainer<WinMenu> winMenus;
	ComponentContainer<LoseMenu> loseMenus;
	ComponentContainer<InfographicMenu> infographicsMenus;
	ComponentContainer<NPC> npcs;
	ComponentContainer<LizardEnemy> lizardEnemies;
	ComponentContainer<WormEnemy> wormEnemies;
	ComponentContainer<Bee2Enemy> bee2Enemies;
	ComponentContainer<GargoyleEnemy> gargoyleEnemies;
	ComponentContainer<EntityAnimation> playonceAni;
	ComponentContainer<AimbotCursor> aimbotCursors;
	ComponentContainer<AimbotBullet> aimbotBullets;
	ComponentContainer<AoeBullet> aoeBullets;
	ComponentContainer<NormalBullet> normalBullets;
	ComponentContainer<CoinFountain> coinFountains;
	ComponentContainer<FlyToPlayer> flytoplayers;
	ComponentContainer<Aura> auras;
	ComponentContainer<AuraLink> auraLinks;
	ComponentContainer<BulletSpeedTimer> bulletSpeedTimers;
	ComponentContainer<BossInvisible> bossInvisibles;
	ComponentContainer<Parralex> parrallaxes;
	ComponentContainer<TurtleEnemy> turtleEnemies;
	ComponentContainer<SkeletonEnemy> skeletonEnemies;
	ComponentContainer<SeagullEnemy> seagullEnemies;
	ComponentContainer<PlaceboWall> placeboWalls;
	ComponentContainer<RoomSignifier> roomSignifiers;
	ComponentContainer<OptionMenu> optionMenus;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&hitTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&renderRequestsForeground);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&pickupables);
		registry_list.push_back(&deadlys);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
		registry_list.push_back(&floors);
		registry_list.push_back(&walls);
		registry_list.push_back(&doors);
		registry_list.push_back(&enemyBullets);
		registry_list.push_back(&invulnerableTimers);
		registry_list.push_back(&hps);
		registry_list.push_back(&playerBullets);
		registry_list.push_back(&idleMoveActions);
		registry_list.push_back(&bulletSpawners);
		registry_list.push_back(&realDeathTimers);
		registry_list.push_back(&kinematics);
		registry_list.push_back(&collidables);
		registry_list.push_back(&aitimers);
		registry_list.push_back(&followpaths);
		registry_list.push_back(&animation);
		registry_list.push_back(&beeEnemies);
		registry_list.push_back(&bomberEnemies);
		registry_list.push_back(&wolfEnemies);
		registry_list.push_back(&coins);
		registry_list.push_back(&products);
		registry_list.push_back(&maxhpIncreases);
		registry_list.push_back(&attackUps);
		registry_list.push_back(&chests);
		registry_list.push_back(&keys);
		registry_list.push_back(&bosses);
		registry_list.push_back(&bulletPatterns);
		registry_list.push_back(&bulletDelayTimers);
		registry_list.push_back(&bulletDeathTimers);
		registry_list.push_back(&bulletLoops);
		registry_list.push_back(&playerHearts);
		registry_list.push_back(&bossHealthBarUIs);
		registry_list.push_back(&bossHealthBarLink);
		registry_list.push_back(&followFlowField);
		registry_list.push_back(&circleCollidables);
		registry_list.push_back(&alwaysplayAni);
		registry_list.push_back(&bezierCurves);
		registry_list.push_back(&focusdots);
		registry_list.push_back(&bulletStartFiringTimers);
		registry_list.push_back(&texts);
		registry_list.push_back(&textsPerm);		
		registry_list.push_back(&textsWorld);
		registry_list.push_back(&textsPermWorld);
		registry_list.push_back(&UIUX);
		registry_list.push_back(&UIUXWorld);
		registry_list.push_back(&dummyenemies);
		registry_list.push_back(&dummyenemyspawners);
		registry_list.push_back(&dummyEnemyLink);
		registry_list.push_back(&tileInstanceData);
		registry_list.push_back(&visibilityTileInstanceData);
		registry_list.push_back(&buttons);
		registry_list.push_back(&mainMenus);
		registry_list.push_back(&pauseMenus);
		registry_list.push_back(&purchasableables);
		registry_list.push_back(&dialogueMenus);
		registry_list.push_back(&teleporters);
		registry_list.push_back(&winMenus);
		registry_list.push_back(&loseMenus);
		registry_list.push_back(&infographicsMenus);
		registry_list.push_back(&npcs);
		registry_list.push_back(&lizardEnemies);
		registry_list.push_back(&wormEnemies);
		registry_list.push_back(&bee2Enemies);
		registry_list.push_back(&gargoyleEnemies);
		registry_list.push_back(&playonceAni);
		registry_list.push_back(&aimbotCursors);
		registry_list.push_back(&aimbotBullets);
		registry_list.push_back(&aoeBullets);
		registry_list.push_back(&normalBullets);
		registry_list.push_back(&coinFountains);
		registry_list.push_back(&flytoplayers);
		registry_list.push_back(&auras);
		registry_list.push_back(&auraLinks);
		registry_list.push_back(&bulletSpeedTimers);
		registry_list.push_back(&bossInvisibles);
		registry_list.push_back(&parrallaxes);
		registry_list.push_back(&turtleEnemies);
		registry_list.push_back(&skeletonEnemies);
		registry_list.push_back(&seagullEnemies);
		registry_list.push_back(&placeboWalls);
		registry_list.push_back(&roomSignifiers);
		registry_list.push_back(&optionMenus);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;