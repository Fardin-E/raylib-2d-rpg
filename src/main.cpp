// main.cpp : Defines the entry point for the application.
// following the youtube tutorial "Start Your Own 2D RPG With Raylib | TUTORIAL by Erik Yuzwa"
// https://www.youtube.com/watch?v=vjU62r-M1CY&t=1404s

#include "header.h"

constexpr int TILE_WIDTH = 8;
constexpr int TILE_HEIGHT = 8;

const int screenWidth = 800;
const int screenHeight = 600;

constexpr int MAX_TEXTURES = 1;

enum texture_asset {
	TEXTURE_TILEMAP = 0
};

Texture2D textures[MAX_TEXTURES];

constexpr int WORLD_WIDTH = 20; // 20 * TILE_WIDTH
constexpr int WORLD_HEIGHT = 20; // 20 * TILE_HEIGHT

#define MAX_SOUNDS 5
enum sound_asset {
	SOUND_FOOT_GRASS = 0,
	SOUND_FOOT_STONE,
	SOUND_ATTACK,
	SOUND_DEATH,
	SOUND_COINS
};

Sound sounds[MAX_SOUNDS];

#define MAX_MUSIC 2
enum music_asset {
	MUSIC_LIGHT_AMBIENCE = 0,
	MUSIC_DARK_AMBIENCE
};

Music music[MAX_MUSIC];

enum tile_type {
	TILE_TYPE_DIRT = 0,
	TILE_TYPE_GRASS,
	TILE_TYPE_TREE,
	TILE_TYPE_STONE,
};

struct sTile{
	int x;
	int y;
	int type;

	// Constructor
	sTile(int x_val = 0, int y_val = 0, int type_val = 0) : x(x_val), y(y_val), type(type_val) {}
};

enum eZone {
	ZONE_ALL = 0,
	ZONE_WORLD,
	ZONE_DUNGEON
};

sTile world[WORLD_WIDTH][WORLD_HEIGHT];
sTile dungeon[WORLD_WIDTH][WORLD_HEIGHT];

Camera2D camera = { 0 };

struct sEntity {
	float x;
	float y;
	eZone zone;
	bool isAlive;
	bool isPassable;
	int health;
	int damage;
	int money;
	int experience;
};

sEntity player;
sEntity dungeon_gate;
sEntity orc;
sEntity chest = { 0 };

struct sTimer {
	double startTime;
	double lifeTime;
	bool isActive;
};

void StartTimer(sTimer* timer, double lifetime) {
	timer->startTime = GetTime();
	timer->lifeTime = lifetime;
};
bool IsTimerDone(sTimer timer) {
	return GetTime() - timer.startTime >= timer.lifeTime;
};
double GetElapsed(sTimer timer) {
	return GetTime() - timer.startTime;
};

sTimer combatTextTimer;

static void DrawTile(float pos_x, float pos_y, float texture_index_x, float texture_index_y) {
	Rectangle source = { (TILE_WIDTH * texture_index_x), (TILE_HEIGHT * texture_index_y), TILE_WIDTH, TILE_HEIGHT };
	Rectangle dest = { (pos_x), (pos_y), TILE_WIDTH, TILE_HEIGHT };
	Vector2 origin = { 0, 0 };
	DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin, 0.0f, WHITE);
}

static void GameStartup() {
	
	InitAudioDevice();

	// the tilemap is currently 128x80 - each tile is 8x8
	// giving us 16 tiles across and 10 tiles down
	Image image = LoadImage("assets/colored_tilemap_packed.png");
	textures[TEXTURE_TILEMAP] = LoadTextureFromImage(image);
	UnloadImage(image);

	for (int i = 0; i < WORLD_WIDTH; i++) {
		for (int j = 0; j < WORLD_HEIGHT; j++) {

			int type_world = GetRandomValue(TILE_TYPE_DIRT, TILE_TYPE_TREE);
			int type_dungeon = TILE_TYPE_DIRT;

			world[i][j] = sTile(i, j, type_world);
			dungeon[i][j] = sTile(i, j, type_dungeon);
		}
	}

	// player initialization
	player.x = TILE_WIDTH * 3;
	player.y = TILE_HEIGHT * 3;
	player.zone = ZONE_WORLD;
	player.isAlive = true;
	player.isPassable = false;
	player.health = 100;
	player.damage = 0;
	player.money = 1000;
	player.experience = 0;


	// position of dungeon gate
	dungeon_gate.x = TILE_WIDTH * 10;
	dungeon_gate.y = TILE_HEIGHT * 10;
	dungeon_gate.zone = ZONE_ALL;

	// orc initialization
	orc.x = TILE_WIDTH * 5;
	orc.y = TILE_HEIGHT * 5;
	orc.zone = ZONE_DUNGEON;
	orc.isAlive = true;
	orc.isPassable = false;
	orc.health = 100;
	orc.damage = 0;
	orc.experience = GetRandomValue(10, 100);
	

	// camera initialization
	camera.target = { player.x, player.y };
	camera.offset = { (screenWidth / 2), (screenHeight / 2) };
	camera.rotation = 0.0f;
	camera.zoom = 3.0f;

	sounds[SOUND_FOOT_GRASS] = LoadSound("assets/Grass1.wav");
	sounds[SOUND_FOOT_STONE] = LoadSound("assets/Concrete1.wav");
	sounds[SOUND_ATTACK] = LoadSound("assets/07_human_atk_sword_2.wav");
	sounds[SOUND_DEATH] = LoadSound("assets/24_orc_death_spin.wav");
	sounds[SOUND_COINS] = LoadSound("assets/handleCoins.ogg");

	music[MUSIC_LIGHT_AMBIENCE] = LoadMusicStream("assets/light-ambience.mp3");
	music[MUSIC_DARK_AMBIENCE] = LoadMusicStream("assets/dark-ambience.mp3");

	PlayMusicStream(music[MUSIC_LIGHT_AMBIENCE]);
};

static void GameUpdate() {

	if (player.zone == ZONE_WORLD) {
		UpdateMusicStream(music[MUSIC_LIGHT_AMBIENCE]);
	}
	else if (player.zone == ZONE_DUNGEON) {
		UpdateMusicStream(music[MUSIC_DARK_AMBIENCE]);
	}

	float x = player.x;
	float y = player.y;
	bool hasKeyBeenPressed = false;

	if (IsKeyPressed(KEY_LEFT)) {
		x -= 1 * TILE_WIDTH;
		hasKeyBeenPressed = true;
	}
	else if (IsKeyPressed(KEY_RIGHT)) {
		x += 1 * TILE_WIDTH;
		hasKeyBeenPressed = true;
	}
	else if (IsKeyPressed(KEY_UP)) {
		y -= 1 * TILE_HEIGHT;
		hasKeyBeenPressed = true;
	}
	else if (IsKeyPressed(KEY_DOWN)) {
		y += 1 * TILE_HEIGHT;
		hasKeyBeenPressed = true;
	}

	float wheel = GetMouseWheelMove();

	if (wheel != 0) {
		const float zoomIncrement = 0.125f;
		camera.zoom += (wheel * zoomIncrement);
		if (camera.zoom < 3.0f) {
			camera.zoom = 3.0f;
		}
		if (camera.zoom > 8.0f) {
			camera.zoom = 8.0f;
		}
	}

	// check for orc collision
	if (player.zone == orc.zone &&
		orc.isAlive && orc.x == x && orc.y == y) {
		int damage = GetRandomValue(2, 20); // 2d10
		orc.health -= damage;
		orc.damage = damage;

		if (!combatTextTimer.isActive) {
			combatTextTimer.isActive = true;
			StartTimer(&combatTextTimer, 0.50);
		}

		if (orc.health < 0) {
			PlaySound(sounds[SOUND_DEATH]);
			//TODO: dead!
			orc.isAlive = false;
			player.experience += orc.experience;

			chest.isAlive = true;
			chest.x = orc.x;
			chest.y = orc.y;
			chest.zone = orc.zone;
			chest.money = GetRandomValue(10, 100);
		}
		else {
			PlaySound(sounds[SOUND_ATTACK]);
		}
	}
	else {

		if (hasKeyBeenPressed) {
			if (player.zone == ZONE_WORLD) {
				PlaySound(sounds[SOUND_FOOT_GRASS]);
			}
			else if (player.zone == ZONE_DUNGEON) {
				PlaySound(sounds[SOUND_FOOT_STONE]);
			}
		}

		player.x = x;
		player.y = y;
		camera.target = { player.x, player.y };
	}


	if (IsKeyPressed(KEY_E)) {
		if (player.x == dungeon_gate.x &&
			player.y == dungeon_gate.y) {
			//enter dungeon
			if (player.zone == ZONE_WORLD) {
				player.zone = ZONE_DUNGEON;
				StopMusicStream(music[MUSIC_LIGHT_AMBIENCE]);
				PlayMusicStream(music[MUSIC_DARK_AMBIENCE]);
			}
			else if (player.zone == ZONE_DUNGEON) {
				player.zone = ZONE_WORLD;
				StopMusicStream(music[MUSIC_DARK_AMBIENCE]);
				PlayMusicStream(music[MUSIC_LIGHT_AMBIENCE]);
			}
		}
	}

	if (IsKeyPressed(KEY_G)) {
		if (player.x == chest.x &&
			player.y == chest.y) {
			
			chest.isAlive = false;
			player.money += chest.money;
			PlaySound(sounds[SOUND_COINS]);
		}
	}

	if (IsTimerDone(combatTextTimer)) {
		combatTextTimer.isActive = false;
	}
};

static void GameRender() {

	BeginMode2D(camera);

	sTile tile;
	int texture_index_x = 0;
	int texture_index_y = 0;

	for (int i = 0; i < WORLD_WIDTH; i++) {
		for (int j = 0; j < WORLD_HEIGHT; j++) {
			if (player.zone == ZONE_WORLD) {
				tile = world[i][j];
			}
			else if (player.zone == ZONE_DUNGEON) {
				tile = dungeon[i][j];
			}

			switch (tile.type) {
			case TILE_TYPE_DIRT:
				texture_index_x = 4;
				texture_index_y = 4;
				break;
			case TILE_TYPE_GRASS:
				texture_index_x = 5;
				texture_index_y = 4;
				break;
			case TILE_TYPE_TREE:
				texture_index_x = 5;
				texture_index_y = 5;
				break;
			case TILE_TYPE_STONE:
				texture_index_x = 1;
				texture_index_y = 6;
				break;
			}

			DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, texture_index_x, texture_index_y);
		}
	}

	// render dungeon gate
	// draw gate
	DrawTile(dungeon_gate.x, dungeon_gate.y, 8, 9);

	if (orc.zone == player.zone) {
		if (orc.isAlive) {
			DrawTile(orc.x, orc.y, 11, 0);
		}
		if (combatTextTimer.isActive) {
			DrawText(TextFormat("%d", orc.damage), orc.x, orc.y - 10, 9, YELLOW);
		}
		if (chest.isAlive) {
			DrawTile(chest.x, chest.y, 9, 3);
		}
	}

	// render player
	DrawTile(camera.target.x, camera.target.y, 4, 0);

	EndMode2D();

	DrawRectangle(5, 5, 330, 120, Fade(SKYBLUE, 0.5f));
	DrawRectangleLines(5, 5, 330, 120, BLUE);

	DrawText(TextFormat("Camera Target: (%06.2f, %06.2f", camera.target.x, camera.target.y), 15, 10, 14, YELLOW);
	DrawText(TextFormat("Camera Zoom: %06.2f", camera.zoom), 15, 30, 14, YELLOW);
	DrawText(TextFormat("Player Health: %d", player.health), 15, 50, 14, YELLOW);
	DrawText(TextFormat("Player XP: %d", player.experience), 15, 70, 14, YELLOW);
	DrawText(TextFormat("Player Money: %d", player.money), 15, 90, 14, YELLOW);

	if (orc.isAlive) {
		DrawText(TextFormat("Orc Health: %d", orc.health), 15, 110, 14, YELLOW);
	}
	
};

static void GameShutdown() {

	for (int i = 0; i < MAX_TEXTURES; i++) {
		UnloadTexture(textures[i]);
	}

	for (int i = 0; i < MAX_SOUNDS; i++) {
		UnloadSound(sounds[i]);
	}

	for (int i = 0; i < MAX_MUSIC; i++) {
		StopMusicStream(music[i]);
		UnloadMusicStream(music[i]);
	}

	CloseAudioDevice();

};


int main()
{
	InitWindow(screenWidth, screenHeight, "Raylib 2D Rpg");
	SetTargetFPS(60);

	GameStartup();

	while (!WindowShouldClose()) {
		GameUpdate();

		BeginDrawing();
		ClearBackground(GRAY);

		GameRender();

		EndDrawing();
	}

	GameShutdown();

	CloseWindow();
	
	return 0;
}
