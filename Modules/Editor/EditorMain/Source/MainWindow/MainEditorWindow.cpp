#pragma once

#include "MainWindow/MainEditorWindow.h"
#include "MainWindow/MainEditorLayer.h"
#include "Core/Globals.h"

namespace Ry
{

	int32 MainEditorWindow::GetViewportWidth()
	{
		return GetWindow()->FindWindowWidth();
	}

	int32 MainEditorWindow::GetViewportHeight()
	{
		return GetWindow()->FindWindowHeight();
	}

    void MainEditorWindow::Init()
    {
        EditorWindow::Init();

        // Only the main editor window initializes the rendering engine
		Ry::InitRenderingEngine();

        // TODO: make this more robust to better support multiple windows
        Ry::ViewportWidthDel.BindMemberFunction(this, &MainEditorWindow::GetViewportWidth);
		Ry::ViewportHeightDel.BindMemberFunction(this, &MainEditorWindow::GetViewportHeight);

        // Add main editor layer
        MainEditorLayer* UILayer = new MainEditorLayer(GetWindow()->GetSwapChain());
        GetLayerStack().PushLayer(UILayer);

    }

    void MainEditorWindow::Update(float Delta)
    {
        EditorWindow::Update(Delta);

    }

    void MainEditorWindow::Render()
    {
        EditorWindow::Render();
        
    }

}