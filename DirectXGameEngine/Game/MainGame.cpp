#include "MainGame.h"
#include "Objects/MyObject.h"


MainGame::MainGame(const dx3d::GameDesc& desc) : dx3d::Game(desc)
{
}

void MainGame::onCreate()
{
	Game::onCreate();
	auto& world = getWorld();

	auto object = world.createGameObject<dx3d::GameObject>();
	object->createOrGetComponent<dx3d::CubeComponent>();
	m_player = object;

	getInputSystem().setCursorLocked(false);
	getInputSystem().setCursorVisible(true);
}

void MainGame::onUpdate(dx3d::f32 deltaTime)
{
	Game::onUpdate(deltaTime);

	auto rot = m_player->getTransform().getRotation();
	rot.x += getInputSystem().getMouseDelta().y * 0.01f;
	rot.y -= getInputSystem().getMouseDelta().x * 0.01f;
	m_player->getTransform().setRotation(rot);


	auto pos = m_player->getTransform().getPosition();
	auto forward = 0.0f;
	auto rightward = 0.0f;
	auto speed = 3.0f;
	if (getInputSystem().isKeyDown(dx3d::KeyCode::W)) forward = 1.0f;
	if (getInputSystem().isKeyDown(dx3d::KeyCode::S)) forward = -1.0f;
	if (getInputSystem().isKeyDown(dx3d::KeyCode::D)) rightward = 1.0f;
	if (getInputSystem().isKeyDown(dx3d::KeyCode::A)) rightward = -1.0f;
	auto direction = dx3d::Vec3::normalize({ rightward,forward,0 });
	pos = pos + direction * speed * deltaTime;
	m_player->getTransform().setPosition(pos);
}