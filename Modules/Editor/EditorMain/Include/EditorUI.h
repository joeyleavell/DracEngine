#pragma once

#include "Layer.h"
#include "2D/Batch/Batch.h"
#include "Widget/UserInterface.h"
#include "Widget/Layout/GridPanel.h"
#include "ContentBrowser/ContentBrowser.h"
#include "EditorUI.gen.h"
#include "Widget/Label.h"
#include "EditorUI.gen.h"

namespace Ry
{

	class TestObj : public Ry::Object
	{
	public:
		GeneratedBody()

		RefField()
		Ry::Label* ChildLabel;

		//RefField()
		//int32 TestInt = 10;

		RefField()
		Ry::String TestString = "some string";

		TestObj()
		{
			ChildLabel = new Label;
			ChildLabel->Text = "this is some text";
			ChildLabel->TextStyleName = "this is a text style";
		}

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
