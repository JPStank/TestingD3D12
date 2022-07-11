#include "pch.h"
#include "Application.h"
#include "CommandQueue.h"
#include "Window.h"
#include "Game.h"

Window::Window(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync)
	: mHwnd(hWnd)
	, mWindowName(windowName)
	, mClientWidth(clientWidth)
	, mClientHeight(clientHeight)
	, mVSync(vSync)
	, mFullscreen(false)
	, mFrameCounter(0)
{
	Application& app = Application::Get();

	mIsTearingSupported = app.IsTearingSupported();

	mSwapChain = CreateSwapChain();
	mRTVDescriptorHeap = app.CreateDescriptorHeap(BufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mRTVDescriptorSize = app.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	UpdateRenderTargetViews();
}

Window::~Window()
{
	// Window should be destroyed with Application::DestroyWindow before
	// the window goes out of scope.
	assert(!mHwnd && "Use Application::DestroyWindow before destruction.");
}

HWND Window::GetWindowHandle() const
{
	return mHwnd;
}

const std::wstring& Window::GetWindowName() const
{
	return mWindowName;
}

void Window::Show()
{
	::ShowWindow(mHwnd, SW_SHOW);
}

/**
* Hide the window.
*/
void Window::Hide()
{
	::ShowWindow(mHwnd, SW_HIDE);
}

void Window::Destroy()
{
	if (auto pGame = mpGame.lock())
	{
		// Notify the registered game that the window is being destroyed.
		pGame->OnWindowsDestroy();
	}
	if (mHwnd)
	{
		DestroyWindow(mHwnd);
		mHwnd = nullptr;
	}
}

int Window::GetClientWidth() const
{
	return mClientWidth;
}

int Window::GetClientHeight() const
{
	return mClientHeight;
}

bool Window::IsVSync() const
{
	return mVSync;
}

void Window::SetVSync(bool vSync)
{
	mVSync = vSync;
}

void Window::ToggleVSync()
{
	SetVSync(!mVSync);
}

bool Window::IsFullScreen() const
{
	return mFullscreen;
}

// Set the fullscreen state of the window.
void Window::SetFullscreen(bool fullscreen)
{
	if (mFullscreen != fullscreen)
	{
		mFullscreen = fullscreen;

		if (mFullscreen)
		{
			::GetWindowRect(mHwnd, &mWindowRect);

			UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

			::SetWindowLongW(mHwnd, GWL_STYLE, windowStyle);

			HMONITOR hMonitor = ::MonitorFromWindow(mHwnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			::GetMonitorInfo(hMonitor, &monitorInfo);

			::SetWindowPos(mHwnd, HWND_TOPMOST,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(mHwnd, SW_MAXIMIZE);
		}
		else
		{
			// Restore all the window decorators.
			::SetWindowLong(mHwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

			::SetWindowPos(mHwnd, HWND_NOTOPMOST,
				mWindowRect.left,
				mWindowRect.top,
				mWindowRect.right - mWindowRect.left,
				mWindowRect.bottom - mWindowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(mHwnd, SW_NORMAL);
		}
	}
}

void Window::ToggleFullscreen()
{
	SetFullscreen(!mFullscreen);
}


void Window::RegisterCallbacks(std::shared_ptr<Game> pGame)
{
	mpGame = pGame;

	return;
}

void Window::OnUpdate(UpdateEventArgs&)
{
	mUpdateClock.Tick();

	if (auto pGame = mpGame.lock())
	{
		mFrameCounter++;

		UpdateEventArgs updateEventArgs(mUpdateClock.GetDeltaSeconds(), mUpdateClock.GetTotalSeconds());
		pGame->OnUpdate(updateEventArgs);
	}
}

void Window::OnRender(RenderEventArgs&)
{
	mRenderClock.Tick();

	if (auto pGame = mpGame.lock())
	{
		RenderEventArgs renderEventArgs(mRenderClock.GetDeltaSeconds(), mRenderClock.GetTotalSeconds());
		pGame->OnRender(renderEventArgs);
	}
}

void Window::OnKeyPressed(KeyEventArgs& e)
{
	if (auto pGame = mpGame.lock())
	{
		pGame->OnKeyPressed(e);
	}
}

void Window::OnKeyReleased(KeyEventArgs& e)
{
	if (auto pGame = mpGame.lock())
	{
		pGame->OnKeyReleased(e);
	}
}

// The mouse was moved
void Window::OnMouseMoved(MouseMotionEventArgs& e)
{
	if (auto pGame = mpGame.lock())
	{
		pGame->OnMouseMoved(e);
	}
}

// A button on the mouse was pressed
void Window::OnMouseButtonPressed(MouseButtonEventArgs& e)
{
	if (auto pGame = mpGame.lock())
	{
		pGame->OnMouseButtonPressed(e);
	}
}

// A button on the mouse was released
void Window::OnMouseButtonReleased(MouseButtonEventArgs& e)
{
	if (auto pGame = mpGame.lock())
	{
		pGame->OnMouseButtonReleased(e);
	}
}

// The mouse wheel was moved.
void Window::OnMouseWheel(MouseWheelEventArgs& e)
{
	if (auto pGame = mpGame.lock())
	{
		pGame->OnMouseWheel(e);
	}
}

void Window::OnResize(ResizeEventArgs& e)
{
	// Update the client size.
	if (mClientWidth != e.Width || mClientHeight != e.Height)
	{
		mClientWidth = std::max(1, e.Width);
		mClientHeight = std::max(1, e.Height);

		Application::Get().Flush();

		for (int i = 0; i < BufferCount; ++i)
		{
			mBackBuffers[i].Reset();
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ThrowIfFailed(mSwapChain->GetDesc(&swapChainDesc));
		ThrowIfFailed(mSwapChain->ResizeBuffers(BufferCount, mClientWidth,
			mClientHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

		mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

		UpdateRenderTargetViews();
	}

	if (auto pGame = mpGame.lock())
	{
		pGame->OnResize(e);
	}
}

ComPtr<IDXGISwapChain4> Window::CreateSwapChain()
{
	Application& app = Application::Get();

	ComPtr<IDXGISwapChain4> dxgiSwapChain4;
	ComPtr<IDXGIFactory4> dxgiFactory4;
	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = mClientWidth;
	swapChainDesc.Height = mClientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = BufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	swapChainDesc.Flags = mIsTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	ID3D12CommandQueue* pCommandQueue = app.GetCommandQueue()->GetD3D12CommandQueue().Get();

	ComPtr<IDXGISwapChain1> swapChain1;
	ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
		pCommandQueue,
		mHwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1));

	// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
	// will be handled manually.
	ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(mHwnd, DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed(swapChain1.As(&dxgiSwapChain4));

	mCurrentBackBufferIndex = dxgiSwapChain4->GetCurrentBackBufferIndex();

	return dxgiSwapChain4;
}

// Update the render target views for the swapchain back buffers.
void Window::UpdateRenderTargetViews()
{
	auto device = Application::Get().GetDevice();

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < BufferCount; ++i)
	{
		ComPtr<ID3D12Resource> backBuffer;
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

		device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

		mBackBuffers[i] = backBuffer;

		rtvHandle.Offset(mRTVDescriptorSize);
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE Window::GetCurrentRenderTargetView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrentBackBufferIndex, mRTVDescriptorSize);
}

ComPtr<ID3D12Resource> Window::GetCurrentBackBuffer() const
{
	return mBackBuffers[mCurrentBackBufferIndex];
}

UINT Window::GetCurrentBackBufferIndex() const
{
	return mCurrentBackBufferIndex;
}

UINT Window::Present()
{
	UINT syncInterval = mVSync ? 1 : 0;
	UINT presentFlags = mIsTearingSupported && !mVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
	ThrowIfFailed(mSwapChain->Present(syncInterval, presentFlags));
	mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	return mCurrentBackBufferIndex;
}