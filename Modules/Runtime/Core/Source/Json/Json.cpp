#include "Json/Json.h"
#include "nlohmann/json.hpp"
#include <complex.h>

namespace Ry
{

	Json::Json()
	{
		Pimpl = new nlohmann::json;
	}

	Json::Json(double Value)
	{
		Pimpl = new nlohmann::json(Value);
	}

	Json::Json(Ry::String Value)
	{
		Pimpl = new nlohmann::json(*Value);
	}

	Json::Json(uint64 Value)
	{
		Pimpl = new nlohmann::json(Value);
	}

	Json::Json(int64 Value)
	{
		Pimpl = new nlohmann::json(Value);
	}

	Json::Json(bool Value)
	{
		Pimpl = new nlohmann::json(Value);
	}

	Json::Json(const Json& Other)
	{
		this->Pimpl = new nlohmann::json(*Other.Pimpl);
	}

	Json::Json(Json&& Other) noexcept
	{
		this->Pimpl = Other.Pimpl;
		Other.Pimpl = nullptr;
	}

	Json::Json(nlohmann::RyJson* PimplCopy)
	{
		this->Pimpl = new nlohmann::RyJson(*PimplCopy);
	}

	bool Json::IsNull() const
	{
		return Pimpl->is_null();
	}

	bool Json::IsObject() const
	{
		return Pimpl->is_object();
	}

	bool Json::IsNumber() const
	{
		return Pimpl->is_number();
	}

	bool Json::IsIntegerNumber() const
	{
		return Pimpl->is_number_integer();
	}

	bool Json::IsUnsignedNumber() const
	{
		return Pimpl->is_number_unsigned();
	}

	bool Json::IsFloatingPointNumber() const
	{
		return Pimpl->is_number_float();
	}

	bool Json::IsArray() const
	{
		return Pimpl->is_array();
	}

	bool Json::IsString() const
	{
		return Pimpl->is_string();
	}

	bool Json::IsBoolean() const
	{
		return Pimpl->is_boolean();
	}

	int32 Json::Num()
	{
		return Pimpl->size();
	}

	template<typename T>
	void Json::Add(const T& Value)
	{
		Pimpl->push_back(Value);
	}

	template <typename T>
	T Json::Value(Ry::String FieldName, T DefaultValue) const
	{
		return Pimpl->value("", DefaultValue);
	}

	// Specialize adding strings
	template<>
	void Json::Add<Ry::String>(const Ry::String& Value)
	{
		Pimpl->push_back(*Value);
	}

	// Specialize adding other json objects
	template<>
	void Json::Add<Ry::Json>(const Json& Value)
	{
		Pimpl->push_back(*Value.Pimpl);
	}

	Json Json::operator[](Ry::String FieldName) const
	{
		nlohmann::json Child = (*Pimpl)[*FieldName];
		return Json(&Child);
	}

	Json& Json::operator=(const Json& Other)
	{
		if (Other == *this)
			return *this;
		
		this->Pimpl = new nlohmann::json(*Other.Pimpl);

		return *this;
	}

	Json& Json::operator=(Json&& Other) noexcept
	{
		this->Pimpl = Other.Pimpl;
		Other.Pimpl = nullptr;

		return *this;
	}

	Json Json::Parse(const Ry::String& Json)
	{
		nlohmann::json TmpJson = nlohmann::json::parse(*Json);
		return Ry::Json(&TmpJson);
	}

	Json Json::CreateObject()
	{
		nlohmann::json Tmp = nlohmann::json::object();
		return Json(&Tmp);
	}

	Json Json::CreateArray()
	{
		nlohmann::json Tmp = nlohmann::json::array();
		return Json(&Tmp);
	}

	bool Json::operator==(const Json& Other) const
	{
		return (*Pimpl) == (*Other.Pimpl);
	}

	Ry::String Json::Stringify() const
	{
		return Ry::String(Pimpl->dump(4).c_str());
	}

	template <typename T>
	T& Json::operator[](int32 Index)
	{
		return (*Pimpl)[Index];
	}

	template <typename T>
	void Json::Insert(Ry::String FieldName, const T& Value)
	{
		(*Pimpl)[*FieldName] = Value;
	}

	// Specialize inserting strings
	template <>
	void Json::Insert<Ry::String>(Ry::String FieldName, const Ry::String& Value)
	{
		(*Pimpl)[*FieldName] = *Value;
	}

	Json::~Json()
	{
		delete Pimpl;
		Pimpl = nullptr;
	}

	template void Json::Insert<int8>(Ry::String FieldName, const int8& Value);
	template void Json::Insert<uint8>(Ry::String FieldName, const uint8& Value);
	template void Json::Insert<int16>(Ry::String FieldName, const int16& Value);
	template void Json::Insert<uint16>(Ry::String FieldName, const uint16& Value);
	template void Json::Insert<int32>(Ry::String FieldName, const int32& Value);
	template void Json::Insert<uint32>(Ry::String FieldName, const uint32& Value);
	template void Json::Insert<int64>(Ry::String FieldName, const int64& Value);
	template void Json::Insert<uint64>(Ry::String FieldName, const uint64& Value);
	template void Json::Insert<float>(Ry::String FieldName, const float& Value);
	template void Json::Insert<double>(Ry::String FieldName, const double& Value);
	
}
