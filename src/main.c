#include "raylib.h"
#include "header/config.h"
#include "header/components.h"
#include "ecs/flecs.h"

Camera3D camera;

void BeforeDrawing(ecs_iter_t *it) {
    ClearBackground((Color){ 18, 18, 18 });
    DrawFPS(20,20);
    UpdateCamera(&camera);
    BeginMode3D(camera);
    DrawGrid(100, 1.0f);
}

void AfterDrawing(ecs_iter_t *it) {
    EndMode3D();
    EndDrawing();
}

float speed = 0.05;
void Move(ecs_iter_t *it) {
    Position3 *p = ecs_column(it, Position3, 1); 

    for (int i = 0; i < it->count; i++) {   
        if (IsKeyDown(KEY_UP))      p[i].y -= speed;
        if (IsKeyDown(KEY_DOWN))    p[i].y += speed;
        if (IsKeyDown(KEY_LEFT))    p[i].x -= speed;
        if (IsKeyDown(KEY_RIGHT))   p[i].x += speed;
    }
}

void DrawCubeSystem(ecs_iter_t *it) {
    Position3 *p = ecs_column(it, Position3, 1); 
    ColorType *c = ecs_column(it, ColorType, 2);
    for (int i = 0; i < it->count; i++) {        
        DrawCube(p[i], 2.0f, 2.0f, 2.0f, c[i].color);
        DrawCubeWires(p[i], 2.0f, 2.0f, 2.0f, WHITE);
    }
}

void DrawModels(ecs_iter_t *it) {
    Position3 *p = ecs_column(it, Position3, 1); 
    Render *r    = ecs_column(it, Render, 2);
    for (int i = 0; i < it->count; i++) {        
        DrawModel(r[i].model, p[i], 1.0f, WHITE);
    }
}

int main(void) {
    InitWindow(1920, 1080, "raylib-flecs");
    SetTargetFPS(60);

    camera.position = (Vector3){ 0.0f, 1.5f, 0.0f  };
    camera.target   = (Vector3){ 0.0f, 1.5f, 10.0f };
    camera.up       = (Vector3){ 0.0f, 1.0f, 0.0f  };
    camera.fovy     = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;  

    SetCameraMode(camera, CAMERA_FIRST_PERSON);


    Model model = LoadModel("res/yellow-beam-5x.obj");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("res/yellow-beam-texture.png");


    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position3);
    ECS_COMPONENT(world, ColorType);
    ECS_COMPONENT(world, Render);

    ECS_TAG(world, PreUpdate);
    ECS_TAG(world, OnUpdate);
    ECS_TAG(world, PostUpdate);
    ECS_TAG(world, PreDraw);
    ECS_TAG(world, OnDraw);
    ECS_TAG(world, PostDraw);

    ECS_PIPELINE(world,CustomPipeline,PreUpdate,OnUpdate,PostUpdate,PreDraw,OnDraw,PostDraw);

    ecs_set_pipeline(world,CustomPipeline);

    ECS_TAG(world, RENDERER);
    ECS_ENTITY(world, RenderEntity, RENDERER);

    ECS_ENTITY(world, Cube1, Position3, ColorType);
    ecs_set(   world, Cube1, Position3, {10, 1, -10});
    ecs_set(   world, Cube1, ColorType, {DARKGREEN});

    ECS_ENTITY(world, Cube2, Position3, ColorType);
    ecs_set(   world, Cube2, Position3, {-10, 1, -15});
    ecs_set(   world, Cube2, ColorType, {DARKBLUE});

    ECS_ENTITY(world, Beams, Position3, Render);
    ecs_set(   world, Beams, Position3, {0, 0, -15});
    ecs_set(   world, Beams, Render,    {model});

    ECS_SYSTEM(world, Move, OnUpdate, Position3);
    ECS_SYSTEM(world, BeforeDrawing, PreDraw, RENDERER);
    ECS_SYSTEM(world, DrawCubeSystem, OnDraw, Position3, ColorType);
    ECS_SYSTEM(world, DrawModels, OnDraw, Position3, Render);
    ECS_SYSTEM(world, AfterDrawing, PostDraw, RENDERER);

    while (ecs_progress(world, 0) && !WindowShouldClose());
    CloseWindow();         
    return ecs_fini(world);
}
