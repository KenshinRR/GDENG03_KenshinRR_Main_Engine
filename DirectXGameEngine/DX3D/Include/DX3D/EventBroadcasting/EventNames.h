#pragma once
#include <string>

namespace dx3d
{
	class EventNames
	{
	public:
		inline static const std::string ON_CREDITS_POPUP = "ON_CREDITS_POPUP";
		inline static const std::string ON_COLORPICKER_POPUP = "ON_COLORPICKER_POPUP";

		inline static const std::string ON_ADD_GAMEOBJECT = "ON_ADD_GAMEOBJECT";
		inline static const std::string ON_ADD_3D_OBJECT = "ON_ADD_3D_OBJECT";
		inline static const std::string ON_GAMEOBJECT_SELECTED = "ON_GAMEOBJECT_SELECTED";

		inline static const std::string ON_SCENE_PLAY = "ON_SCENE_PLAY";
		inline static const std::string ON_SCENE_STOP = "ON_SCENE_STOP";
		inline static const std::string ON_SCENE_PAUSE = "ON_SCENE_PAUSE";
		inline static const std::string ON_SCENE_UNPAUSE = "ON_SCENE_UNPAUSE";
		inline static const std::string ON_SCENE_FRAMESTEP = "ON_SCENE_FRAMESTEP";

		inline static const std::string ACTIVITY_SPAWN20CUBES = "ACTIVITY_SPAWN20CUBES";
	};
}
