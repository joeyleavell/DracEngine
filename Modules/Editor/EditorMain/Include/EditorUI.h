#pragma once

#include "Layer.h"
#include "2D/Batch/Batch.h"
#include "Widget/UserInterface.h"
#include "Widget/Layout/GridPanel.h"
#include "ContentBrowser/ContentBrowser.h"
#include "EditorUI.gen.h"

namespace Ry
{
	class TestRefl : public Ry::Widget
	{
	public:
		GeneratedBody()

		RefField()
		Ry::String TestString{ "test_string" };
		
		RefField()
		Ry::ArrayList<int32> TestField{};

		RefField()
		float TestField2{ 20.2f };

		RefField()
		int64 TestField3 {};

	} RefClass();
	
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
