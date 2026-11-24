#include "render3d.h"
#include "app.h"
#include "anim.h"
#include "ui.h"
#include "fx.h"
#include "rlgl.h"
// raymath.h is raylib's maths library. Its code gives harmless warnings in C++,
// so they're switched off just for this one include.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include "raymath.h"
#pragma GCC diagnostic pop
#include <cmath>
#include <cstdio>

// From pool.cpp
extern Color ballColours[8];
extern float pocketX[6], pocketY[6];

// ---------- Sizes ----------
// 100 table pixels = 1 unit in 3D. The felt is 6.4 x 3.2 units, a ball is 0.11 units across... radius.
const float S = 0.01f;
const float CENTER_X = FELT_LEFT + FELT_WIDTH / 2.0f;
const float CENTER_Y = FELT_TOP + FELT_HEIGHT / 2.0f;
const float BALL_R = BALL_RADIUS * S;
const float HALF_W = FELT_WIDTH * S / 2;      // half the felt's width
const float HALF_H = FELT_HEIGHT * S / 2;     // half the felt's depth
const float RAIL_W = 0.34f;                   // how far the wooden rail sticks out
const float NOTCH = POCKET_RADIUS * S;        // the rail is cut back this far around each pocket
const float CUSHION_W = 0.07f;                // the green rubber cushion inside the rail
const float RAIL_TOP = 0.17f;
const float CUSHION_TOP = 0.15f;
const float FLOOR_Y = -1.3f;

// ---------- Colours ----------
const Color WOOD_SIDE = { 78, 44, 22, 255 };
const Color WOOD_TOP = { 122, 72, 38, 255 };
const Color CUSHION = { 14, 72, 46, 255 };
const Color CUSHION_TOP_COLOUR = { 20, 86, 54, 255 };
const Color FELT = { 18, 88, 58, 255 };   // darker than ball 6, so the green ball stands out
const Color BODY_TOP = { 40, 28, 20, 255 };
const Color FLOOR_COLOUR = { 52, 38, 28, 255 };
const Color BACKGROUND = { 8, 7, 7, 255 };          // the far, dark corners of the room
const Color LAMP_SHADE = { 24, 62, 44, 255 };       // dark green metal
const Color LAMP_GLOW = { 255, 238, 205, 255 };     // the lit inside of the shade
const Color DIAMOND = { 150, 140, 120, 255 };

// ---------- The lamp ----------
// A long lamp hangs over the table with three bulbs. The lighting shader is told where they are.
const float LAMP_Y = 2.45f;   // the bottom of the shade
const Vector3 BULBS[3] = { { -1.6f, LAMP_Y - 0.05f, 0 }, { 0, LAMP_Y - 0.05f, 0 }, { 1.6f, LAMP_Y - 0.05f, 0 } };

// ---------- The cue ----------
// Its parts from the tip to the butt: length, thickness at each end, colour (normal and golden)
struct CuePart {
    float length, r1, r2;
    Color colour, golden;
};
const int CUE_PART_COUNT = 5;
const float CUE_LENGTH = 3.0f;
const CuePart CUE_PARTS[CUE_PART_COUNT] = {
    { 0.015f, 0.011f, 0.011f, { 80, 130, 200, 255 }, { 80, 130, 200, 255 } },                     // chalked tip
    { 0.025f, 0.0115f, 0.0115f, { 240, 238, 228, 255 }, { 240, 238, 228, 255 } },                 // ferrule
    { CUE_LENGTH * 0.62f - 0.04f, 0.012f, 0.02f, { 222, 184, 128, 255 }, { 240, 200, 70, 255 } }, // shaft
    { CUE_LENGTH * 0.26f, 0.02f, 0.026f, { 40, 30, 28, 255 }, { 150, 100, 20, 255 } },            // wrap
    { CUE_LENGTH * 0.12f, 0.026f, 0.028f, { 92, 46, 24, 255 }, { 200, 150, 40, 255 } },           // butt
};

// ---------- State ----------
RenderTexture2D view;          // the game's 3D scene is drawn into this image
bool viewLoaded = false;
RenderTexture2D menuView;      // and the menu's into this one
bool menuViewLoaded = false;
Camera3D camera;               // the camera actually used (it glides towards the wanted view)
bool cameraReady = false;
bool topDown = false;
Camera3D menuCamera;
float menuAngle = 0.6f;        // how far round the table the menu's camera has gone

Shader lighting;               // shaders/lighting.vs + .fs: the lamp
int shineLoc, shinePowerLoc;
Shader post;                   // shaders/post.fs: glow and dark corners
int texelLoc, vignetteLoc, bloomLoc;
Texture2D shadowTexture;       // a soft round dark spot

Mesh ballMesh;                         // one sphere, shared by every ball
Material ballMaterial[BALL_COUNT];     // each ball's own texture (colour, stripe and number)
Mesh cueMesh[2][CUE_PART_COUNT];       // [0] the normal cue, [1] the golden cue
Material plainMaterial;                // white, lit by the lamp (the cue's colours are in its meshes)

// How each ball is turned, so it rolls. The game's table and the menu's table have one each.
struct Spin {
    Quaternion turn[BALL_COUNT];
    Vector2 last[BALL_COUNT];
    bool known[BALL_COUNT];
};
Spin gameSpin;
Spin menuSpin;

// Slow motion: where the balls were before the last physics step, and how far on from there to draw them
Vector2 blendFrom[BALL_COUNT];
float blendAmount = 1;
bool blending = false;
const float PHYSICS_STEP_TIME = 1.0f / 60;

// The film-like camera for the final 8
bool cinematic = false;
Vector3 cinematicFocus;
float cinematicTime = 0;
float cinematicStartAngle = 0;
float filmBars = 0;   // 0 to 1: how far the black bars have slid in

// Glowing trails: where each ball was over the last few frames (newest first)
const int TRAIL_LENGTH = 14;
Vector3 trail[BALL_COUNT][TRAIL_LENGTH];
int trailCount[BALL_COUNT];

// The menu's own game: its balls roll by themselves, with the real physics
Game menuGame;
float menuWait = 1.0f;
int menuShots = 0;

// A table point (x, y), at a height in 3D units, as a 3D point
Vector3 toWorld(float x, float y, float height) {
    return Vector3{ (x - CENTER_X) * S, height, (y - CENTER_Y) * S };
}

// ============================================================
// Making the balls
// ============================================================

// A sphere mesh whose texture is wrapped round it like a map of the world:
// u goes round the middle (longitude) and v goes from the top to the bottom.
Mesh makeSphereMesh(float radius, int rings, int slices) {
    Mesh mesh = {};
    mesh.vertexCount = (rings + 1) * (slices + 1);
    mesh.triangleCount = rings * slices * 2;
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.normals = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
    mesh.indices = (unsigned short *)MemAlloc(mesh.triangleCount * 3 * sizeof(unsigned short));

    int v = 0;
    for (int i = 0; i <= rings; i++) {
        float lat = PI * i / rings;              // 0 at the top, PI at the bottom
        for (int j = 0; j <= slices; j++) {
            float lon = 2 * PI * j / slices;     // round the middle
            float nx = sinf(lat) * cosf(lon);
            float ny = cosf(lat);
            float nz = sinf(lat) * sinf(lon);
            mesh.vertices[v * 3 + 0] = nx * radius;
            mesh.vertices[v * 3 + 1] = ny * radius;
            mesh.vertices[v * 3 + 2] = nz * radius;
            mesh.normals[v * 3 + 0] = nx;
            mesh.normals[v * 3 + 1] = ny;
            mesh.normals[v * 3 + 2] = nz;
            mesh.texcoords[v * 2 + 0] = 1 - (float)j / slices;   // 1 - ... so numbers aren't mirrored
            mesh.texcoords[v * 2 + 1] = (float)i / rings;
            v++;
        }
    }
    // Two triangles for each square of the grid (counter-clockwise, seen from outside)
    int t = 0;
    for (int i = 0; i < rings; i++) {
        for (int j = 0; j < slices; j++) {
            unsigned short a = (unsigned short)(i * (slices + 1) + j);
            unsigned short b = (unsigned short)((i + 1) * (slices + 1) + j);
            unsigned short c = (unsigned short)(b + 1);
            unsigned short d = (unsigned short)(a + 1);
            mesh.indices[t++] = a; mesh.indices[t++] = c; mesh.indices[t++] = b;
            mesh.indices[t++] = a; mesh.indices[t++] = d; mesh.indices[t++] = c;
        }
    }
    UploadMesh(&mesh, false);
    return mesh;
}

// Each ball's texture is a 512 x 256 "world map": colour, an optional stripe
// round the middle, and a white circle with the number on each side.
Texture2D makeBallTexture(int number, Font font) {
    Color white = { 245, 243, 232, 255 };
    Color colour = ballColours[number % 8];
    if (number == 8) colour = Color{ 18, 18, 20, 255 };
    bool stripe = number >= 9;

    Image image = GenImageColor(512, 256, stripe ? white : colour);
    if (number == 0) {
        // The cue ball: plain, with small red dots so you can see it spin
        ImageClearBackground(&image, white);
        int dots[6][2] = { { 0, 128 }, { 128, 128 }, { 256, 128 }, { 384, 128 }, { 128, 36 }, { 384, 220 } };
        for (int i = 0; i < 6; i++) ImageDrawCircle(&image, dots[i][0], dots[i][1], 9, Color{ 200, 40, 40, 255 });
        ImageDrawCircle(&image, 512, 128, 9, Color{ 200, 40, 40, 255 });   // the dot at 0 wraps round the edge
    } else {
        if (stripe) ImageDrawRectangle(&image, 0, 128 - 46, 512, 92, colour);
        const char *text = TextFormat("%d", number);
        float size = number >= 10 ? 40 : 46;
        Vector2 textSize = MeasureTextEx(font, text, size, 0);
        int spotX[2] = { 128, 384 };   // the two sides of the ball
        for (int k = 0; k < 2; k++) {
            ImageDrawCircle(&image, spotX[k], 128, 34, white);
            ImageDrawTextEx(&image, font, text, Vector2{ spotX[k] - textSize.x / 2, 128 - textSize.y / 2 }, size, 0, BLACK);
        }
    }
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    GenTextureMipmaps(&texture);                          // smaller copies, so it stays smooth far away
    SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR);
    return texture;
}

// Adds one corner of a triangle to a mesh
void addVertex(Mesh *mesh, int *v, Vector3 position, Vector3 normal, Color colour) {
    int i = *v;
    mesh->vertices[i * 3 + 0] = position.x;
    mesh->vertices[i * 3 + 1] = position.y;
    mesh->vertices[i * 3 + 2] = position.z;
    mesh->normals[i * 3 + 0] = normal.x;
    mesh->normals[i * 3 + 1] = normal.y;
    mesh->normals[i * 3 + 2] = normal.z;
    mesh->colors[i * 4 + 0] = colour.r;
    mesh->colors[i * 4 + 1] = colour.g;
    mesh->colors[i * 4 + 2] = colour.b;
    mesh->colors[i * 4 + 3] = colour.a;
    (*v)++;
}

// A tube along y from 0 to 'length', r1 thick at the bottom and r2 at the top,
// closed at both ends, all one colour. The cue is made of five of these.
Mesh makeTubeMesh(float r1, float r2, float length, int slices, Color colour) {
    Mesh mesh = {};
    mesh.vertexCount = slices * 12;    // per slice: 2 triangles of side, 1 for each end
    mesh.triangleCount = slices * 4;
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.normals = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
    mesh.colors = (unsigned char *)MemAlloc(mesh.vertexCount * 4);

    float slope = (r1 - r2) / length;   // a thinner top tips the side's normal upwards a little
    Vector3 down = { 0, -1, 0 };
    Vector3 up = { 0, 1, 0 };
    int v = 0;
    for (int i = 0; i < slices; i++) {
        float a0 = 2 * PI * i / slices;
        float a1 = 2 * PI * (i + 1) / slices;
        Vector3 b0 = { r1 * cosf(a0), 0, r1 * sinf(a0) };         // bottom ring
        Vector3 b1 = { r1 * cosf(a1), 0, r1 * sinf(a1) };
        Vector3 t0 = { r2 * cosf(a0), length, r2 * sinf(a0) };    // top ring
        Vector3 t1 = { r2 * cosf(a1), length, r2 * sinf(a1) };
        Vector3 n0 = Vector3Normalize(Vector3{ cosf(a0), slope, sinf(a0) });
        Vector3 n1 = Vector3Normalize(Vector3{ cosf(a1), slope, sinf(a1) });
        addVertex(&mesh, &v, b0, n0, colour);    // the side
        addVertex(&mesh, &v, t0, n0, colour);
        addVertex(&mesh, &v, b1, n1, colour);
        addVertex(&mesh, &v, b1, n1, colour);
        addVertex(&mesh, &v, t0, n0, colour);
        addVertex(&mesh, &v, t1, n1, colour);
        addVertex(&mesh, &v, Vector3{ 0, 0, 0 }, down, colour);   // the bottom end
        addVertex(&mesh, &v, b1, down, colour);
        addVertex(&mesh, &v, b0, down, colour);
        addVertex(&mesh, &v, Vector3{ 0, length, 0 }, up, colour);   // the top end
        addVertex(&mesh, &v, t0, up, colour);
        addVertex(&mesh, &v, t1, up, colour);
    }
    UploadMesh(&mesh, false);
    return mesh;
}

// Every ball starts with its number facing up, turned a different way
void resetSpin(Spin *spin) {
    for (int n = 0; n < BALL_COUNT; n++) {
        Quaternion tilt = QuaternionFromAxisAngle(Vector3{ 1, 0, 0 }, -PI / 2);
        Quaternion yaw = QuaternionFromAxisAngle(Vector3{ 0, 1, 0 }, n * 2.4f);
        spin->turn[n] = QuaternionMultiply(yaw, tilt);
        spin->known[n] = false;
    }
}

void setupLighting() {
    lighting = LoadShader("shaders/lighting.vs", "shaders/lighting.fs");
    lighting.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lighting, "viewPos");
    shineLoc = GetShaderLocation(lighting, "shine");
    shinePowerLoc = GetShaderLocation(lighting, "shinePower");

    // The lamp never moves, so it's set once
    float bulbs[9];
    for (int i = 0; i < 3; i++) {
        bulbs[i * 3 + 0] = BULBS[i].x;
        bulbs[i * 3 + 1] = BULBS[i].y;
        bulbs[i * 3 + 2] = BULBS[i].z;
    }
    SetShaderValueV(lighting, GetShaderLocation(lighting, "lampPos"), bulbs, SHADER_UNIFORM_VEC3, 3);
    Vector3 lampColour = { 1.0f, 0.84f, 0.62f };     // warm, like an old bulb
    Vector3 ambient = { 0.12f, 0.115f, 0.115f };     // the room is dim
    Vector3 fogColour = { BACKGROUND.r / 255.0f, BACKGROUND.g / 255.0f, BACKGROUND.b / 255.0f };
    float white[4] = { 1, 1, 1, 1 };
    SetShaderValue(lighting, GetShaderLocation(lighting, "lampColour"), &lampColour, SHADER_UNIFORM_VEC3);
    SetShaderValue(lighting, GetShaderLocation(lighting, "ambient"), &ambient, SHADER_UNIFORM_VEC3);
    SetShaderValue(lighting, GetShaderLocation(lighting, "fogColour"), &fogColour, SHADER_UNIFORM_VEC3);
    SetShaderValue(lighting, lighting.locs[SHADER_LOC_COLOR_DIFFUSE], white, SHADER_UNIFORM_VEC4);

    post = LoadShader(nullptr, "shaders/post.fs");   // nullptr: raylib's normal vertex shader
    texelLoc = GetShaderLocation(post, "texel");
    vignetteLoc = GetShaderLocation(post, "vignette");
    bloomLoc = GetShaderLocation(post, "bloom");
    SetShaderValue(post, post.locs[SHADER_LOC_COLOR_DIFFUSE], white, SHADER_UNIFORM_VEC4);

    // A soft shadow: black in the middle, fading to see-through at the edge
    Image spot = GenImageGradientRadial(64, 64, 0.0f, Color{ 0, 0, 0, 255 }, Color{ 0, 0, 0, 0 });
    shadowTexture = LoadTextureFromImage(spot);
    UnloadImage(spot);
    SetTextureFilter(shadowTexture, TEXTURE_FILTER_BILINEAR);
}

void setupRender3D() {
    setupLighting();

    Font font = LoadFontEx(FONT_BOLD, 64, nullptr, 0);
    ballMesh = makeSphereMesh(BALL_R, 24, 32);
    for (int n = 0; n < BALL_COUNT; n++) {
        ballMaterial[n] = LoadMaterialDefault();
        ballMaterial[n].shader = lighting;
        ballMaterial[n].maps[MATERIAL_MAP_DIFFUSE].texture = makeBallTexture(n, font);
    }
    UnloadFont(font);
    resetSpin(&gameSpin);
    resetSpin(&menuSpin);

    plainMaterial = LoadMaterialDefault();
    plainMaterial.shader = lighting;
    for (int i = 0; i < CUE_PART_COUNT; i++) {
        const CuePart *part = &CUE_PARTS[i];
        cueMesh[0][i] = makeTubeMesh(part->r1, part->r2, part->length, 16, part->colour);
        cueMesh[1][i] = makeTubeMesh(part->r1, part->r2, part->length, 16, part->golden);
    }

    newGame(&menuGame, "", "");
}

void unloadRender3D() {
    // (UnloadMaterial would also unload the shared shader, so each part is unloaded by hand)
    for (int n = 0; n < BALL_COUNT; n++) {
        UnloadTexture(ballMaterial[n].maps[MATERIAL_MAP_DIFFUSE].texture);
        MemFree(ballMaterial[n].maps);
    }
    MemFree(plainMaterial.maps);
    for (int i = 0; i < CUE_PART_COUNT; i++) {
        UnloadMesh(cueMesh[0][i]);
        UnloadMesh(cueMesh[1][i]);
    }
    UnloadMesh(ballMesh);
    UnloadTexture(shadowTexture);
    UnloadShader(lighting);
    UnloadShader(post);
    if (viewLoaded) UnloadRenderTexture(view);
    if (menuViewLoaded) UnloadRenderTexture(menuView);
}

// Where to draw a ball: where it is, or in slow motion, part of the way there from its last step
Vector2 shownPosition(Game *game, int n) {
    Ball *ball = &game->balls[n];
    if (!blending || game == &menuGame) return Vector2{ ball->x, ball->y };
    return Vector2{ lerpf(blendFrom[n].x, ball->x, blendAmount), lerpf(blendFrom[n].y, ball->y, blendAmount) };
}

void setBallBlend(const Vector2 *before, float amount) {
    for (int n = 0; n < BALL_COUNT; n++) blendFrom[n] = before[n];
    blendAmount = clamp01(amount);
    blending = true;
}

void stopBallBlend() {
    blending = false;
}

// Rolls each ball by as far as it moved since the last frame: a ball that moves
// a distance d turns by d / radius, around the axis at right angles to its movement.
void rollBalls(Game *game, Spin *spin) {
    for (int n = 0; n < BALL_COUNT; n++) {
        Ball *ball = &game->balls[n];
        if (ball->pocketed) {
            spin->known[n] = false;
            continue;
        }
        Vector2 shown = shownPosition(game, n);
        float dx = (shown.x - spin->last[n].x) * S;
        float dz = (shown.y - spin->last[n].y) * S;
        float distance = sqrtf(dx * dx + dz * dz);
        // A big jump means the ball was put back (a new game, or the cue ball after a foul)
        if (spin->known[n] && distance > 0.0001f && distance < 0.6f) {
            Vector3 axis = { dz / distance, 0, -dx / distance };
            Quaternion roll = QuaternionFromAxisAngle(axis, distance / BALL_R);
            spin->turn[n] = QuaternionNormalize(QuaternionMultiply(roll, spin->turn[n]));
        }
        spin->last[n] = shown;
        spin->known[n] = true;
    }
}

// ============================================================
// The camera
// ============================================================

// The view the camera should glide to right now
void wantedView(App *app, Vector3 *position, Vector3 *target) {
    Game *game = &app->game;
    if (cinematic) {
        // Swing slowly round the 8, low down, looking at it
        float angle = cinematicStartAngle + cinematicTime * 0.55f;
        *position = Vector3Add(cinematicFocus, Vector3{ cosf(angle) * 1.5f, 0.45f, sinf(angle) * 1.5f });
        *target = cinematicFocus;
        return;
    }
    if (topDown) {
        // From above, tilted a tiny bit so "up" is never straight along the view
        *position = Vector3{ 0, 6.0f, 0.15f };
        *target = Vector3{ 0, 0, 0 };
        return;
    }
    bool aiming = app->phase == PHASE_AIM || app->phase == PHASE_STRIKE;
    Ball *cue = &game->balls[0];
    float dx = app->aimX - cue->x;
    float dy = app->aimY - cue->y;
    float length = sqrtf(dx * dx + dy * dy);
    if (aiming && length > 0 && !cue->pocketed) {
        // Low, behind the cue ball, looking along the aim line
        Vector3 dir = { dx / length, 0, dy / length };
        Vector3 ball = toWorld(cue->x, cue->y, BALL_R);
        *position = Vector3Add(Vector3Add(ball, Vector3Scale(dir, -2.1f)), Vector3{ 0, 0.72f, 0 });
        *target = Vector3Add(ball, Vector3Scale(dir, 1.3f));
        return;
    }
    // Angled, looking down at the whole table
    *position = Vector3{ 0, 4.7f, 4.1f };
    *target = Vector3{ 0, -0.45f, 0.2f };
}

void updateCamera(App *app, float dt) {
    Vector3 position, target;
    wantedView(app, &position, &target);
    if (!cameraReady) {
        camera.position = position;
        camera.target = target;
        camera.up = Vector3{ 0, 1, 0 };
        camera.fovy = 40;
        camera.projection = CAMERA_PERSPECTIVE;
        cameraReady = true;
    }
    // Glide towards the wanted view (never jump), a bit faster for the film-like swing
    float speed = cinematic ? 5.0f : 3.5f;
    camera.position = smoothTowards3(camera.position, position, speed, dt);
    camera.target = smoothTowards3(camera.target, target, speed, dt);
    if (cinematic) cinematicTime += dt;
}

void setCinematicCamera(bool on, float x, float y) {
    if (on) {
        Vector3 focus = toWorld(x, y, BALL_R);
        if (!cinematic) {
            // Start the swing from where the camera is now
            cinematicTime = 0;
            cinematicStartAngle = atan2f(camera.position.z - focus.z, camera.position.x - focus.x);
        }
        cinematicFocus = focus;
    }
    cinematic = on;
}

// The camera with the shake added (the shake isn't kept, so it never builds up)
Camera3D shakenCamera() {
    Vector2 shake = getShake();
    Camera3D shaken = camera;
    shaken.target = Vector3Add(camera.target, Vector3{ shake.x * 0.12f, shake.y * 0.08f, 0 });
    shaken.position = Vector3Add(camera.position, Vector3{ shake.x * 0.04f, shake.y * 0.04f, 0 });
    return shaken;
}

// The menu's camera circles slowly round the table. It looks a little to the
// left of the table, so the table sits on the right (the menu is on the left).
void updateMenuCamera(float dt) {
    menuAngle += 0.12f * dt;
    float radius = 8.0f;
    menuCamera.position = Vector3{ cosf(menuAngle) * radius, 2.2f, sinf(menuAngle) * radius };
    Vector3 forward = Vector3Normalize(Vector3Negate(menuCamera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, Vector3{ 0, 1, 0 }));
    menuCamera.target = Vector3Add(Vector3{ 0, 0.1f, 0 }, Vector3Scale(right, -2.4f));
    menuCamera.up = Vector3{ 0, 1, 0 };
    menuCamera.fovy = 42;
    menuCamera.projection = CAMERA_PERSPECTIVE;
}

void toggleTopDownView() {
    topDown = !topDown;
}

bool isTopDownView() {
    return topDown;
}

// ============================================================
// Drawing the table
// ============================================================

// Sets how shiny the next lit things are (0 = matte like felt, 1 = shiny like a ball).
// raylib collects shapes in a "batch" and draws them all at once, so the waiting
// shapes are drawn first, with the old shine, before it changes.
void setShine(float shine, float power) {
    rlDrawRenderBatchActive();
    SetShaderValue(lighting, shineLoc, &shine, SHADER_UNIFORM_FLOAT);
    SetShaderValue(lighting, shinePowerLoc, &power, SHADER_UNIFORM_FLOAT);
}

// A box from corner (x1, y1, z1) to (x2, y2, z2), with a lighter top so its edges show
void box(float x1, float y1, float z1, float x2, float y2, float z2, Color side, Color top) {
    Vector3 centre = { (x1 + x2) / 2, (y1 + y2) / 2, (z1 + z2) / 2 };
    Vector3 size = { x2 - x1, y2 - y1, z2 - z1 };
    DrawCubeV(centre, size, side);
    DrawCubeV(Vector3{ centre.x, y2 + 0.0005f, centre.z }, Vector3{ size.x, 0.001f, size.z }, top);
}

// The six inner strips round the felt (green cushion next to the felt, wood behind it).
// They stop at each pocket, leaving a gap.
struct Strip {
    float x1, z1, x2, z2;
    bool alongX;      // along the long side?
    float outward;    // -1 or +1: which way is away from the felt
};
const Strip STRIPS[6] = {
    { -HALF_W + NOTCH, -HALF_H - NOTCH, -NOTCH, -HALF_H, true, -1 },          // top left
    { NOTCH, -HALF_H - NOTCH, HALF_W - NOTCH, -HALF_H, true, -1 },            // top right
    { -HALF_W + NOTCH, HALF_H, -NOTCH, HALF_H + NOTCH, true, 1 },             // bottom left
    { NOTCH, HALF_H, HALF_W - NOTCH, HALF_H + NOTCH, true, 1 },               // bottom right
    { -HALF_W - NOTCH, -HALF_H + NOTCH, -HALF_W, HALF_H - NOTCH, false, -1 }, // left
    { HALF_W, -HALF_H + NOTCH, HALF_W + NOTCH, HALF_H - NOTCH, false, 1 },    // right
};

// Draws a strip's cushion, or its wood
void drawStrip(Strip s, bool wood) {
    float edge;   // the side touching the felt
    if (s.alongX) edge = s.outward < 0 ? s.z2 : s.z1;
    else edge = s.outward < 0 ? s.x2 : s.x1;
    float a = wood ? edge + s.outward * CUSHION_W : edge;
    float b = wood ? edge + s.outward * NOTCH : edge + s.outward * CUSHION_W;
    float top = wood ? RAIL_TOP : CUSHION_TOP;
    Color side = wood ? WOOD_SIDE : CUSHION;
    Color topColour = wood ? WOOD_TOP : CUSHION_TOP_COLOUR;
    if (s.alongX) box(s.x1, 0, fminf(a, b), s.x2, top, fmaxf(a, b), side, topColour);
    else box(fminf(a, b), 0, s.z1, fmaxf(a, b), top, s.z2, side, topColour);
}

// The table and the floor, lit by the lamp
void drawTableBody() {
    // Matte things first: the floor, the felt and the rubber cushions
    setShine(0, 1);
    DrawPlane(Vector3{ 0, FLOOR_Y, 0 }, Vector2{ 40, 40 }, FLOOR_COLOUR);
    box(-HALF_W, -0.05f, -HALF_H, HALF_W, 0, HALF_H, FELT, FELT);
    for (int i = 0; i < 6; i++) drawStrip(STRIPS[i], false);

    // Then the varnished wood, which is a little shiny
    setShine(0.12f, 20);
    for (int sx = -1; sx <= 1; sx += 2) {
        for (int sz = -1; sz <= 1; sz += 2) {
            float x = sx * 2.9f, z = sz * 1.4f;
            box(x - 0.15f, FLOOR_Y, z - 0.15f, x + 0.15f, -0.38f, z + 0.15f, WOOD_SIDE, WOOD_SIDE);   // a leg
        }
    }
    box(-HALF_W - RAIL_W, -0.38f, -HALF_H - RAIL_W, HALF_W + RAIL_W, -0.001f, HALF_H + RAIL_W, WOOD_SIDE, BODY_TOP);
    // The outer part of the rails goes all the way round
    box(-HALF_W - RAIL_W, 0, -HALF_H - RAIL_W, HALF_W + RAIL_W, RAIL_TOP, -HALF_H - NOTCH, WOOD_SIDE, WOOD_TOP);
    box(-HALF_W - RAIL_W, 0, HALF_H + NOTCH, HALF_W + RAIL_W, RAIL_TOP, HALF_H + RAIL_W, WOOD_SIDE, WOOD_TOP);
    box(-HALF_W - RAIL_W, 0, -HALF_H - NOTCH, -HALF_W - NOTCH, RAIL_TOP, HALF_H + NOTCH, WOOD_SIDE, WOOD_TOP);
    box(HALF_W + NOTCH, 0, -HALF_H - NOTCH, HALF_W + RAIL_W, RAIL_TOP, HALF_H + NOTCH, WOOD_SIDE, WOOD_TOP);
    for (int i = 0; i < 6; i++) drawStrip(STRIPS[i], true);
}

// The flat details on the table, drawn without lighting: pocket holes and diamonds
void drawTableMarks() {
    for (int p = 0; p < 6; p++) {
        Vector3 centre = toWorld(pocketX[p], pocketY[p], 0.002f);
        DrawCylinder(centre, NOTCH, NOTCH, 0.002f, 28, Color{ 22, 18, 16, 255 });
        centre.y = 0.0045f;
        DrawCylinder(centre, NOTCH * 0.82f, NOTCH * 0.82f, 0.001f, 28, BLACK);
    }
    // Diamonds (sighting marks) on top of the rails
    float railMiddle = (NOTCH + RAIL_W) / 2;
    for (int i = 1; i < 8; i++) {
        if (i == 4) continue;   // the middle pocket is there
        float x = -HALF_W + FELT_WIDTH * S * i / 8;
        DrawCylinder(Vector3{ x, RAIL_TOP + 0.001f, -HALF_H - railMiddle }, 0.022f, 0.022f, 0.002f, 10, DIAMOND);
        DrawCylinder(Vector3{ x, RAIL_TOP + 0.001f, HALF_H + railMiddle }, 0.022f, 0.022f, 0.002f, 10, DIAMOND);
    }
    for (int i = 1; i < 4; i++) {
        float z = -HALF_H + FELT_HEIGHT * S * i / 4;
        DrawCylinder(Vector3{ -HALF_W - railMiddle, RAIL_TOP + 0.001f, z }, 0.022f, 0.022f, 0.002f, 10, DIAMOND);
        DrawCylinder(Vector3{ HALF_W + railMiddle, RAIL_TOP + 0.001f, z }, 0.022f, 0.022f, 0.002f, 10, DIAMOND);
    }
}

// The lamp's dark green shade (lit, so it's drawn with the table)
void drawLampShade() {
    setShine(0.6f, 40);
    box(-1.95f, LAMP_Y, -0.3f, 1.95f, LAMP_Y + 0.22f, 0.3f, LAMP_SHADE, LAMP_SHADE);
}

// The inside of the shade glows (it IS the light, so it isn't lit), and two cords hold it up
void drawLampGlow() {
    DrawCubeV(Vector3{ 0, LAMP_Y - 0.004f, 0 }, Vector3{ 3.8f, 0.008f, 0.5f }, LAMP_GLOW);
    for (int side = -1; side <= 1; side += 2) {
        DrawCylinderEx(Vector3{ side * 1.3f, LAMP_Y + 0.22f, 0 }, Vector3{ side * 1.3f, 9, 0 }, 0.01f, 0.01f, 5, Color{ 18, 16, 15, 255 });
    }
}

// A soft dark spot lying flat (the shadow texture on a flat square)
void drawShadowSpot(float x, float z, float y, float sizeX, float sizeZ, float alpha) {
    rlSetTexture(shadowTexture.id);
    rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, (unsigned char)(alpha * 255));
    rlNormal3f(0, 1, 0);
    rlTexCoord2f(0, 0); rlVertex3f(x - sizeX / 2, y, z - sizeZ / 2);
    rlTexCoord2f(0, 1); rlVertex3f(x - sizeX / 2, y, z + sizeZ / 2);
    rlTexCoord2f(1, 1); rlVertex3f(x + sizeX / 2, y, z + sizeZ / 2);
    rlTexCoord2f(1, 0); rlVertex3f(x + sizeX / 2, y, z - sizeZ / 2);
    rlEnd();
    rlSetTexture(0);
}

// Shadows: a big soft one under the table, and two under each ball (a soft one pushed a
// little away from the nearest bulb, and a dark one right where the ball touches the felt)
void drawShadows(Game *game) {
    rlDrawRenderBatchActive();
    rlDisableDepthMask();   // they're see-through, so they mustn't hide each other
    drawShadowSpot(0, 0, FLOOR_Y + 0.003f, 10.0f, 6.0f, 0.95f);
    for (int n = 0; n < BALL_COUNT; n++) {
        Ball *ball = &game->balls[n];
        if (ball->pocketed) continue;
        Vector2 shown = shownPosition(game, n);
        Vector3 p = toWorld(shown.x, shown.y, 0);
        float bulbX = BULBS[0].x;
        for (int i = 1; i < 3; i++) {
            if (fabsf(p.x - BULBS[i].x) < fabsf(p.x - bulbX)) bulbX = BULBS[i].x;
        }
        drawShadowSpot(p.x + (p.x - bulbX) * 0.08f, p.z * 1.08f, 0.003f, BALL_R * 3.4f, BALL_R * 3.4f, 0.5f);
        drawShadowSpot(p.x, p.z, 0.0035f, BALL_R * 1.6f, BALL_R * 1.6f, 0.6f);
    }
    rlDrawRenderBatchActive();
    rlEnableDepthMask();
}

// ============================================================
// Drawing the balls, the cue and the aim guide
// ============================================================

void drawBalls3D(Game *game, Spin *spin) {
    for (int n = 0; n < BALL_COUNT; n++) {
        Ball *ball = &game->balls[n];
        Vector3 position;
        if (!ball->pocketed) {
            Vector2 shown = shownPosition(game, n);
            position = toWorld(shown.x, shown.y, BALL_R);
        } else if (game->sinkTimer[n] > 0) {
            // Going down a pocket: slide to the middle of the hole, then drop
            float timer = game->sinkTimer[n];
            if (blending && game != &menuGame) timer += (1 - blendAmount) * PHYSICS_STEP_TIME;   // smooth in slow motion
            float t = clamp01(1 - timer / 0.35f);
            float x = lerpf(game->sinkFromX[n], game->sinkToX[n], easeOutCubic(t));
            float y = lerpf(game->sinkFromY[n], game->sinkToY[n], easeOutCubic(t));
            float drop = clamp01((t - 0.3f) / 0.7f);
            position = toWorld(x, y, BALL_R - drop * drop * 0.4f);
        } else {
            continue;   // already gone
        }
        Matrix transform = MatrixMultiply(QuaternionToMatrix(spin->turn[n]), MatrixTranslate(position.x, position.y, position.z));
        DrawMesh(ballMesh, ballMaterial[n], transform);
    }
}

// The cue stick: tip at the cue ball, rising gently towards the player.
// It's pulled back further for more power, and slides forward to strike.
void drawCue3D(Game *game, float aimX, float aimY, float power, float strike, bool golden) {
    Ball *cue = &game->balls[0];
    float dx = aimX - cue->x;
    float dy = aimY - cue->y;
    float length = sqrtf(dx * dx + dy * dy);
    if (length == 0 || cue->pocketed) return;
    Vector3 dir = { dx / length, 0, dy / length };

    // Same pull-back as the 2D stick, in table pixels
    float pull = (BALL_RADIUS + 6 + power * 0.6f) * (1 - strike) + BALL_RADIUS * strike;
    Vector3 ball = toWorld(cue->x, cue->y, BALL_R);
    Vector3 tip = Vector3Add(ball, Vector3Scale(dir, -pull * S));
    float tilt = 6 * DEG2RAD;
    Vector3 back = { -dir.x * cosf(tilt), sinf(tilt), -dir.z * cosf(tilt) };   // from the tip towards the butt

    // Each part is a tube along y, so it's turned to point along 'back' and moved to where it starts
    Matrix turn = QuaternionToMatrix(QuaternionFromVector3ToVector3(Vector3{ 0, 1, 0 }, back));
    Vector3 start = tip;
    setShine(0.35f, 36);
    rlDisableBackfaceCulling();
    for (int i = 0; i < CUE_PART_COUNT; i++) {
        Matrix transform = MatrixMultiply(turn, MatrixTranslate(start.x, start.y, start.z));
        DrawMesh(cueMesh[golden ? 1 : 0][i], plainMaterial, transform);
        start = Vector3Add(start, Vector3Scale(back, CUE_PARTS[i].length));
    }
    rlEnableBackfaceCulling();
}

// Dots on the felt up to where the cue ball will hit, a ghost ball there (an
// upgrade), and a yellow line showing where the object ball will go (an upgrade)
void drawAimGuide3D(Game *game, float aimX, float aimY, bool showGhost, bool showPath) {
    Ball *cue = &game->balls[0];
    float dx, dy, hitDistance;
    int hitBall;
    if (cue->pocketed || !aimContact(game, aimX, aimY, &dx, &dy, &hitDistance, &hitBall)) return;

    for (float d = BALL_RADIUS + 8; d < hitDistance - 4; d += 14) {
        Vector3 dot = toWorld(cue->x + dx * d, cue->y + dy * d, 0.004f);
        DrawCylinder(dot, 0.012f, 0.012f, 0.001f, 8, Fade(WHITE, 0.75f));
    }
    float gx = cue->x + dx * hitDistance;
    float gy = cue->y + dy * hitDistance;
    if (showPath && hitBall != -1) {
        Ball *ball = &game->balls[hitBall];
        float px = ball->x - gx;
        float py = ball->y - gy;
        float pl = sqrtf(px * px + py * py);
        if (pl > 0) {
            Vector3 from = toWorld(ball->x, ball->y, 0.005f);
            Vector3 to = toWorld(ball->x + px / pl * 110, ball->y + py / pl * 110, 0.005f);
            DrawCylinderEx(from, to, 0.01f, 0.01f, 6, Fade(YELLOW, 0.8f));
        }
    }
    if (showGhost) {
        Vector3 ghost = toWorld(gx, gy, BALL_R);
        DrawCircle3D(toWorld(gx, gy, 0.006f), BALL_R, Vector3{ 1, 0, 0 }, 90, Fade(WHITE, 0.9f));
        DrawSphereEx(ghost, BALL_R, 10, 14, Fade(WHITE, 0.2f));   // see-through, so it's drawn last
    }
}

// ---------- Glowing trails behind fast balls ----------

void updateTrails(Game *game) {
    for (int n = 0; n < BALL_COUNT; n++) {
        if (game->balls[n].pocketed) {
            trailCount[n] = 0;
            continue;
        }
        Vector2 shown = shownPosition(game, n);
        Vector3 now = toWorld(shown.x, shown.y, BALL_R);
        if (trailCount[n] > 0) {
            float moved = Vector3Distance(now, trail[n][0]);
            if (moved > 0.6f) trailCount[n] = 0;   // put back on the table: start a new trail
            else if (moved < 0.0005f) {            // standing still: the trail shrinks away
                trailCount[n]--;
                continue;
            }
        }
        // Push the newest position on the front
        int last = trailCount[n] < TRAIL_LENGTH ? trailCount[n] : TRAIL_LENGTH - 1;
        for (int i = last; i > 0; i--) trail[n][i] = trail[n][i - 1];
        trail[n][0] = now;
        if (trailCount[n] < TRAIL_LENGTH) trailCount[n]++;
    }
}

// Each trail is a flat ribbon in the ball's colour, thinner and fainter towards its end.
// Only fast balls have one: a slow ball moves too little between frames for it to show.
void drawTrails(Game *game) {
    rlDrawRenderBatchActive();
    rlDisableDepthMask();
    rlDisableBackfaceCulling();
    BeginBlendMode(BLEND_ADDITIVE);   // glowing: the colour is added on top
    for (int n = 0; n < BALL_COUNT; n++) {
        int count = trailCount[n];
        if (count < 2 || game->balls[n].pocketed) continue;
        Color colour = ballColours[n % 8];
        if (n == 0) colour = WHITE;
        if (n == 8) colour = Color{ 170, 120, 255, 255 };
        rlBegin(RL_QUADS);
        for (int i = 0; i < count - 1; i++) {
            Vector3 a = trail[n][i];
            Vector3 b = trail[n][i + 1];
            float length = Vector3Distance(a, b);
            float fast = clamp01((length - 0.03f) / 0.08f);   // 0 below about 180 px/s, 1 above about 650
            if (fast <= 0 || length <= 0) continue;
            float t0 = 1 - (float)i / count;
            float t1 = 1 - (float)(i + 1) / count;
            Vector3 side = { -(b.z - a.z) / length, 0, (b.x - a.x) / length };   // at right angles, flat on the table
            float w0 = BALL_R * 0.85f * t0;
            float w1 = BALL_R * 0.85f * t1;
            rlColor4ub(colour.r, colour.g, colour.b, (unsigned char)(fast * t0 * 140));
            rlVertex3f(a.x + side.x * w0, a.y, a.z + side.z * w0);
            rlVertex3f(a.x - side.x * w0, a.y, a.z - side.z * w0);
            rlVertex3f(b.x - side.x * w1, b.y, b.z - side.z * w1);
            rlVertex3f(b.x + side.x * w1, b.y, b.z + side.z * w1);
        }
        rlEnd();
    }
    EndBlendMode();
    rlEnableBackfaceCulling();
    rlEnableDepthMask();
}

// A pulsing yellow ring on the felt round the ball that target points at
void drawTargetRing3D(Game *game) {
    if (game->calledBall < 1) return;
    Ball *target = &game->balls[game->calledBall];
    if (target->pocketed) return;
    float pulse = 0.02f + 0.02f * sinf((float)GetTime() * 6);
    Vector3 centre = toWorld(target->x, target->y, 0.007f);
    for (int k = 0; k < 3; k++) {
        DrawCircle3D(centre, BALL_R + 0.03f + pulse + k * 0.005f, Vector3{ 1, 0, 0 }, 90, YELLOW);
    }
}

// The table, its lamp, the balls and their shadows (the same for the game and the menu)
void drawScene(Game *game, Spin *spin, Camera3D *cam, bool showLamp) {
    SetShaderValue(lighting, lighting.locs[SHADER_LOC_VECTOR_VIEW], &cam->position, SHADER_UNIFORM_VEC3);
    BeginShaderMode(lighting);      // everything solid is lit by the lamp
    drawTableBody();
    if (showLamp) drawLampShade();
    setShine(0.7f, 160);            // balls are very shiny: small, sharp highlights
    drawBalls3D(game, spin);
    EndShaderMode();

    drawTableMarks();
    if (showLamp) drawLampGlow();
    drawShadows(game);
}

// ============================================================
// Each frame
// ============================================================

// Makes an image the size of 'width' x 'height' game pixels on the real screen (sharp at any window size)
void fitTexture(RenderTexture2D *picture, bool *loaded, float width, float height) {
    Vector2 dpi = GetWindowScaleDPI();
    int w = (int)(width * getViewScale() * dpi.x);
    int h = (int)(height * getViewScale() * dpi.y);
    if (w < 16) w = 16;
    if (h < 16) h = 16;
    if (*loaded && picture->texture.width == w && picture->texture.height == h) return;
    if (*loaded) UnloadRenderTexture(*picture);
    *picture = LoadRenderTexture(w, h);
    SetTextureFilter(picture->texture, TEXTURE_FILTER_BILINEAR);
    *loaded = true;
}

void renderTable3D(App *app, float dt) {
    Game *game = &app->game;
    fitTexture(&view, &viewLoaded, TABLE_VIEW.width, TABLE_VIEW.height);
    updateCamera(app, dt);
    rollBalls(game, &gameSpin);
    updateTrails(game);

    Camera3D shaken = shakenCamera();
    BeginTextureMode(view);
    ClearBackground(BACKGROUND);
    BeginMode3D(shaken);
    drawScene(game, &gameSpin, &shaken, camera.position.y < 4.0f);   // the lamp would block the view from high up
    drawTrails(game);

    bool aiming = app->phase == PHASE_AIM || app->phase == PHASE_STRIKE;
    if (aiming) {
        float strike = 0;
        if (app->phase == PHASE_STRIKE) strike = app->strikeTimer / 0.09f;
        BeginShaderMode(lighting);
        drawCue3D(game, app->aimX, app->aimY, app->power, strike, app->learner->levelsDone >= GOLD_CUE_LEVEL);
        EndShaderMode();
    }
    drawTargetRing3D(game);
    if (app->phase == PHASE_AIM) {
        drawAimGuide3D(game, app->aimX, app->aimY, app->learner->levelsDone >= AIM_GUIDE_LEVEL,
                       app->learner->levelsDone >= BALL_PATH_LEVEL);
    }
    drawParticles3D();   // chalk dust, sparks and pot bursts (fx.cpp)
    EndMode3D();
    EndTextureMode();
}

// The menu's balls: every so often the cue ball is hit at a random ball, using the game's own physics
void updateMenuGame(float dt) {
    tableEffects = false;   // no sounds or sparks from the menu's table
    updateBalls(&menuGame, dt);
    tableEffects = true;
    if (ballsMoving(&menuGame)) return;

    menuWait -= dt;
    if (menuWait > 0) return;
    menuWait = 1.4f;

    // After a few shots (or if the cue ball went down) rack them up again
    int left = 0;
    for (int n = 1; n < BALL_COUNT; n++) {
        if (!menuGame.balls[n].pocketed) left++;
    }
    if (menuGame.balls[0].pocketed || left < 6 || menuShots >= 7) {
        newGame(&menuGame, "", "");
        menuShots = 0;
        return;
    }
    int target = GetRandomValue(1, BALL_COUNT - 1);
    while (menuGame.balls[target].pocketed) target = GetRandomValue(1, BALL_COUNT - 1);
    float power = menuShots == 0 ? 100.0f : (float)GetRandomValue(45, 85);   // the first one is a break
    startShot(&menuGame, menuGame.balls[target].x, menuGame.balls[target].y, power);
    menuShots++;
}

void renderMenu3D(float dt) {
    fitTexture(&menuView, &menuViewLoaded, SCREEN_WIDTH, SCREEN_HEIGHT);
    updateMenuGame(dt);
    updateMenuCamera(dt);
    rollBalls(&menuGame, &menuSpin);

    BeginTextureMode(menuView);
    ClearBackground(BACKGROUND);
    BeginMode3D(menuCamera);
    drawScene(&menuGame, &menuSpin, &menuCamera, true);
    EndMode3D();
    EndTextureMode();
}

// Puts a picture of a 3D scene on the screen through the post-processing shader
void drawWithPost(RenderTexture2D picture, Rectangle area, float vignette) {
    float w = (float)picture.texture.width;
    float h = (float)picture.texture.height;
    float step = h / area.height * 2.4f;    // the glow's samples are 2.4 game pixels apart, at any resolution
    Vector2 texel = { step / w, step / h };
    float bloom = 0.9f;
    SetShaderValue(post, texelLoc, &texel, SHADER_UNIFORM_VEC2);
    SetShaderValue(post, vignetteLoc, &vignette, SHADER_UNIFORM_FLOAT);
    SetShaderValue(post, bloomLoc, &bloom, SHADER_UNIFORM_FLOAT);
    BeginShaderMode(post);
    // Render textures are stored upside down, so the source height is negative
    DrawTexturePro(picture.texture, Rectangle{ 0, 0, w, -h }, area, Vector2{ 0, 0 }, 0, WHITE);
    EndShaderMode();
}

void drawTableView() {
    if (!viewLoaded) return;
    drawWithPost(view, TABLE_VIEW, 0.5f);

    // Black bars slide in for the film-like slow motion
    filmBars = smoothTowards(filmBars, cinematic ? 1.0f : 0.0f, 6, GetFrameTime());
    if (filmBars > 0.01f) {
        float bar = 34 * filmBars;
        DrawRectangleRec(Rectangle{ TABLE_VIEW.x, TABLE_VIEW.y, TABLE_VIEW.width, bar }, BLACK);
        DrawRectangleRec(Rectangle{ TABLE_VIEW.x, TABLE_VIEW.y + TABLE_VIEW.height - bar, TABLE_VIEW.width, bar }, BLACK);
    }
    const char *label = topDown ? "T: 3D view" : "T: top-down view";
    drawText(label, TABLE_VIEW.x + 12, TABLE_VIEW.y + 8, 13, Fade(COLOUR_TEXT, 0.55f));
}

void drawMenuView() {
    if (!menuViewLoaded) return;
    drawWithPost(menuView, Rectangle{ 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT }, 0.65f);
}

// ============================================================
// The mouse and the 3D table
// ============================================================

bool mouseOverTableView() {
    return CheckCollisionPointRec(GetMousePosition(), TABLE_VIEW);
}

// A ray from the camera through the mouse, into the 3D scene
bool mouseRay(Ray *ray) {
    if (!viewLoaded || !cameraReady || !mouseOverTableView()) return false;
    Vector2 mouse = GetMousePosition();
    float w = (float)view.texture.width;
    float h = (float)view.texture.height;
    Vector2 pixel = { (mouse.x - TABLE_VIEW.x) / TABLE_VIEW.width * w, (mouse.y - TABLE_VIEW.y) / TABLE_VIEW.height * h };
    *ray = GetScreenToWorldRayEx(pixel, camera, (int)w, (int)h);
    return true;
}

// Where the mouse ray meets the flat plane through the middle of the balls
bool mouseOnTable(Vector2 *point) {
    Ray ray;
    if (!mouseRay(&ray) || ray.direction.y >= -0.0001f) return false;
    float t = (BALL_R - ray.position.y) / ray.direction.y;
    if (t <= 0) return false;
    float wx = ray.position.x + ray.direction.x * t;
    float wz = ray.position.z + ray.direction.z * t;
    point->x = wx / S + CENTER_X;
    point->y = wz / S + CENTER_Y;
    return true;
}

int ballUnderMouse(Game *game) {
    Ray ray;
    if (!mouseRay(&ray)) return -1;
    int best = -1;
    float bestDistance = 1e9f;
    for (int n = 0; n < BALL_COUNT; n++) {
        Ball *ball = &game->balls[n];
        if (ball->pocketed) continue;
        RayCollision hit = GetRayCollisionSphere(ray, toWorld(ball->x, ball->y, BALL_R), BALL_R);
        if (hit.hit && hit.distance < bestDistance) {
            bestDistance = hit.distance;
            best = n;
        }
    }
    return best;
}

Vector3 tableToWorld(float x, float y, float height) {
    return toWorld(x, y, height * S);
}

Vector2 tableToScreen(float x, float y, float height) {
    if (!viewLoaded || !cameraReady) return Vector2{ x, y };
    float w = (float)view.texture.width;
    float h = (float)view.texture.height;
    Vector2 pixel = GetWorldToScreenEx(toWorld(x, y, height * S), camera, (int)w, (int)h);
    return Vector2{ TABLE_VIEW.x + pixel.x / w * TABLE_VIEW.width, TABLE_VIEW.y + pixel.y / h * TABLE_VIEW.height };
}
