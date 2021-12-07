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

		RefField()
		Ry::ArrayList<Ry::Object*> IntArray;

		RefField()
		int32 TestInt = 10;

		RefField()
		Ry::String TestString = "Another string";

		RefField()
		Ry::String TestString2 = "some string";


		TestObj()
		{
			ChildLabel = new Label;
			ChildLabel->Text = "this is some text";
			ChildLabel->TextStyleName = "this is a text style";

			//IntArray = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			Label* Test = new Ry::Label{};
			Test->Text = "label1";
			IntArray.Add(Test);
			IntArray.Add(new Ry::Label);
			IntArray.Add(new Ry::Label);
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
