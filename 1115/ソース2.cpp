#include <d3d12.h>
#include <dxgi1_4.h>
#include <stdio.h>
#include <wchar.h> 

class DIRECTX12
{
	IDXGIFactory4* CreateDXGIFactory()
	{
		IDXGIFactory4* factory;
		UINT createFactoryFlags = 0;
			
#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif // defined(_DEBUG)

		HRESULT hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory));
		if (FAILED(hr))
		{
			OutputDebugString("Failed to create DXGI Factory\n");
			return nullptr;
		}

		return factory;
	}

	IDXGIAdapter* GetHardwareAdapter(IDXGIFactory4* factory)
	{
		IDXGIAdapter1* adapter;

		for (UINT adapterIndex = 0; ; ++adapterIndex)
		{
			adapter = nullptr;
			if (DXGI_ERROR_NOT_FOUND == factory->EnumAdapters1(adapterIndex, &adapter))
			{
				break;
			}
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				adapter->Release();
				continue;
			}

			if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
			{
				return adapter;
			}

			adapter->Release();
		}

		return nullptr;

	}

	ID3D12Device* CreateD3D12Device(IDXGIAdapter1* adapter)
	{
		ID3D12Device* device;

		HRESULT hr = D3D12CreateDevice
		(
			adapter,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&device)
		);
		if (FAILED(hr))
		{
			hr = D3D12CreateDevice(
				nullptr,
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&device)
			);
			if (FAILED(hr))
			{
				OutputDebugString("Failed to create D3D12 Device\n");
				return nullptr;
			}

			OutputDebugString("Using software adapter (WARP)\n");
		}

		return device;
	}
	ID3D12DescriptorHeap* CreateRTVDescriptorHeap(ID3D12Device* device, UINT numDescriptors) 
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = 2;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ID3D12DescriptorHeap* rtvHeap;
		device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		UINT rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		rtvHandle.ptr += rtvDescriptorSize;
	}
	ID3D12CommandAllocator* CreateCommandAllocator(ID3D12CommandAllocator* CommandAllocator)
	{
		ID3D12CommandAllocator* commandAllocators[2];

		for (int i = 0; i < 2; i++)
		{
			
		}
	}
	ID3D12CommandQueue* CreateCommandQueue(ID3D12Device* device)
	{
		// コマンドキューの設定
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;    // 直接実行型
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;  // 通常優先度
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;    // 特別フラグなし
		queueDesc.NodeMask = 0;                             // 単一GPU使用

		ID3D12CommandQueue* commandQueue;
		HRESULT hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

		if (FAILED(hr))
		{
			OutputDebugString("Failed to create Command Queue\n");
			return nullptr;
		}

		// デバッグ用の名前設定（任意だが推奨）
		commandQueue->SetName(L"Main Command Queue");

		return commandQueue;
	}
	ID3D12Resource* CreateRenderTargets(
		ID3D12Device* device,
		IDXGISwapChain3* swapChain,
		ID3D12DescriptorHeap* rtvHeap,
		UINT rtvDescriptorSize)
	{
		ID3D12Resource* renderTargets[2];

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
			rtvHeap->GetCPUDescriptorHandleForHeapStart();

		for (UINT i = 0; i < 2; i++)
		{
			HRESULT hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
			if (FAILED(hr))
			{
				OutputDebugString("Failed to get SwapChain Buffer\n");
				return nullptr;
			}

			device->CreateRenderTargetView(renderTargets[i], nullptr, rtvHandle);

			wchar_t name[64];
			swprintf_s(name,64,L"RenderTarget_%d", i);
			renderTargets[i]->SetName(name);

			rtvHandle.ptr += rtvDescriptorSize;
		}

		return renderTargets[0];
	}

	IDXGISwapChain3* CreateSwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* commandQueue, HWND hwnd)
	{
		// スワップチェーンの詳細設定
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = 2;                      // ダブルバッファリング
		swapChainDesc.Width = 1280;                         // 画面幅
		swapChainDesc.Height = 720;                         // 画面高
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ピクセルフォーマット
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // レンダーターゲット用
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;     // 高速切替
		swapChainDesc.SampleDesc.Count = 1;                 // マルチサンプリングなし

		IDXGISwapChain1* swapChain1;
		HRESULT hr = factory->CreateSwapChainForHwnd(
			commandQueue,       // コマンドキュー
			hwnd,              // ターゲットウィンドウ
			&swapChainDesc,    // 設定
			nullptr,           // フルスクリーン設定
			nullptr,           // 出力制限
			&swapChain1        // 作成されるスワップチェーン
		);

		if (FAILED(hr))
		{
			OutputDebugString("Failed to create Swap Chain\n");
			return nullptr;
		}

		// より高機能なインターフェースにキャスト
		IDXGISwapChain3* swapChain;
		hr = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain));
		swapChain1->Release();
		if (FAILED(hr))
		{
			OutputDebugString("Failed to cast to SwapChain3\n");
			return nullptr;
		}

		return swapChain;
	}
	void EnableDebugLayer()
	{
#if defined(_DEBUG)
		// デバッグインターフェースを取得
		ID3D12Debug* debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			// デバッグレイヤーを有効化
			debugController->EnableDebugLayer();

			// より詳細な検証を有効化（任意）
			ID3D12Debug1* debugController1;
			if (SUCCEEDED(debugController->QueryInterface(IID_PPV_ARGS(&debugController1))))
			{
				debugController1->SetEnableGPUBasedValidation(TRUE);
			}
		}
#endif
	}
	ID3D12Resource* CreteResource(ID3D12Resource* Resource)
	{
		ID3D12Resource* renderTargets[2];

		for (int i = 0; i < 2; i++)
		{
			
		}
	}
	ID3D12CommandAllocator* CreateCommandAllocators(ID3D12Device* device)
	{
		ID3D12CommandAllocator* commandAllocators[2];

		for (int i = 0; i < 2; i++)
		{
			HRESULT hr = device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(&commandAllocators[i])
			);

			if (FAILED(hr))
			{
				OutputDebugString("Failed to create Command Allocator\n");
				return nullptr;
			}

			wchar_t name[64];
			swprintf_s(name, L"CommandAllocator_%d", i);
			commandAllocators[i]->SetName(name);
		}

		// とりあえず 0番を返す（必要なら配列化してクラスのメンバ管理）
		return commandAllocators[0];
	}

};