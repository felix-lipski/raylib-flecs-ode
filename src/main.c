#include "raylib.h"
#include "header/config.h"
#include "header/components.h"
#include "ecs/flecs.h"

Camera3D camera = { 0 };
Vector3 cubePosition = { 100.0f, 0.0f, 100.0f };

Color bgColor = { 18, 18, 18 };

void BeforeDrawing(ecs_iter_t *it){
    ClearBackground(bgColor);
    DrawFPS(20,20);
    UpdateCamera(&camera);
    BeginMode3D(camera);
    DrawGrid(100, 1.0f);
}

void AfterDrawing(ecs_iter_t *it){
    EndMode3D();
    EndDrawing();
}

float speed = 0.05;
void Move(ecs_iter_t *it){
    Position2 *p = ecs_column(it, Position2, 1); 

    for (int i = 0; i < it->count; i++)
    {   
        if (IsKeyDown(KEY_UP))
            p[i].y-=speed;
        if (IsKeyDown(KEY_DOWN))
            p[i].y+=speed;
        if (IsKeyDown(KEY_LEFT))
            p[i].x-=speed;
        if (IsKeyDown(KEY_RIGHT))
            p[i].x+=speed;
    }
}

void DrawSquare(ecs_iter_t *it){
    Position2 *p = ecs_column(it, Position2, 1); 
    ColorType *c = ecs_column(it, ColorType, 3);
    for (int i = 0; i < it->count; i++)
    {        
        Vector3 cubePosition = { p[i].x, 1.0f, p[i].y };
        DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, c[i].color);
        DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, WHITE);
    }
}

int main(void)
{
    InitWindow(1920, 1080, "raylib-flecs");
    SetTargetFPS(60);

    camera.position = (Vector3){ 0.0f, 1.5f, 0.0f };
    camera.target = (Vector3){ 0.0f, 1.5f, 10.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;  

    SetCameraMode(camera, CAMERA_FIRST_PERSON);


    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position2);
    ECS_COMPONENT(world, Size2);
    ECS_COMPONENT(world, ColorType);

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

    ECS_ENTITY(world, TestEntity, Position2, Size2, ColorType);
    ecs_set(world, TestEntity, Position2, {10, -10});
    ecs_set(world, TestEntity, Size2, {100, 100});
    ecs_set(world, TestEntity, ColorType, {DARKGREEN});

    ECS_ENTITY(world, TestEntity2, Position2, Size2, ColorType);
    ecs_set(world, TestEntity2, Position2, {-10, -15});
    ecs_set(world, TestEntity2, Size2, {100, 100});
    ecs_set(world, TestEntity2, ColorType, {DARKBLUE});

    ECS_SYSTEM(world, Move, OnUpdate, Position2);
    ECS_SYSTEM(world,BeforeDrawing,PreDraw,RENDERER);
    ECS_SYSTEM(world,DrawSquare, OnDraw, Position2, Size2, ColorType);
    ECS_SYSTEM(world,AfterDrawing,PostDraw,RENDERER);

    while (ecs_progress(world, 0) && !WindowShouldClose());
    CloseWindow();         
    return ecs_fini(world);
}
