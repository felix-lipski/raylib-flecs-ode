#pragma once
#include "raylib.h"
#include <ode/ode.h>

typedef struct ColorType {
    Color color;
} ColorType;

typedef struct Render {
    Model model;
} Render;

typedef Vector3 Position3;

typedef struct Physics {
    dBodyID bid;
} Physics;
