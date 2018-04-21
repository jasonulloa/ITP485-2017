#include "ITPEnginePCH.h"
#include "Profiler.h"

const TypeInfo Object::sType(nullptr);

void Object::SetProperties(const rapidjson::Value& properties)
{
	PROFILE_SCOPE(Object_SetProperties);
}
