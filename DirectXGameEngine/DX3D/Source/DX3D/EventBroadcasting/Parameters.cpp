#include <DX3D/EventBroadcasting/Parameters.h>

dx3d::Parameters::Parameters()
{
}

dx3d::Parameters::~Parameters()
{
}

void dx3d::Parameters::PutExtra(std::string paramName, uint32_t value)
{
	this->uint32Data[paramName] = (value);
}

void dx3d::Parameters::PutExtra(std::string paramName, size_t value)
{
	this->sizeTData[paramName] = value;
}

void dx3d::Parameters::PutExtra(std::string paramName, bool value)
{
	this->boolData[paramName] = value;
}

void dx3d::Parameters::PutExtra(std::string paramName, GameObject* value)
{
	this->ptrGameObjectData[paramName] = (value);
}

void dx3d::Parameters::PutExtra(std::string paramName, std::string value)
{
	this->stringData[paramName] = value;
}

void dx3d::Parameters::PutExtra(std::string paramName, const Vec3& value)
{
	this->vec3Data[paramName] = value;
}

uint32_t dx3d::Parameters::GetUInt32Extra(std::string paramName, uint32_t def_value)
{
	if (this->uint32Data.contains(paramName))
	{
		return this->uint32Data[paramName];
	}
	else
	{
		return def_value;
	}
}

size_t dx3d::Parameters::GetSizeTExtra(std::string paramName, size_t def_value)
{
	if (this->sizeTData.contains(paramName))
	{
		return this->sizeTData[paramName];
	}
	else
	{
		return def_value;
	}
}

bool dx3d::Parameters::GetBoolExtra(std::string paramName, bool def_value)
{
	if (this->boolData.contains(paramName))
	{
		return this->boolData[paramName];
	}
	else
	{
		return def_value;
	}
}

dx3d::GameObject* dx3d::Parameters::GetGameObjectPtr(std::string paramName, GameObject* def_value)
{
	if (this->ptrGameObjectData.contains(paramName))
	{
		return this->ptrGameObjectData[paramName];
	}
	else
	{
		return def_value;
	}
}

dx3d::Vec3 dx3d::Parameters::GetVec3Extra(std::string paramName, const Vec3& def_value)
{
	if (this->vec3Data.contains(paramName))
	{
		return this->vec3Data[paramName];
	}
	else
	{
		return def_value;
	}
}

std::string dx3d::Parameters::GetStringExtra(std::string paramName, std::string def_value)
{
	if (this->stringData.contains(paramName))
	{
		return this->stringData[paramName];
	}
	else
	{
		return def_value;
	}
}
