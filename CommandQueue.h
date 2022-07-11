#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <cstdint>
#include <queue>

using namespace Microsoft::WRL;

class CommandQueue
{
public:
	CommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
	virtual ~CommandQueue();

	ComPtr<ID3D12GraphicsCommandList2> GetCommandList();
	uint64_t ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList);
	uint64_t Signal();
	bool IsFenceComplete(uint64_t fenceValue);
	void WaitForFenceValue(uint64_t fenceValue);
	void Flush();
	ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;

protected:

	ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
	ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(ComPtr<ID3D12CommandAllocator> allocator);

private:

	struct CommandAllocatorEntry
	{
		uint64_t fenceValue;
		ComPtr<ID3D12CommandAllocator> commandAllocator;
	};

	using CommandAllocatorQueue = std::queue<CommandAllocatorEntry>;
	using CommandListQueue = std::queue<ComPtr<ID3D12GraphicsCommandList2>>;

	D3D12_COMMAND_LIST_TYPE		mCommandListType;
	ComPtr<ID3D12Device2>		mDevice;
	ComPtr<ID3D12CommandQueue>	mCommandQueue;
	ComPtr<ID3D12Fence>			mFence;
	HANDLE						mFenceEvent;
	uint64_t					mFenceValue;
	CommandAllocatorQueue		mCommandAllocatorQueue;
	CommandListQueue			mCommandListQueue;
};