#include "MainGame.h"
#include "Objects/Player.h"
#include <DX3D/Graphics/Mesh/MeshFactory.h>
#include <DX3D/Component/MeshComponent.h>
#include <DX3D/UI/SceneUI.h>
#include <DX3D/UI/MainMenuBarUI.h>
#include <filesystem>


MainGame::MainGame(const dx3d::GameDesc& desc) : dx3d::Game(desc)
{
}

void MainGame::onCreate()
{
	Game::onCreate();
	auto& world = getWorld();
	std::filesystem::path base = std::filesystem::current_path().parent_path();
	auto woodTex = getResourceManager().createResourceFromFile<dx3d::TextureResource>((base/"DirectXGameEngine/Game/Assets/Textures/wood.jpg").c_str());
	auto floorTex = getResourceManager().createResourceFromFile<dx3d::TextureResource>((base / "DirectXGameEngine/Game/Assets/Textures/floor.jpg").c_str());

	auto logo = getResourceManager().createResourceFromFile<dx3d::TextureResource>((base / "DirectXGameEngine/Game/Assets/Textures/dlsuLogo.png").c_str());

	// UI initilization
	std::unique_ptr<dx3d::SceneUI> scene_UI = std::make_unique<dx3d::SceneUI>(dx3d::BaseDesc{ getLogger() });
	scene_UI->setLogo(logo);
	m_UIs.push_back(std::move(scene_UI));
	m_UIs.push_back(std::make_unique<dx3d::MainMenuBarUI>(dx3d::BaseDesc{ getLogger() }));
	
	// Create mesh resources (reusable)
	auto cubeMesh = getMeshFactory().createCubeMesh();
	auto sphereMesh = getMeshFactory().createSphereMesh(20, 20);
	auto capsuleMesh = getMeshFactory().createCapsuleMesh(0.5f, 2.0f);
	auto cylinderMesh = getMeshFactory().createCylinderMesh(0.5f, 2.0f);
	auto planeMesh = getMeshFactory().createPlaneMesh(10.0f, 10.0f);
	auto circleMesh = getMeshFactory().createCircleMesh(0.5f, 32);

	auto teapotMesh = getResourceManager().createResourceFromFile<dx3d::MeshResource>((base / "DirectXGameEngine/Game/Assets/3D Objects/teapot.obj").c_str());

	//// Create a floor with plane
	//auto floor = world.createGameObject<dx3d::GameObject>();
	//auto floorMeshComp = floor->createOrGetComponent<dx3d::MeshComponent>();
	//floorMeshComp->setMesh(planeMesh);
	//floor->getTransform().setScale({ 1.0f, 0.0f, 1.0f });
	//floor->getTransform().setPosition({ 0.0f, -1.0f, 0.0f });

	{
		auto basicMat = getResourceManager().createResourceFromFile<dx3d::MaterialResource>((base/"DirectXGameEngine/Game/Assets/Shaders/Basic.hlsl").c_str());
		if (basicMat)
		{
			auto matData = dx3d::Vec3(1, 1, 1);
			basicMat->setData(std::as_bytes(std::span{ &matData, 1 }));
			basicMat->setTexture(0, floorTex);
		}

		auto floor = world.createGameObject<dx3d::GameObject>();
		auto floorMeshComp = floor->createOrGetComponent<dx3d::MeshComponent>();
		floorMeshComp->setMesh(planeMesh);
		floorMeshComp->setMaterial(basicMat);
		floor->getTransform().setScale({ 6.8f, 0.1f, 6.8f });
		floor->getTransform().setPosition({ 0, 0, 0 });

	}

	srand((unsigned int)time(NULL));

	auto basicMat = getResourceManager().createResourceFromFile<dx3d::MaterialResource>((base / "DirectXGameEngine/Game/Assets/Shaders/Basic.hlsl").c_str());
	if (basicMat)
	{
		auto matData = dx3d::Vec3(1, 1, 1);
		basicMat->setData(std::as_bytes(std::span{ &matData, 1 }));
		basicMat->setTexture(0, woodTex);
	}

	// Creating cubes
	/*for (auto y = -2; y < 3; y++)
	{
		for (auto x = -2; x < 3; x++)
		{
			auto cube = world.createGameObject<dx3d::GameObject>();
			auto comp = cube->createOrGetComponent<dx3d::MeshComponent>();
			comp->setMaterial(basicMat);
			comp->setMesh(cubeMesh);
			auto roty = (rand() % 628) / 100.0f;
			cube->getTransform().setScale({ 0.5,0.5,0.5 });
			cube->getTransform().setPosition({ x * 1.4f, 0.25f + 0.05f, y * 1.4f });
			cube->getTransform().setRotation({ 0,roty,0 });
		}
	}*/

	// Create a teapot
	auto teapot = world.createGameObject<dx3d::GameObject>();
	auto teapotMeshComponent = teapot->createOrGetComponent<dx3d::MeshComponent>();
	teapotMeshComponent->setMaterial(basicMat);
	teapotMeshComponent->setMesh(teapotMesh->getMesh());
	teapot->getTransform().setPosition({ 0.0f, 1.0f, 0.0f });
	teapot->getTransform().setRotation({ 0.0f, 0.0f, 0.0f });

	auto player = world.createGameObject<Player>();
	player->getTransform().setPosition({ 0, 1, -2 });

	getInputSystem().setCursorLocked(false);
	getInputSystem().setCursorVisible(true);
}

void MainGame::onUpdate(dx3d::f32 deltaTime)
{
	Game::onUpdate(deltaTime);
}

void MainGame::onDrawUi()
{
	for (auto& m_UI : m_UIs)
	{
		m_UI->draw();
	}
}
