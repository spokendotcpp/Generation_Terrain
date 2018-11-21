#ifndef SIMULATION_H
#define SIMULATION_H

#include <deque>
#include <random>
#include <chrono>

#include "drawableobject.h"
#include "btBulletDynamicsCommon.h"

typedef std::chrono::steady_clock Clock;

class Simulation {
private:
    std::deque<DrawableObject*> drawable_objects; // Real objects
    std::deque<btCollisionShape*> collision_shapes; // Their collision shapes

    // Dynamics World configuration:
    btCollisionDispatcher* collision_dispatcher;
    btBroadphaseInterface* broadphase_interface;
    btSequentialImpulseConstraintSolver* solver;
    btDefaultCollisionConfiguration* collision_configuration;
    btDiscreteDynamicsWorld* dynamics_world;

    float seconds;

public:
    Simulation(btVector3 gravity=btVector3(0.0f, -9.81f, 0.0f), float seconds=1.0f/60.0f);
    ~Simulation();

    void add_object(DrawableObject* obj, btCollisionShape* shape, btRigidBody* body);
    void step();
    void display(QOpenGLShaderProgram* program) const;

    static btRigidBody* make_rigid_body(btCollisionShape* shape, btTransform matrix,
                                        btScalar mass, btVector3 inertia);
};

Simulation* plane_and_sphere_simulation(
        QOpenGLShaderProgram* program, size_t nb_spheres=10);

Simulation* wall_and_sphere_simulation(
        QOpenGLShaderProgram* program, size_t wall_width=30, size_t wall_height=10);


#endif // SIMULATION_H
