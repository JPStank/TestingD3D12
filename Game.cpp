#include "pch.h"

#include "Application.h"
#include "Game.h"
#include "Window.h"

Game::Game(const std::wstring& name, int width, int height, bool vSync)
	: mName(name)
	, mWidth(width)
	, mHeight(height)
	, mVsync(vSync)
{
}

Game::~Game()
{
	assert(!mWindow && "Use Game::Destroy() before desctruction.");
}

bool Game::Initialize()
{
	if (!DirectX::XMVerifyCPUSupport())
	{
		MessageBoxA(NULL, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	mWindow = Application::Get().CreateRenderWindow(mName, mWidth, mHeight, mVsync);
	mWindow->RegisterCallbacks(shared_from_this());
	mWindow->Show();

	return true;
}

void Game::Destroy()
{
	Application::Get().DestroyWindow(mWindow);

	mWindow.reset();
}

void Game::OnUpdate(UpdateEventArgs& e)
{

}

void Game::OnRender(RenderEventArgs& e)
{

}

void Game::OnKeyPressed(KeyEventArgs& e)
{
	// By default, do nothing.
}

void Game::OnKeyReleased(KeyEventArgs& e)
{
	// By default, do nothing.
}

void Game::OnMouseMoved(class MouseMotionEventArgs& e)
{
	// By default, do nothing.
}

void Game::OnMouseButtonPressed(MouseButtonEventArgs& e)
{
	// By default, do nothing.
}

void Game::OnMouseButtonReleased(MouseButtonEventArgs& e)
{
	// By default, do nothing.
}

void Game::OnMouseWheel(MouseWheelEventArgs& e)
{
	// By default, do nothing.
}

void Game::OnResize(ResizeEventArgs& e)
{
	mWidth = e.Width;
	mHeight = e.Height;
}

void Game::OnWindowsDestroy()
{
	// If the Window which we are registered to is 
	// destroyed, then any resources which are associated 
	// to the window must be released.
	UnloadContent();
}