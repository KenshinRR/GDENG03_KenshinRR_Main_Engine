#pragma once

#include "vector"
#include "string"

#include <DX3D/Resource/MaterialResource.h>

namespace dx3d
{
	class TextureComponentInspectorUIT
	{
	public:
		TextureComponentInspectorUIT();

		void draw(MaterialResource* matRes);

	private:
		int getTextureNameKey(std::string textName);
	private:
		std::vector<const char*> m_textureNames;
	};
}

