#pragma once
#define _WINSOCKAPI_

#undef  WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <stdio.h>
#include <iostream>
#include <audioclient.h>
#include <atlsync.h>
#include <atlbase.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;
using std::string;
using namespace rapidjson;

typedef struct {
    BOOL bMuted;
    float fMasterVolume;
} VOLUME_INFO;

class CVolumeMonitor : IMMNotificationClient, IAudioEndpointVolumeCallback
{
private:
    BOOL                            m_bRegisteredForEndpointNotifications;
    BOOL                            m_bRegisteredForVolumeNotifications;
    CComPtr<IMMDeviceEnumerator>    m_spEnumerator;
    CComPtr<IMMDevice>              m_spAudioEndpoint;
    CComPtr<IAudioEndpointVolume>   m_spVolumeControl;
    CCriticalSection                m_csEndpoint;

    long                            m_cRef;

    server* s;
    websocketpp::connection_hdl hdl;
    BOOL hasHandler;

    ~CVolumeMonitor();       // refcounted object... make the destructor private
    HRESULT AttachToDefaultEndpoint();
    void    DetachFromEndpoint();

    // IMMNotificationClient (only need to really implement OnDefaultDeviceChanged)
    IFACEMETHODIMP OnDeviceStateChanged(LPCWSTR /*pwstrDeviceId*/, DWORD /*dwNewState*/) { return S_OK; }
    IFACEMETHODIMP OnDeviceAdded(LPCWSTR /*pwstrDeviceId*/) { return S_OK; }
    IFACEMETHODIMP OnDeviceRemoved(LPCWSTR /*pwstrDeviceId*/) { return S_OK; }
    IFACEMETHODIMP OnPropertyValueChanged(LPCWSTR /*pwstrDeviceId*/, const PROPERTYKEY /*key*/) { return S_OK; }
    IFACEMETHODIMP OnDefaultDeviceChanged(EDataFlow, ERole, LPCWSTR);
    IFACEMETHODIMP OnDeviceQueryRemove() { return S_OK; }
    IFACEMETHODIMP OnDeviceQueryRemoveFailed() { return S_OK; }
    IFACEMETHODIMP OnDeviceRemovePending() { return S_OK; }

    // IAudioEndpointVolumeCallback
    IFACEMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);

    // IUnknown
    IFACEMETHODIMP QueryInterface(const IID& iid, void** ppUnk);

public:
    CVolumeMonitor(server*);

    HRESULT Initialize();
    void    Dispose();
    HRESULT GetLevelInfo(VOLUME_INFO* pInfo);
    void    ChangeEndpoint();
    void    SetMasterMute(const bool&);
    void    SetMasterVolume(const float&);
    void    AttachWebsocket(websocketpp::connection_hdl);
    string  GetLevelInfoJson();
    void    DispatchWebsocketMessage(string);

    // IUnknown
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();
};
