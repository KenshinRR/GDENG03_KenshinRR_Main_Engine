#pragma once

#include <reactphysics3d/reactphysics3d.h>

#include <DX3D/Component/TransformComponent.h>

#include <unordered_map>

namespace dx3d
{
	class PhysicsManager
	{
    public:
        PhysicsManager();
        ~PhysicsManager();

        PhysicsManager(const PhysicsManager&) = delete;
        PhysicsManager& operator=(const PhysicsManager&) = delete;

        static PhysicsManager& getInstance() {
            static PhysicsManager instance;
            return instance;
        }

        void update(float deltaTime);
        void syncTransforms();
        dx3d::Vec3 quaternionToEuler(const reactphysics3d::Quaternion& q);

        void addRigidBody(TransformComponent* transformComp);
        void removeRigidBody(reactphysics3d::RigidBody* body);

    private:
        inline static PhysicsManager* instancePtra = nullptr;

        reactphysics3d::PhysicsCommon m_physicsCommon;
        reactphysics3d::PhysicsWorld* m_physicsWorld;

        std::unordered_map<reactphysics3d::RigidBody*, TransformComponent*> m_rigidBodies{};

        const float M_PI = 3.14f;
	};

}

