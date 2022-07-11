#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <memory>
#include <string>

class Window;
class Game;
class CommandQueue;

using Microsoft::WRL::ComPtr;

class Application
{
public:

	static void Create(HINSTANCE hInst);

	static void Destroy();
	static Application& Get();

	bool IsTearingSupported() const;

	std::shared_ptr<Window> CreateRenderWindow(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync = true);

	void DestroyWindow(const std::wstring& windowName);
	void DestroyWindow(std::shared_ptr<Window> window);

	std::shared_ptr<Window> GetWindowByName(const std::wstring& windowName);

	int Run(std::shared_ptr<Game> pGame);

	void Quit(int exitCode = 0);

	ComPtr<ID3D12Device2> GetDevice() const;
	std::shared_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;

	void Flush();

	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type);
	UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const;

protected:

	Application(HINSTANCE hInst);
	virtual ~Application();

	ComPtr<IDXGIAdapter4> GetAdapter(bool bUseWarp);
	ComPtr<ID3D12Device2> CreateDevice(ComPtr<IDXGIAdapter4> adapter);
	bool CheckTearingSupport();

private:
	Application(const Application& copy) = delete;
	Application& operator=(const Application& other) = delete;

	// The application instance handle that this application was created with.
	HINSTANCE mHinstance;

	ComPtr<IDXGIAdapter4> mAdapter;
	ComPtr<ID3D12Device2> mDevice;

	std::shared_ptr<CommandQueue> mDirectCommandQueue;
	std::shared_ptr<CommandQueue> mComputeCommandQueue;
	std::shared_ptr<CommandQueue> mCopyCommandQueue;

	bool mTearingSupported;
};