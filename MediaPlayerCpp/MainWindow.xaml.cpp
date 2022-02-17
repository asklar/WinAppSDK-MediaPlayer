#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif


using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MediaPlayerCpp::implementation
{
  void MainWindow::CreateSwapChainInternal() {
    

    const D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    unsigned int flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG;

    winrt::check_hresult(D3D11CreateDevice(
      nullptr,
      D3D_DRIVER_TYPE_HARDWARE,
      nullptr,
      flags,
      featureLevels,
      ARRAYSIZE(featureLevels),
      D3D11_SDK_VERSION,
      m_d3dDevice.put(),
      nullptr,
      nullptr
    ));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
    swapChainDesc.Width = (UINT)swapchain().Width();
    swapChainDesc.Height = (UINT)swapchain().Height();
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    swapChainDesc.Flags = 0;

    auto dxgiDevice = m_d3dDevice.as<IDXGIDevice>();
    winrt::com_ptr<IDXGIAdapter> dxgiAdapter;
    winrt::check_hresult(dxgiDevice->GetAdapter(dxgiAdapter.put()));
    winrt::com_ptr<IDXGIFactory2> dxgiFactory;
    winrt::check_hresult(dxgiAdapter->GetParent(IID_IDXGIFactory2, dxgiFactory.put_void()));
    
    winrt::check_hresult(dxgiFactory->CreateSwapChainForComposition(m_d3dDevice.get(), &swapChainDesc, nullptr, m_swapchain.put()));

    auto panelNative = swapchain().as<ISwapChainPanelNative>();
    winrt::check_hresult(panelNative->SetSwapChain(m_swapchain.get()));


  }
    MainWindow::MainWindow()
    {
        InitializeComponent();
        m_player = winrt::Windows::Media::Playback::MediaPlayer();
        m_player.SetUriSource(Windows::Foundation::Uri{ L"http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4" });
        m_player.SetSurfaceSize(Windows::Foundation::Size{ 600, 400 });
        m_player.IsVideoFrameServerEnabled(true);
        m_player.VideoFrameAvailable([this](auto&...) {
          winrt::com_ptr<IDXGISurface> surface;
          winrt::check_hresult(m_swapchain->GetBuffer(0, IID_IDXGISurface, surface.put_void()));
          winrt::com_ptr<::IInspectable> isurface{ nullptr };
          winrt::check_hresult(::CreateDirect3D11SurfaceFromDXGISurface(surface.get(), isurface.put()));
          auto d3dSurface = isurface.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface>();
          m_player.CopyFrameToVideoSurface(d3dSurface);

          DXGI_PRESENT_PARAMETERS presentParam = { 0, nullptr, nullptr, nullptr };
          winrt::check_hresult(m_swapchain->Present1(1, 0, &presentParam));


          });
        CreateSwapChainInternal();
        
    }
    void MainWindow::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
      m_playing = !m_playing;
      if (m_playing) {
        playPause().Symbol(Controls::Symbol::Pause);
        m_player.Play();
      }
      else {
        playPause().Symbol(Controls::Symbol::Play);
        m_player.Pause();
      }
        
    }
}
