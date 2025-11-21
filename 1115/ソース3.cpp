/*#include <d3d12.h>
#include <dxgi1_4.h>

class DIRECTX12
{
public:

    IDXGIFactory4* CreateDXGIFactory()
    {
        IDXGIFactory4* factory;
        UINT createFactoryFlags = 0;

#if defined(_DEBUG)
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

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

        for (UINT adapterIndex = 0;; ++adapterIndex)
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

        HRESULT hr = D3D12CreateDevice(
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

    // -------------------------------
    //  RTV ディスクリプタヒープ作成
    // -------------------------------
    ID3D12DescriptorHeap* CreateRTVDescriptorHeap(ID3D12Device* device)
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = 2;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        ID3D12DescriptorHeap* rtvHeap;
        device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));

        return rtvHeap;
    }

    // -------------------------------
    // コマンドアロケータ（録画メモリ）
    // -------------------------------
    ID3D12CommandAllocator* CreateCommandAllocator(ID3D12Device* device)
    {
        ID3D12CommandAllocator* allocator;
        HRESULT hr = device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&allocator)
        );

        if (FAILED(hr))
        {
            OutputDebugString("Failed to create Command Allocator\n");
            return nullptr;
        }

        return allocator;
    }

    // -------------------------------
    // コマンドリスト（描画命令）
    // -------------------------------
    ID3D12GraphicsCommandList* CreateCommandList(
        ID3D12Device* device,
        ID3D12CommandAllocator* allocator)
    {
        ID3D12GraphicsCommandList* commandList;
        HRESULT hr = device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            allocator,
            nullptr,
            IID_PPV_ARGS(&commandList)
        );

        if (FAILED(hr))
        {
            OutputDebugString("Failed to create Command List\n");
            return nullptr;
        }

        // 最初は閉じておく（必要）
        commandList->Close();

        return commandList;
    }

    // -------------------------------
    // コマンドキュー
    // -------------------------------
    ID3D12CommandQueue* CreateCommandQueue(ID3D12Device* device)
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.NodeMask = 0;

        ID3D12CommandQueue* commandQueue;
        HRESULT hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

        if (FAILED(hr))
        {
            OutputDebugString("Failed to create Command Queue\n");
            return nullptr;
        }

        return commandQueue;
    }

    // -------------------------------
    // スワップチェーン
    // -------------------------------
    IDXGISwapChain3* CreateSwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* commandQueue, HWND hwnd)
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Width = 1280;
        swapChainDesc.Height = 720;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        IDXGISwapChain1* swapChain1;
        HRESULT hr = factory->CreateSwapChainForHwnd(
            commandQueue,
            hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1
        );

        if (FAILED(hr))
        {
            OutputDebugString("Failed to create Swap Chain\n");
            return nullptr;
        }

        IDXGISwapChain3* swapChain;
        swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain));
        swapChain1->Release();

        return swapChain;
    }

    // -------------------------------
    // Fence（同期）
    // -------------------------------
    ID3D12Fence* CreateFence(ID3D12Device* device)
    {
        ID3D12Fence* fence;
        HRESULT hr = device->CreateFence(
            0,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&fence)
        );

        if (FAILED(hr))
        {
            OutputDebugString("Failed to create Fence\n");
            return nullptr;
        }

        return fence;
    }

    // -------------------------------
    // レンダーターゲット（バックバッファ）
    // -------------------------------
    void CreateRenderTargets(
        ID3D12Device* device,
        IDXGISwapChain3* swapChain,
        ID3D12DescriptorHeap* rtvHeap,
        ID3D12Resource* renderTargets[2])
    {
        UINT rtvSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();

        for (UINT i = 0; i < 2; i++)
        {
            swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
            device->CreateRenderTargetView(renderTargets[i], nullptr, rtvHandle);
            rtvHandle.ptr += rtvSize;
        }
    }

};*/
