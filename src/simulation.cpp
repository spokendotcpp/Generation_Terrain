#include "../include/simulation.h"
#include "../include/icosphere.h"
#include "../include/plane.h"
#include "../include/cube.h"

#include <iostream>
#include <thread>

Simulation::Simulation(btVector3 gravity, float _seconds)
{
    collision_configuration = new btDefaultCollisionConfiguration();
    collision_dispatcher = new btCollisionDispatcher(collision_configuration);

    broadphase_interface = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();

    dynamics_world = new btDiscreteDynamicsWorld(
        collision_dispatcher,
        broadphase_interface,
        solver,
        collision_configuration
    );

    dynamics_world->setGravity(gravity);

    seconds = _seconds;
}

Simulation::~Simulation()
{
    btCollisionObjectArray collision_objects = dynamics_world->getCollisionObjectArray();

    for(int i=0; i < dynamics_world->getNumCollisionObjects(); ++i){
        btCollisionObject* obj = collision_objects[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if( body != nullptr && body->getMotionState() != nullptr ){
            delete body->getMotionState();
        }

        dynamics_world->removeCollisionObject(obj);

        if( obj != nullptr ){
            delete obj;
            obj = nullptr;
        }
    }

    for(btCollisionShape* shape: collision_shapes){
        if( shape != nullptr ){
            delete shape;
            shape = nullptr;
        }
    }

    for(DrawableObject* obj: drawable_objects){
        if( obj != nullptr ){
            delete obj;
            obj = nullptr;
        }
    }

    collision_shapes.clear();
    drawable_objects.clear();

    delete dynamics_world;
    delete solver;
    delete broadphase_interface;
    delete collision_dispatcher;
    delete collision_configuration;
}

void
Simulation::add_object(DrawableObject* obj, btCollisionShape* shape, btRigidBody* body)
{
    drawable_objects.push_back(obj);
    collision_shapes.push_back(shape);
    dynamics_world->addRigidBody(body);
}

void
Simulation::step()
{
    dynamics_world->stepSimulation(seconds);
}

void
Simulation::display(QOpenGLShaderProgram* program) const
{
    size_t deque_idx = 0;
    btCollisionObjectArray objects = dynamics_world->getCollisionObjectArray();

    btTransform matrix;
    btScalar opengl_matrix[16];

    btCollisionObject* obj;
    btRigidBody* body;

    for(int i=0; i < dynamics_world->getNumCollisionObjects(); ++i){
        obj = objects[i];
        body = btRigidBody::upcast(obj);
        if( body != nullptr && body->getMotionState() != nullptr ){
            body->getMotionState()->getWorldTransform(matrix);
        }
        else {
            matrix = obj->getWorldTransform();
        }

        matrix.getOpenGLMatrix(opengl_matrix);

        // Qt Matrix are column-majored but Bullet ones are row-majored
        // You need to transpose matrix to proceed product.
        QMatrix4x4 model = (QMatrix4x4(opengl_matrix).transposed() // Bullet model matrix (translation and/or rotation during simulation)
                * drawable_objects[deque_idx]->model_matrix());

        program->setUniformValue("model", model);
        program->setUniformValue("model_inverse", model.transposed().inverted());

        drawable_objects[deque_idx++]->show(GL_TRIANGLES);
    }
}


btRigidBody*
Simulation::make_rigid_body(btCollisionShape* shape, btTransform matrix, btScalar mass, btVector3 inertia)
{
    if( mass != 0.0f )
        shape->calculateLocalInertia(mass, inertia);

    btDefaultMotionState* motion_state = new btDefaultMotionState(matrix);
    btRigidBody::btRigidBodyConstructionInfo rb_information(mass, motion_state, shape, inertia);
    return new btRigidBody(rb_information);
}


Simulation* plane_and_sphere_simulation(QOpenGLShaderProgram* program, size_t nb_spheres)
{
    Simulation* simulation = new Simulation();

    std::random_device random_device;
    std::default_random_engine random_engine(random_device());

    std::uniform_real_distribution<float> color{0.0f, 1.0f};
    std::uniform_real_distribution<float> scale_factor{1.0f, 5.0f};
    std::uniform_real_distribution<float> position{-50.0f, 50.0f};

    IcoSphere sphere(3);
    sphere.build(program);

    for(size_t i=0; i < nb_spheres; ++i){
        IcoSphere* isphere = new IcoSphere(sphere);

        float factor = scale_factor(random_engine);
        isphere->scale(factor, factor, factor);
        isphere->use_unique_color(
            color(random_engine),
            color(random_engine),
            color(random_engine)
        );

        isphere->update_buffers(program);

        btCollisionShape* sphere_shape = new btSphereShape(factor);
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(
            btVector3(
                position(random_engine),
                position(random_engine) + 55.0f,
                position(random_engine)
        ));

        btRigidBody* sphere_body = Simulation::make_rigid_body(
                    sphere_shape, transform, factor, btVector3(0.0f, 0.0f, 0.0f));
        sphere_body->setRestitution(1.0f);
        simulation->add_object(isphere, sphere_shape, sphere_body);
    }

    Plane* plane = new Plane();
    plane->rotate(-90.0f, 1.0f, 0.0f, 0.0f);
    plane->scale(50.0f, 50.0f, 50.0f);

    plane->build(program);
    plane->use_unique_color(0.0f, 0.0f, 0.0f);
    plane->update_buffers(program);

    btCollisionShape* plane_shape = new btBoxShape(btVector3(50.0f, 0.0f, 50.0f));
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(0.0f, 0.0f, 0.0f));

    btRigidBody* plane_body = Simulation::make_rigid_body(
                plane_shape, transform, 0.0f, btVector3(0.0f, 0.0f, 0.0f));
    plane_body->setRestitution(0.6f);
    simulation->add_object(plane, plane_shape, plane_body);

    return simulation;
}

Simulation* wall_and_sphere_simulation(QOpenGLShaderProgram* program, size_t w, size_t h)
{
    Simulation* simulation = new Simulation();

    std::random_device random_device;
    std::default_random_engine random_engine(random_device());
    std::uniform_real_distribution<float> color{0.0f, 1.0f};

    float cube_w = 2.0f;

    Cube cube;
    cube.build(program);
    cube.scale(cube_w, 1.0f, 1.0f);

    float hpos = 1.0f;
    float wpos = 0.0f;

    for(size_t j=0; j < h; ++j){
        wpos = -(cube_w * float(w)) + cube_w;

        float decal = 0.0f;
        if( j % 2 == 0 )
            decal = cube_w / 2.0f;

        for(size_t i=0; i < w; ++i){
            Cube* icube = new Cube(cube);
            icube->use_unique_color(
                color(random_engine),
                color(random_engine),
                color(random_engine)
            );
            icube->update_buffers(program);

            btCollisionShape* cube_shape = new btBoxShape(btVector3(cube_w, 1.0f, 1.0f));
            btTransform transform;
            transform.setIdentity();
            transform.setOrigin(btVector3(wpos-decal, hpos, 0.0f));

            btRigidBody* cube_body = Simulation::make_rigid_body(
                cube_shape, transform, 1.0f, btVector3(0.0f, 0.0f, 0.0f));

            cube_body->setRestitution(0.0f);
            simulation->add_object(icube, cube_shape, cube_body);

            wpos += 2*cube_w;
        }
        hpos += 2.0f;
    }

    IcoSphere* sphere = new IcoSphere(3);
    sphere->build(program);
    sphere->scale(3.0f, 3.0f, 3.0f);
    sphere->use_unique_color(1.0f, 0.0f, 0.0f);
    sphere->update_buffers(program);

    btCollisionShape* sphere_shape = new btSphereShape(3.0f);
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(0, ((float(h)*2.0f)/2.0f)+1.0f, 20.0f));

    btRigidBody* sphere_body = Simulation::make_rigid_body(
        sphere_shape, transform, 10.0f, btVector3(0.0f, 0.0f, 0.0f));


    sphere_body->setLinearVelocity(btVector3(0.0f, 0.0f, -50.0f));
    sphere_body->setRestitution(1.0f);

    simulation->add_object(sphere, sphere_shape, sphere_body);

    Plane* plane = new Plane();
    plane->rotate(-90.0f, 1.0f, 0.0f, 0.0f);
    plane->scale(50.0f, 50.0f, 50.0f);

    plane->build(program);
    plane->use_unique_color(0.0f, 0.0f, 0.0f);
    plane->update_buffers(program);

    btCollisionShape* plane_shape = new btBoxShape(btVector3(50.0f, 0.0f, 50.0f));
    transform.setIdentity();
    transform.setOrigin(btVector3(0.0f, 0.0f, 0.0f));

    btRigidBody* plane_body = Simulation::make_rigid_body(
                plane_shape, transform, 0.0f, btVector3(0.0f, 0.0f, 0.0f));
    plane_body->setRestitution(0.8f);
    simulation->add_object(plane, plane_shape, plane_body);

    return simulation;
}
