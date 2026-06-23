#include <DX3D/All.h>
#include <DX3D/Graphics/Mesh.h>
#include <DX3D/Graphics/MeshFactory.h>
#include <vector>

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
	std::vector<GameObject*> _circleContainer;
};