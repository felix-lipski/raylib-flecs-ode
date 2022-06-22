#include "raylib.h"
#include <ode/ode.h>

dWorldID world;
dJointGroupID contactgroup;
dSpaceID space;

void setTransform(const float pos[3], const float R[12], Matrix* matrix) {
    matrix->m0  = R[0]; matrix->m1    = R[4]; matrix->m2    = R[8];
    matrix->m4  = R[1]; matrix->m5    = R[5]; matrix->m6    = R[9];
    matrix->m8  = R[2]; matrix->m9    = R[6]; matrix->m10   = R[10];
    matrix->m12 = pos[0]; matrix->m13 = pos[1]; matrix->m14 = pos[2];
}

#define MAX_CONTACTS 8
void nearCallback(void *data, dGeomID o1, dGeomID o2) {
    (void)data;
    int i;

    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact))
        return;

    dContact contact[MAX_CONTACTS];
    for (i = 0; i < MAX_CONTACTS; i++) {
        contact[i].surface.mode = dContactBounce;
        contact[i].surface.mu = dInfinity;
        contact[i].surface.mu2 = 0;
        contact[i].surface.bounce = 0.5;
        contact[i].surface.bounce_vel = 0.5;
    }
    int numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom,
                        sizeof(dContact));
    if (numc) {
        dMatrix3 RI;
        dRSetIdentity(RI);
        for (i = 0; i < numc; i++) {
            dJointID c =
                dJointCreateContact(world, contactgroup, contact + i);
            dJointAttach(c, b1, b2);
        }
    }
}
