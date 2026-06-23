#include "CircleSpawner.h"
#include <DX3D/Graphics/MeshFactory.h>
#include <DX3D/Component/MeshComponent.h>

CircleSpawner::CircleSpawner(const dx3d::GameObjectDesc& desc) : GameObject(desc)
{
}

CircleSpawner::~CircleSpawner()
{
}

void CircleSpawner::onCreate()
{
}

void CircleSpawner::onUpdate(dx3d::f32 deltaTime)
{
	if (getInputSystem().isKeyDown(dx3d::KeyCode::Space))
	{
		auto newCircle = getWorld().createGameObject<dx3d::GameObject>();
		auto circleMeshComp = newCircle->createOrGetComponent<dx3d::MeshComponent>();
		circleMeshComp->setMesh(dx3d::MeshFactory::getInstance().getMesh(dx3d::Circle));
		newCircle->getTransform().setPosition({ -1.0f, 1.0f, 0.0f });
		newCircle->getTransform().setRotation({ 1.57f, 0.0f, 0.0f }); // rotate 90 degrees on X-axis
		_circleContainer.push_back(newCircle);
	}
}
