#include <d3d12.h>
#include <dxgi1_4.h>
#include <stdio.h>
#include <iostream>
#include <wchar.h>
#include <cassert>
#include <windows.h>
#include "../clear/ソース1.h"
#include "../clear/ソース2.h"

class DX12App {
public:
	DX12App() : hWnd(NULL), width(800), height(600),
		device(nullptr), commandQueue(nullptr),
		swapChain(nullptr), rtvHeap(nullptr),
		frameIndex(0) {
	}
	bool Initialize(HINSTANCE hInstance, int nCmdShow);
	void Run();
	void Cleanup();
private:
	HWND hWnd;
	UINT width;
	UINT height;
	ID3D12Device* device;
	ID3D12CommandQueue* commandQueue;
	IDXGISwapChain3* swapChain;
	ID3D12DescriptorHeap* rtvHeap;
	UINT rtvDescriptorSize;
	ID3D12Resource* renderTargets[2];
	ID3D12CommandAllocator* commandAllocator;
	ID3D12GraphicsCommandList* commandList;
	UINT frameIndex;
	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	app.loop();
};
