#include <DX3D/All.h>
#include <DX3D/Graphics/Mesh.h>
#include <DX3D/Graphics/MeshFactory.h>
#include <stack>
#include <random>

class BouncingCircle : public dx3d::GameObject
{
	dx3d_typeid(BouncingCircle)
public:
	explicit BouncingCircle(const dx3d::GameObjectDesc& desc);
	virtual ~BouncingCircle() override;
protected:
	virtual void onCreate();
	virtual void onUpdate(dx3d::f32 deltaTime);

private:
	std::stack<GameObject*> _circleContainer;
	std::random_device rd;
	float _upDir, _rightDir;
	dx3d::Vec3 _velocity;
	float minX, maxX, minY, maxY;
};