#include <DX3D/All.h>
#include <DX3D/Graphics/Mesh.h>
#include <DX3D/Graphics/MeshFactory.h>
#include <stack>

class CircleSpawner : public dx3d::GameObject
{
	dx3d_typeid(CircleSpawner)
public:
	explicit CircleSpawner(const dx3d::GameObjectDesc& desc);
	virtual ~CircleSpawner() override;
protected:
	virtual void onCreate();
	virtual void onUpdate(dx3d::f32 deltaTime);

private:
	std::stack<GameObject*> _circleContainer;
};