#include "Interface/Rendering.h"

namespace Ry
{

	void Renderer::SetBlendState(const BlendParameters& State)
	{
		SetBlendingEnabled(State.BlendingEnabled);
		SetBlendMode(State.Mode);
		SetBlendFactorsSeparate(State.SourceColorFactor, State.DestColorFactor, State.SourceAlphaFactor, State.DestAlphaFactor);
	}

	void Renderer::PushCurrentBlendState()
	{
		BlendParameters CurrentState;
		CurrentState.BlendingEnabled = IsBlendingEnabled();
		CurrentState.Mode = GetBlendMode();
		CurrentState.SourceColorFactor = GetSourceBlendFactor();
		CurrentState.DestColorFactor = GetDestBlendFactor();

		PushBlendState(CurrentState);
	}
	
	void Renderer::PushBlendState(const BlendParameters& BlendState)
	{
		BlendStateStack.Add(BlendState);
	}
	
	void Renderer::PopBlendState()
	{
		// Restore the old blend state
		BlendParameters& Top = BlendStateStack[BlendStateStack.GetSize() - 1];
		SetBlendState(Top);
		BlendStateStack.RemoveAt(BlendStateStack.GetSize() - 1);
	}
	
}