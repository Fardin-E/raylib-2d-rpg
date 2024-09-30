// main.cpp : Defines the entry point for the application.
//

#include "header.h"

constexpr auto TILE_WIDTH = 8;
constexpr auto TILE_HEIGHT = 8;

const int screenWidth = 800;
const int screenHeight = 600;

constexpr auto MAX_TEXTURES = 1;

enum texture_asset {
	TEXTURE_TILEMAP = 0
};

Texture2D textures[MAX_TEXTURES];

constexpr auto WORLD_WIDTH = 20; // 20 * TILE_WIDTH
constexpr auto WORLD_HEIGHT = 20; // 20 * TILE_HEIGHT

struct sTile{
	int x;
	int y;

	// Constructor
	sTile(int x_val = 0, int y_val = 0) : x(x_val), y(y_val) {}
};

sTile world[WORLD_WIDTH][WORLD_HEIGHT];

Camera2D camera = { 0 };

struct sEntity {
	float x;
	float y;
};

sEntity player;

static void GameStartup() {
	
	InitAudioDevice();

	// the tilemap is currently 128x80 - each tile is 8x8
	// giving us 16 tiles across and 10 tiles down
	Image image = LoadImage("assets/colored_tilemap_packed.png");
	textures[TEXTURE_TILEMAP] = LoadTextureFromImage(image);
	UnloadImage(image);

	for (int i = 0; i < WORLD_WIDTH; i++) {
		for (int j = 0; j < WORLD_HEIGHT; j++) {
			world[i][j] = sTile(i, j);
		}
	}

	player.x = TILE_WIDTH * 3;
	player.y = TILE_HEIGHT * 3;

	camera.target = { player.x, player.y };
	camera.offset = { (screenWidth / 2), (screenHeight / 2) };
	camera.rotation = 0.0f;
	camera.zoom = 3.0f;

};

static void GameUpdate() {
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

	camera.target = { player.x, player.y };
};

static void GameRender() {

	BeginMode2D(camera);

	sTile tile;
	int texture_index_x = 0;
	int texture_index_y = 0;

	for (int i = 0; i < WORLD_WIDTH; i++) {
		for (int j = 0; j < WORLD_HEIGHT; j++) {
			tile = world[i][j];
			texture_index_x = 4;
			texture_index_y = 4;

			Rectangle source = { texture_index_x * TILE_WIDTH, texture_index_y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT };
			Rectangle dest = { (tile.x * TILE_WIDTH), (tile.y * TILE_HEIGHT), TILE_WIDTH, TILE_HEIGHT };
			Vector2 origin = { 0, 0 };
			DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin, 0.0f, WHITE);
		}
	}

	Rectangle source = { (4 * TILE_WIDTH), (0 * TILE_HEIGHT), TILE_WIDTH, TILE_HEIGHT };
	Rectangle dest = { (camera.target.x), (camera.target.y), TILE_WIDTH, TILE_HEIGHT };
	Vector2 origin = { 0, 0 };
	DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin, 0.0f, WHITE);

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
