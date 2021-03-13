#include "VolumeControl.h"

extern GUID g_guidMyContext;

CVolumeMonitor::CVolumeMonitor(server* s)
    : m_bRegisteredForEndpointNotifications(FALSE),
    m_bRegisteredForVolumeNotifications(FALSE),
    m_cRef(1),
    s(s),
    hasHandler(FALSE) {}

CVolumeMonitor::~CVolumeMonitor() { Dispose(); }

// ----------------------------------------------------------------------
//  Call when the app is done with this object before calling release.
//  This detaches from the endpoint and releases all audio service references.
//
// ----------------------------------------------------------------------
void CVolumeMonitor::Dispose() {
    DetachFromEndpoint();

    if (m_bRegisteredForEndpointNotifications) {
        m_spEnumerator->UnregisterEndpointNotificationCallback(this);
        m_bRegisteredForEndpointNotifications = FALSE;
    }
}

// ----------------------------------------------------------------------
//  Initialize this object.  Call after constructor.
//
// ----------------------------------------------------------------------
HRESULT CVolumeMonitor::Initialize() {
    HRESULT hr;

    // create enumerator
    hr = m_spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    if (SUCCEEDED(hr)) {
        hr = m_spEnumerator->RegisterEndpointNotificationCallback(this);
        if (SUCCEEDED(hr)) {
            hr = AttachToDefaultEndpoint();
        }
    }

    return hr;
}

// ----------------------------------------------------------------------
//  Called from the UI thread when the volume is changed (see OSD.cpp
//  WM_VOLUMECHANGE handler)
//
// ----------------------------------------------------------------------
HRESULT CVolumeMonitor::GetLevelInfo(VOLUME_INFO* pInfo) {
    HRESULT hr = E_FAIL;

    m_csEndpoint.Enter();

    if (m_spVolumeControl != NULL) {
        hr = m_spVolumeControl->GetMute(&pInfo->bMuted);
        if (SUCCEEDED(hr)) {
            hr = m_spVolumeControl->GetMasterVolumeLevelScalar(&pInfo->fMasterVolume);
        }
    }

    m_csEndpoint.Leave();

    return hr;
}

// ----------------------------------------------------------------------
//  Start monitoring the current default device
//
// ----------------------------------------------------------------------
HRESULT CVolumeMonitor::AttachToDefaultEndpoint() {
    m_csEndpoint.Enter();

    // get the default music & movies playback device
    HRESULT hr = m_spEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia,
        &m_spAudioEndpoint);
    if (SUCCEEDED(hr)) {
        // get the volume control for it
        hr = m_spAudioEndpoint->Activate(__uuidof(m_spVolumeControl),
            CLSCTX_INPROC_SERVER, NULL,
            (void**)&m_spVolumeControl);
        if (SUCCEEDED(hr)) {
            // register for callbacks
            hr = m_spVolumeControl->RegisterControlChangeNotify(this);
            m_bRegisteredForVolumeNotifications = SUCCEEDED(hr);
        }
    }

    m_csEndpoint.Leave();

    return hr;
}

// ----------------------------------------------------------------------
//  Stop monitoring the device and release all associated references
//
// ----------------------------------------------------------------------
void CVolumeMonitor::DetachFromEndpoint() {
    m_csEndpoint.Enter();

    if (m_spVolumeControl != NULL) {
        // be sure to unregister...
        if (m_bRegisteredForVolumeNotifications) {
            m_spVolumeControl->UnregisterControlChangeNotify(this);
            m_bRegisteredForVolumeNotifications = FALSE;
        }

        m_spVolumeControl.Release();
    }

    if (m_spAudioEndpoint != NULL) {
        m_spAudioEndpoint.Release();
    }

    m_csEndpoint.Leave();
}

// ----------------------------------------------------------------------
//  Call this from the UI thread when the default device changes
//
// ----------------------------------------------------------------------
void CVolumeMonitor::ChangeEndpoint() {
    DetachFromEndpoint();

    AttachToDefaultEndpoint();
}

// ----------------------------------------------------------------------
//  Implementation of IMMNotificationClient::OnDefaultDeviceChanged
//
//  When the user changes the default output device we want to stop monitoring
//  the former default and start monitoring the new default
//
// ----------------------------------------------------------------------
HRESULT CVolumeMonitor::OnDefaultDeviceChanged(EDataFlow flow, ERole role,
    LPCWSTR pwstrDefaultDeviceId) {
    ChangeEndpoint();
    return S_OK;
}

// ----------------------------------------------------------------------
//  Implementation of IAudioEndpointVolumeCallback::OnNotify
//
//  This is called by the audio core when anyone in any process changes the
//  volume or mute state for the endpoint we are monitoring
//
// ----------------------------------------------------------------------
HRESULT CVolumeMonitor::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) {
    // something changed outside of our context
    if (pNotify->guidEventContext != g_guidMyContext) {
        std::cout << "Something changed external" << std::endl;
        DispatchWebsocketMessage(GetLevelInfoJson());
    }

    return S_OK;
}

//  IUnknown methods
HRESULT CVolumeMonitor::QueryInterface(REFIID iid, void** ppUnk) {
    if ((iid == __uuidof(IUnknown)) || (iid == __uuidof(IMMNotificationClient))) {
        *ppUnk = static_cast<IMMNotificationClient*>(this);
    }
    else if (iid == __uuidof(IAudioEndpointVolumeCallback)) {
        *ppUnk = static_cast<IAudioEndpointVolumeCallback*>(this);
    }
    else {
        *ppUnk = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

ULONG CVolumeMonitor::AddRef() { return InterlockedIncrement(&m_cRef); }

ULONG CVolumeMonitor::Release() {
    long lRef = InterlockedDecrement(&m_cRef);
    if (lRef == 0) {
        delete this;
    }
    return lRef;
}

void CVolumeMonitor::SetMasterMute(const bool& mute) {
    m_spVolumeControl->SetMute(mute, &g_guidMyContext);
}

void CVolumeMonitor::SetMasterVolume(const float& value) {
    m_spVolumeControl->SetMasterVolumeLevelScalar(value, &g_guidMyContext);
}

void CVolumeMonitor::AttachWebsocket(websocketpp::connection_hdl handle) {
    hdl = handle;
    hasHandler = TRUE;
    s->send(hdl, "{\"text\": \"CVolumeMonitor attached to websocket\"}",
        websocketpp::frame::opcode::text);
}

string CVolumeMonitor::GetLevelInfoJson() {
    VOLUME_INFO info;
    GetLevelInfo(&info);

    Document d;
    d.SetObject();
    d.AddMember("vMuted", info.bMuted, d.GetAllocator());
    d.AddMember("vVolume", (int)(info.fMasterVolume * 100), d.GetAllocator());
    d.AddMember("state", true, d.GetAllocator());

    StringBuffer buffer;
    buffer.Clear();

    Writer<StringBuffer> writer(buffer);

    d.Accept(writer);

    return string(buffer.GetString(), buffer.GetSize());
}

void CVolumeMonitor::DispatchWebsocketMessage(string msg) {
    if (hasHandler == TRUE) {
        s->send(hdl, msg, websocketpp::frame::opcode::text);
    }
}