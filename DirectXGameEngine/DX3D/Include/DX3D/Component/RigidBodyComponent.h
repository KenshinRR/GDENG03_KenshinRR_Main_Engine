#pragma once

#include <DX3D/Core/Core.h>
#include <DX3D/Game/Component.h>

#include <DX3D/Component/TransformComponent.h>

#include <DX3D/Math/Vec3.h>

#include <unordered_map>

namespace dx3d
{
	class RigidBodyComponent final : public Component
	{
		dx3d_typeid(RigidBodyComponent)
	public:
		RigidBodyComponent(const ComponentDesc& data);

		void setTransformComponent(TransformComponent* tc);;

		void onStart(); // scene start
		void onEnd(); // scene end

	private:
		TransformComponent* m_transformComponent{};
		std::unordered_map<std::string, Vec3> m_initial_transform{};
	};
}

