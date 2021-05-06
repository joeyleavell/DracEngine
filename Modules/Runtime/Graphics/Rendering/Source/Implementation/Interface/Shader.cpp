#include "Interface/Shader.h"
#include "Language/ShaderReflector.h"
#include "Core/Globals.h"

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

	void ShaderReflection::AddResourceDescription(ResourceLayout* Desc)
	{
		Resources.Add(Desc);
	}

	void ShaderReflection::AddInputVariable(ShaderVariable Var)
	{
		InputVars.Add(Var);
	}

	const Ry::ArrayList<ResourceLayout*>& ShaderReflection::GetResources() const
	{
		return Resources;
	}

	const Ry::ArrayList<ShaderVariable>& ShaderReflection::GetInputVars() const
	{
		return InputVars;
	}

	const ResourceLayout* ShaderReflection::operator[](int32 Index) const
	{
		for(const ResourceLayout* Desc : Resources)
		{
			if(Desc->SetIndex == Index)
			{
				return Desc;
			}
		}
		return nullptr;
	}

	Shader::Shader(const Ry::String& VSAsset, const Ry::String& FSAsset)
	{
		// Generate reflection data
		
		ReflectShader(VSAsset, ShaderStage::Vertex, VertReflectionData);
		ReflectShader(FSAsset, ShaderStage::Fragment, FragReflectionData);

		// Create vertex format from vertex shader input variables
		const Ry::ArrayList<ShaderVariable>& InputVars = VertReflectionData.GetInputVars();
		for(int32 VertInputVar = 0; VertInputVar < InputVars.GetSize(); VertInputVar++)
		{
			const ShaderVariable& Var = InputVars[VertInputVar];

			if(Var.ArraySize != 1)
			{
				Ry::Log->LogErrorf("Arrays not supported for vertex attributes: %s", *Var.Name);
				return;
			}

			if(!Var.IsVectorType())
			{
				Ry::Log->LogErrorf("Vertex input data type not supported for input var: ", *Var.Name);
				return;
			}

			// Add the variable to the format
			VertexAttrib NewAttrib;
			NewAttrib.Name = Var.Name;
			NewAttrib.Size = Var.GetVectorElementCount();
			
			VertFormat.Attributes.Add(NewAttrib);
			VertFormat.ElementCount += NewAttrib.Size;
		}
	}

	const VertexFormat& Shader::GetVertexFormat() const
	{
		return VertFormat;
	}

	const ShaderReflection& Shader::GetVertexReflectionData() const
	{
		return VertReflectionData;
	}

	const ShaderReflection& Shader::GetFragmentReflectionData() const
	{
		return FragReflectionData;
	}
	
}
