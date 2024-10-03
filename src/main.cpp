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
};

sEntity player;
sEntity dungeon_gate;
sEntity orc;

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

	player.x = TILE_WIDTH * 3;
	player.y = TILE_HEIGHT * 3;
	player.zone = ZONE_WORLD;

	// position of dungeon gate
	dungeon_gate.x = TILE_WIDTH * 10;
	dungeon_gate.y = TILE_HEIGHT * 10;
	dungeon_gate.zone = ZONE_ALL;

	orc.x = TILE_WIDTH * 5;
	orc.y = TILE_HEIGHT * 5;
	orc.zone = ZONE_DUNGEON;

	camera.target = { player.x, player.y };
	camera.offset = { (screenWidth / 2), (screenHeight / 2) };
	camera.rotation = 0.0f;
	camera.zoom = 3.0f;

};

static void GameUpdate() {

	float x = player.x;
	float y = player.y;

	if (IsKeyPressed(KEY_LEFT)) {
		x -= 1 * TILE_WIDTH;
	}
	else if (IsKeyPressed(KEY_RIGHT)) {
		x += 1 * TILE_WIDTH;
	}
	else if (IsKeyPressed(KEY_UP)) {
		y -= 1 * TILE_HEIGHT;
	}
	else if (IsKeyPressed(KEY_DOWN)) {
		y += 1 * TILE_HEIGHT;
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

	player.x = x;
	player.y = y;

	camera.target = { player.x, player.y };

	if (IsKeyPressed(KEY_E)) {
		if (player.x == dungeon_gate.x &&
			player.y == dungeon_gate.y) {
			//enter dungeon
			if (player.zone == ZONE_WORLD) {
				player.zone = ZONE_DUNGEON;
			}
			else if (player.zone == ZONE_DUNGEON) {
				player.zone = ZONE_WORLD;
			}
		}
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
		DrawTile(orc.x, orc.y, 11, 0);
	}

	// render player
	DrawTile(camera.target.x, camera.target.y, 4, 0);

	EndMode2D();

	DrawRectangle(5, 5, 330, 120, Fade(SKYBLUE, 0.5f));
	DrawRectangleLines(5, 5, 330, 120, BLUE);

	DrawText(TextFormat("Camera Target: (%06.2f, %06.2f", camera.target.x, camera.target.y), 15, 10, 14, YELLOW);
	DrawText(TextFormat("Camera Zoom: %06.2f", camera.zoom), 15, 30, 14, YELLOW);

};

static void GameShutdown() {

	for (int i = 0; i < MAX_TEXTURES; i++) {
		UnloadTexture(textures[i]);
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
