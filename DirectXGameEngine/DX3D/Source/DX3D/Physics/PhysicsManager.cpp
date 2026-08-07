#include <DX3D/Physics/PhysicsManager.h>

#include <DX3D/Math/Vec3.h>

dx3d::PhysicsManager::PhysicsManager()
{
    reactphysics3d::PhysicsWorld::WorldSettings settings;
    settings.gravity = reactphysics3d::Vector3(0, -9.81f, 0);
    m_physicsWorld = m_physicsCommon.createPhysicsWorld(settings);
}

dx3d::PhysicsManager::~PhysicsManager()
{
    if (m_physicsWorld) {
        m_physicsCommon.destroyPhysicsWorld(m_physicsWorld);
        m_physicsWorld = nullptr;
    }
}

void dx3d::PhysicsManager::update(float deltaTime)
{
    m_physicsWorld->update(deltaTime);

    syncTransforms();
}

void dx3d::PhysicsManager::syncTransforms()
{
    for (auto& [body, transformComp] : m_rigidBodies) 
    {
        reactphysics3d::Transform physicsTransform = body->getTransform();

        // Position
        reactphysics3d::Vector3 pos = physicsTransform.getPosition();
        transformComp->setPosition(dx3d::Vec3(pos.x, pos.y, pos.z));

        // Orientation → Euler angles
        reactphysics3d::Quaternion q = physicsTransform.getOrientation();
        dx3d::Vec3 euler = quaternionToEuler(q);

        // Convert to degrees if your TransformComponent expects degrees
        dx3d::Vec3 rotationDegrees(
            euler.x * (180.0f / M_PI),
            euler.y * (180.0f / M_PI),
            euler.z * (180.0f / M_PI)
        );

        transformComp->setRotation(rotationDegrees);
    }
}

dx3d::Vec3 dx3d::PhysicsManager::quaternionToEuler(const reactphysics3d::Quaternion& q) {
    // Extract components
    float w = q.w;
    float x = q.x;
    float y = q.y;
    float z = q.z;

    // Roll (X-axis rotation)
    float sinr_cosp = 2.0f * (w * x + y * z);
    float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
    float roll = std::atan2(sinr_cosp, cosr_cosp);

    // Pitch (Y-axis rotation)
    float sinp = 2.0f * (w * y - z * x);
    float pitch;
    if (std::abs(sinp) >= 1)
        pitch = std::copysign(M_PI / 2.0f, sinp); // clamp
    else
        pitch = std::asin(sinp);

    // Yaw (Z-axis rotation)
    float siny_cosp = 2.0f * (w * z + x * y);
    float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
    float yaw = std::atan2(siny_cosp, cosy_cosp);

    return dx3d::Vec3(roll, pitch, yaw);
}


void dx3d::PhysicsManager::addRigidBody(TransformComponent* transformComp)
{
    // getting the orientation
    Vec3 rotationEuler = transformComp->getRotation();

    float pitch = rotationEuler.x * (M_PI / 180.0f);
    float yaw = rotationEuler.y * (M_PI / 180.0f);
    float roll = rotationEuler.z * (M_PI / 180.0f);

    reactphysics3d::Quaternion orientation =
        reactphysics3d::Quaternion::fromEulerAngles(pitch, yaw, roll);

    reactphysics3d::Transform physicsTransform(
        // position
        reactphysics3d::Vector3(
            transformComp->getPosition().x,
            transformComp->getPosition().y,
            transformComp->getPosition().z
        ),   
        orientation       // orientation
    );

    reactphysics3d::RigidBody* body = m_physicsWorld->createRigidBody(physicsTransform);

    m_rigidBodies[body] = transformComp;
}

void dx3d::PhysicsManager::removeRigidBody(reactphysics3d::RigidBody* body) {
    if (body) {
        // Destroy in physics world
        m_physicsWorld->destroyRigidBody(body);

        // Remove from tracking map
        auto it = m_rigidBodies.find(body);
        if (it != m_rigidBodies.end()) {
            m_rigidBodies.erase(it);
        }
    }
}

