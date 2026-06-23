#include "BouncingCircle.h"

BouncingCircle::BouncingCircle(const dx3d::GameObjectDesc& desc) : GameObject(desc)
{
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	_upDir = std::round(dist(gen) * 10.0f) / 10.0f;
	_rightDir = std::round(dist(gen) * 10.0f) / 10.0f;

    _velocity = { 1.0f,1.0f,0.0f };

	maxX = 2.5f;
	minX = -maxX;

	maxY = 1.25f;
	minY = -maxY;
}

BouncingCircle::~BouncingCircle()
{
}

void BouncingCircle::onCreate()
{
}

void BouncingCircle::onUpdate(dx3d::f32 deltaTime)
{
    dx3d::Vec3 pos = getTransform().getPosition();

    // Update position based on velocity
    pos += _velocity * deltaTime;

    // Bounce on X boundaries
    if (pos.x >= maxX)
    {
        pos.x = maxX;
        _velocity.x *= -1; // reverse X direction
    }
    else if (pos.x <= minX)
    {
        pos.x = minX;
        _velocity.x *= -1;
    }

    // Bounce on Y boundaries
    if (pos.y >= maxY)
    {
        pos.y = maxY;
        _velocity.y *= -1; // reverse Y direction
    }
    else if (pos.y <= minY)
    {
        pos.y = minY;
        _velocity.y *= -1;
    }

    getTransform().setPosition(pos);
}


