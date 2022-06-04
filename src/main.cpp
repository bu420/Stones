#include <iostream>

#include <flecs.h>
#include <btBulletDynamicsCommon.h>
#include <optional>

#include "gl.h"
#include "shader.h"
#include "renderer.h"
#include "input.h"
#include "window.h"
#include "asset.h"
#include "camera.h"

struct Physics {
    btDefaultCollisionConfiguration* config;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* pairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* physics;
    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    void setup() {
        config = new btDefaultCollisionConfiguration;
        dispatcher = new btCollisionDispatcher(config);
        pairCache = new btDbvtBroadphase;
        solver = new btSequentialImpulseConstraintSolver;
        physics = new btDiscreteDynamicsWorld(dispatcher, pairCache, solver, config);
    
        physics->setGravity(btVector3(0, -10, 0));
    }

    void update() {
        physics->stepSimulation(1.f / 60, 10);
    }

    void newRigidBody(btCollisionShape* shape, const glm::vec3& pos, const float mass = 0) {
        collisionShapes.push_back(shape);

        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(pos.x, pos.y, pos.z));

        btVector3 localInertia(0, 0, 0);
        if (mass > 0) {
            shape->calculateLocalInertia(mass, localInertia);
        }

        btDefaultMotionState* motionState = new btDefaultMotionState(transform);
        btRigidBody::btRigidBodyConstructionInfo info(mass, motionState, shape, localInertia);
        btRigidBody* body = new btRigidBody(info);

        physics->addRigidBody(body);
    }
};

int main() {
    glfwInit();
    
    glfwSetErrorCallback([](int error, const char* desc) {
        std::cout << desc << std::endl;
    });

    GLFWUserPointers userPointers;

    Window window;
    window.setup(&userPointers);

    Input input;
    input.setup(window.window, &userPointers);

    Renderer renderer;
    renderer.setup();

    Camera camera;
    camera.setup({ 0, 0, 0 });

    glm::vec3 pos(10, 10, 10);

    Physics physics;
    physics.setup();

    physics.newRigidBody(new btBoxShape(btVector3(5, 5, 5)), glm::vec3(0, -20, 0));
    physics.newRigidBody(new btBoxShape(btVector3(5, 5, 5)), glm::vec3(0, 40, 0));

    while (!glfwWindowShouldClose(window.window)) {
        // Update.

        camera.update();
        physics.update();

        if (input.keyDown(GLFW_KEY_LEFT)) {
            camera.rotate(-1, 0);
        }
        if (input.keyDown(GLFW_KEY_RIGHT)) {
            camera.rotate(1, 0);
        }
        if (input.keyDown(GLFW_KEY_UP)) {
            camera.rotate(0, 1);
        }
        if (input.keyDown(GLFW_KEY_DOWN)) {
            camera.rotate(0, -1);
        }
        
        if (input.keyDown(GLFW_KEY_A)) {
            camera.moveX(-.1);
        }
        if (input.keyDown(GLFW_KEY_D)) {
            camera.moveX(.1);
        }
        if (input.keyDown(GLFW_KEY_W)) {
            camera.moveZ(-.1);
        }
        if (input.keyDown(GLFW_KEY_S)) {
            camera.moveZ(.1);
        }
        if (input.keyDown(GLFW_KEY_SPACE)) {
            camera.moveY(.1);
        }
        if (input.keyDown(GLFW_KEY_LEFT_SHIFT)) {
            camera.moveY(-.1);
        }

        // Render.

        renderer.render(camera, window.w, window.h);

        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }
}