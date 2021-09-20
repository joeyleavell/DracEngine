#pragma once
#include "Layer.h"
#include "2D/Batch/Batch.h"
#include "Widget/UserInterface.h"
#include "Widget/GridLayout.h"
#include "ContentBrowser/ContentBrowser.h"

namespace Ry
{

	class EditorUI : public Layer
	{
	public:

		EditorUI(SwapChain* Parent);

		void Update(float Delta) override;
		void Render() override;
		bool OnEvent(const Event& Ev) override;

		void OnResize(int32 Width, int32 Height) override;

	private:
		
		// Record draw commands for primary editor window
		void RecordCmds();

		Ry::CommandBuffer* Cmd;

		Ry::Batch* Bat;

		UserInterface* UI;

		// Editor utility classes
		ContentBrowser* ContentBrowse;

	};
	
}
