#pragma once
#include <DX3D/Core/Core.h>
#include <DX3D/Core/Base.h>
#include <string>
#include <filesystem>

namespace dx3d
{
	class Resource : public Base
	{
	public:
		explicit Resource(const ResourceDesc& desc);

		std::wstring getFileName() const {
			return std::filesystem::path(m_path).filename().wstring();
		}
	protected:
		std::wstring m_path{};
		ResourceManager& m_manager;
	};

}