#include "MainGame.h"
#include "Objects/Player.h"
#include "Objects/CircleSpawner.h"
#include <DX3D/Graphics/MeshFactory.h>
#include <DX3D/Component/MeshComponent.h>


MainGame::MainGame(const dx3d::GameDesc& desc) : dx3d::Game(desc)
{
}

void MainGame::onCreate()
{
	Game::onCreate();
	auto& world = getWorld();

	// Create mesh resources (reusable)
	auto cubeMesh = dx3d::MeshFactory::getInstance().getMesh(dx3d::Cube);
	auto sphereMesh = dx3d::MeshFactory::getInstance().getMesh(dx3d::Sphere);
	auto capsuleMesh = dx3d::MeshFactory::getInstance().getMesh(dx3d::Capsule);
	auto cylinderMesh = dx3d::MeshFactory::getInstance().getMesh(dx3d::Cylinder);
	auto planeMesh = dx3d::MeshFactory::getInstance().getMesh(dx3d::Plane);
	auto circleMesh = dx3d::MeshFactory::getInstance().getMesh(dx3d::Circle);

	// Create a floor with plane
	/*auto floor = world.createGameObject<dx3d::GameObject>();
	auto floorMeshComp = floor->createOrGetComponent<dx3d::MeshComponent>();
	floorMeshComp->setMesh(planeMesh);
	floor->getTransform().setScale({ 1.0f, 0.0f, 1.0f });
	floor->getTransform().setPosition({ 0.0f, -1.0f, 0.0f });*/

	// Creating Circle Spawner
	auto circleSpawner = world.createGameObject<CircleSpawner>();
	circleSpawner->getTransform().setPosition({ 0, 0, 0 });


	srand((unsigned int)time(NULL));

	// Create a circle object
	//auto circle = world.createGameObject<dx3d::GameObject>();
	//auto circleMeshComp = circle->createOrGetComponent<dx3d::MeshComponent>();
	//circleMeshComp->setMesh(circleMesh);
	//circle->getTransform().setPosition({ 1.0f, 1.0f, 0.0f });
	//circle->getTransform().setRotation({ 1.57f, 0.0f, 0.0f}); // rotate 90 degrees on X-axis

	auto player = world.createGameObject<Player>();
	player->getTransform().setPosition({ 0, 1, -2 });

	getInputSystem().setCursorLocked(false);
	getInputSystem().setCursorVisible(true);
}

void MainGame::onUpdate(dx3d::f32 deltaTime)
{
	Game::onUpdate(deltaTime);
}