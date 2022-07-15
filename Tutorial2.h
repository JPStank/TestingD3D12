#pragma once

#include "Game.h"
#include "Window.h"

#include <DirectXMath.h>

class Tutorial2 : public Game
{
public:
	using super = Game;
	Tutorial2(const std::wstring& name, int width, int height, bool vSync = false);

	virtual bool LoadContent() override;
	virtual void UnloadContent() override;

protected:
	virtual void OnUpdate(UpdateEventArgs& e) override;
	virtual void OnRender(RenderEventArgs& e) override;

	virtual void OnKeyPressed(KeyEventArgs& e) override;
	virtual void OnMouseWheel(MouseWheelEventArgs& e) override;


	virtual void OnResize(ResizeEventArgs& e) override;

private:
	void TransitionResource(ComPtr<ID3D12GraphicsCommandList2> commandList,
		ComPtr<ID3D12Resource> resource,
		D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

	void ClearRTV(ComPtr<ID3D12GraphicsCommandList2> commandList,
		D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor);

	void ClearDepth(ComPtr<ID3D12GraphicsCommandList2> commandList,
		D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth = 1.0f);

	void UpdateBufferResource(ComPtr<ID3D12GraphicsCommandList2> commandList,
		ID3D12Resource** pDestinationResource, ID3D12Resource** pIntermediateResource,
		size_t numElements, size_t elementSize, const void* bufferData,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

	void ResizeDepthBuffer(int width, int height);

	uint64_t mFenceValues[Window::BufferCount] = {};

	// Vertex buffer for the cube.
	ComPtr<ID3D12Resource> mVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
	// Index buffer for the cube.
	ComPtr<ID3D12Resource> mIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView{};

	// Depth buffer.
	ComPtr<ID3D12Resource> mDepthBuffer;
	// Descriptor heap for depth buffer.
	ComPtr<ID3D12DescriptorHeap> mDSVHeap;

	// Root signature
	ComPtr<ID3D12RootSignature> mRootSignature;

	// Pipeline state object.
	ComPtr<ID3D12PipelineState> mPipelineState;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	float mFoV;

	DirectX::XMMATRIX mModelMatrix{};
	DirectX::XMMATRIX mViewMatrix{};
	DirectX::XMMATRIX mProjectionMatrix{};

	bool mContentLoaded;
};
