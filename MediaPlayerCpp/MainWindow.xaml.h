#pragma once

#include "MainWindow.g.h"

namespace winrt::MediaPlayerCpp::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void myButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);

    private:
        winrt::Windows::Media::Playback::MediaPlayer m_player{ nullptr };
        void CreateSwapChainInternal();
        winrt::com_ptr<ID3D11Device> m_d3dDevice;
        winrt::com_ptr<IDXGISwapChain1> m_swapchain;
        bool m_playing{ false };
    };


}

namespace winrt::MediaPlayerCpp::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
