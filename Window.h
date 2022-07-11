#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <memory>

#include "Events.h"
#include "HighResolutionClock.h"

using Microsoft::WRL::ComPtr;

class Game;

class Window
{
public:
	static const UINT BufferCount = 3;

	HWND GetWindowHandle() const;

	void Destroy();

	const std::wstring& GetWindowName() const;

	int GetClientWidth() const;
	int GetClientHeight() const;

	bool IsVSync() const;
	void SetVSync(bool vSync);
	void ToggleVSync();

	bool IsFullScreen() const;

	void SetFullscreen(bool fullscreen);
	void ToggleFullscreen();

	void Show();
	void Hide();

	UINT GetCurrentBackBufferIndex() const;

	UINT Present();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView() const;

	ComPtr<ID3D12Resource> GetCurrentBackBuffer() const;

protected:
	friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	friend class Application;
	friend class Game;

	Window() = delete;
	Window(HWND hwnd, const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync);
	virtual ~Window();

	void RegisterCallbacks(std::shared_ptr<Game> pGame);

	virtual void OnUpdate(UpdateEventArgs& e);
	virtual void OnRender(RenderEventArgs& e);

	virtual void OnKeyPressed(KeyEventArgs& e);
	virtual void OnKeyReleased(KeyEventArgs& e);

	virtual void OnMouseMoved(MouseMotionEventArgs& e);
	virtual void OnMouseButtonPressed(MouseButtonEventArgs& e);
	virtual void OnMouseButtonReleased(MouseButtonEventArgs& e);
	virtual void OnMouseWheel(MouseWheelEventArgs& e);

	virtual void OnResize(ResizeEventArgs& e);

	ComPtr<IDXGISwapChain4> CreateSwapChain();

	void UpdateRenderTargetViews();

private:
	// Windows should not be copied.
	Window(const Window& copy) = delete;
	Window& operator=(const Window& other) = delete;

	HWND mHwnd;

	std::wstring mWindowName;

	int mClientWidth;
	int mClientHeight;
	bool mVSync;
	bool mFullscreen;

	HighResolutionClock mUpdateClock;
	HighResolutionClock mRenderClock;
	uint64_t mFrameCounter;

	std::weak_ptr<Game> mpGame;

	ComPtr<IDXGISwapChain4> mSwapChain;
	ComPtr<ID3D12DescriptorHeap> mRTVDescriptorHeap;
	ComPtr<ID3D12Resource> mBackBuffers[BufferCount];

	UINT mRTVDescriptorSize;
	UINT mCurrentBackBufferIndex;

	RECT mWindowRect;
	bool mIsTearingSupported;
};