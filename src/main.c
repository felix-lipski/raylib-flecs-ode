#include "raylib.h"
#include "header/config.h"
#include "header/components.h"
#include "ecs/flecs.h"

#include <ode/ode.h>

#define RLIGHTS_IMPLEMENTATION
#include "header/rlights.h"
#include "header/physics.h"

Camera3D camera;

void BeforeDrawing(ecs_iter_t *it) {
    /* ClearBackground((Color){ 18, 18, 18 }); */
    ClearBackground(DARKBLUE);
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

void DrawModels(ecs_iter_t *it) {
    Position3 *p = ecs_column(it, Position3, 1); 
    Render *r    = ecs_column(it, Render, 2);
    for (int i = 0; i < it->count; i++) {        
        DrawModel(r[i].model, p[i], 1.0f, ORANGE);
        DrawModelWires(r[i].model, (Vector3){0,0,0}, 1.0f, BLACK);
    }
}

void ApplyPhysics(ecs_iter_t *it) {
    dSpaceCollide(space, 0, &nearCallback);
    dWorldQuickStep(world, 1. / 60.0);
    dJointGroupEmpty(contactgroup);

    Render *r     = ecs_column(it, Render, 1);
    Physics *p    = ecs_column(it, Physics, 2);

    for (int i = 0; i < it->count; i++) {        
        dBodyID bid = p[i].bid;
        float* pos = (float *) dBodyGetPosition(bid);
        float* rot = (float *) dBodyGetRotation(bid);
        setTransform(pos, rot, &r->model.transform);
        DrawModel(r->model, (Vector3){0,0,0}, 1.0f, GREEN);
        DrawModelWires(r->model, (Vector3){0,0,0}, 1.0f, BLACK);
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

    Shader shader = LoadShader("res/simpleLight.vs", "res/simpleLight.fs");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    int amb = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, amb, (float[4]){0.1,0.1,0.5,1.0}, SHADER_UNIFORM_VEC4);

    Model model = LoadModel("res/yellow-beam-5x.obj");
    /* model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("res/yellow-beam-texture.png"); */
    model.materials[0].shader = shader;

    Light light = CreateLight(LIGHT_POINT, (Vector3){ 20,20,0 }, (Vector3){ 0,0,0 }, (Color){180,150,100,0}, shader);

    Model box = LoadModelFromMesh(GenMeshCube(1,1,1));
    box.materials[0].shader = shader;

    dInitODE2(0);
    world = dWorldCreate();
    space = dHashSpaceCreate(NULL);
    contactgroup = dJointGroupCreate(0);
    dWorldSetGravity(world, 0, -9.8, 0);
    dCreatePlane (space,0,1,0,0);

    int numObjs = 5;
    dBodyID obj[numObjs];
    for (int i = 0; i < numObjs; i++) {
        obj[i] = dBodyCreate(world);
        dGeomID geom;
        dMatrix3 R;
        dMass m;
        geom = dCreateBox(space, 1,1,1);
        dMassSetBoxTotal (&m, 1, 0.5, 0.5, 0.5);
        dBodySetPosition(obj[i], 0, 1 + i*3, -5);
        dRFromAxisAndAngle(R, dRandReal() * 2.0 - 1.0, dRandReal() * 2.0 - 1.0, dRandReal() * 2.0 - 1.0, dRandReal() * 10.0 - 5.0);
        dBodySetRotation(obj[i], R);
        dBodySetMass(obj[i], &m);
        dGeomSetBody(geom, obj[i]);
    }


    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position3);
    ECS_COMPONENT(world, ColorType);
    ECS_COMPONENT(world, Render);
    ECS_COMPONENT(world, Physics);

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

    ECS_ENTITY(world, Cube1, Render, Physics);
    ecs_set(world, Cube1, Render, {box});
    ecs_set(world, Cube1, Physics, {obj[0]});

    ECS_ENTITY(world, Cube2, Render, Physics);
    ecs_set(world, Cube2, Render, {box});
    ecs_set(world, Cube2, Physics, {obj[1]});

    ECS_ENTITY(world, Cube3, Render, Physics);
    ecs_set(world, Cube3, Render, {box});
    ecs_set(world, Cube3, Physics, {obj[2]});

    ECS_ENTITY(world, Cube4, Render, Physics);
    ecs_set(world, Cube4, Render, {box});
    ecs_set(world, Cube4, Physics, {obj[3]});

    ECS_ENTITY(world, Cube5, Render, Physics);
    ecs_set(world, Cube5, Render, {box});
    ecs_set(world, Cube5, Physics, {obj[4]});

    ECS_ENTITY(world, Beams, Position3, Render);
    ecs_set(world, Beams, Position3, {0, 0, 0});
    ecs_set(world, Beams, Render,    {model});

    ECS_SYSTEM(world, Move, OnUpdate, Position3);
    ECS_SYSTEM(world, BeforeDrawing, PreDraw, RENDERER);
    ECS_SYSTEM(world, DrawModels, OnDraw, Position3, Render);
    ECS_SYSTEM(world, ApplyPhysics, OnDraw, Render, Physics);
    ECS_SYSTEM(world, AfterDrawing, PostDraw, RENDERER);

    while (ecs_progress(world, 0) && !WindowShouldClose());
    CloseWindow();         
    return ecs_fini(world);
}
