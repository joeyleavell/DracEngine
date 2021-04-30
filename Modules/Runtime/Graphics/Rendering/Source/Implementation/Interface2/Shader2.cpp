#include "Interface2/Shader2.h"
#include "Language/ShaderReflector.h"

namespace Ry
{
	Ry::String ToString(ShaderPrimitiveDataType DT)
	{
		// Just manually reflecting the enum here
		switch (DT)
		{
		case ShaderPrimitiveDataType::Float:
			return "Float";
		case ShaderPrimitiveDataType::Float2:
			return "Float2";
		case ShaderPrimitiveDataType::Float1x2:
			return "Float1x2";
		case ShaderPrimitiveDataType::Float2x1:
			return "Float2x1";
		case ShaderPrimitiveDataType::Float3:
			return "Float3";
		case ShaderPrimitiveDataType::Float1x3:
			return "Float1x3";
		case ShaderPrimitiveDataType::Float3x1:
			return "Float3x1";
		case ShaderPrimitiveDataType::Float4:
			return "Float4";
		case ShaderPrimitiveDataType::Float4x1:
			return "Float4x1";
		case ShaderPrimitiveDataType::Float1x4:
			return "Float1x4";
		case ShaderPrimitiveDataType::Float2x2:
			return "Float2x2";
		case ShaderPrimitiveDataType::Float3x2:
			return "Float3x2";
		case ShaderPrimitiveDataType::Float2x3:
			return "Float2x3";
		case ShaderPrimitiveDataType::Float4x2:
			return "Float4x2";
		case ShaderPrimitiveDataType::Float2x4:
			return "Float2x4";
		case ShaderPrimitiveDataType::Float3x3:
			return "Float3x3";
		case ShaderPrimitiveDataType::Float4x3:
			return "Float4x3";
		case ShaderPrimitiveDataType::Float3x4:
			return "Float3x4";
		case ShaderPrimitiveDataType::Float4x4:
			return "Float4x4";
		default:
			return "None";
		}
	}

	void ShaderReflection::AddResourceDescription(ResourceSetDescription* Desc)
	{
		Resources.Add(Desc);
	}

	void ShaderReflection::AddInputVariable(ShaderInputVariable Var)
	{
		InputVars.Add(Var);
	}

	const Ry::ArrayList<ResourceSetDescription*>& ShaderReflection::GetResources() const
	{
		return Resources;
	}

	const Ry::ArrayList<ShaderInputVariable>& ShaderReflection::GetInputVars() const
	{
		return InputVars;
	}

	const ResourceSetDescription* ShaderReflection::operator[](int32 Index) const
	{
		for(const ResourceSetDescription* Desc : Resources)
		{
			if(Desc->SetIndex == Index)
			{
				return Desc;
			}
		}
		return nullptr;
	}

	Shader2::Shader2(const Ry::String& VSAsset, const Ry::String& FSAsset)
	{
		// Generate reflection data
		
		ReflectShader(VSAsset, ShaderStage::Vertex, VertReflectionData);
		ReflectShader(FSAsset, ShaderStage::Fragment, FragReflectionData);
	}

	const ShaderReflection& Shader2::GetVertexReflectionData() const
	{
		return VertReflectionData;
	}

	const ShaderReflection& Shader2::GetFragmentReflectionData() const
	{
		return FragReflectionData;
	}
	
}
