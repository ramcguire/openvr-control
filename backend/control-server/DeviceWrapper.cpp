#include "DeviceWrapper.h"

DeviceWrapper::DeviceWrapper(server* s) : s(s) {
	volume = new CVolumeMonitor(s, TRUE);
	microphone = new CVolumeMonitor(s, FALSE);

	volume->Initialize();
	microphone->Initialize();
}

DeviceWrapper::~DeviceWrapper() {
	volume->Dispose();
	microphone->Dispose();
	volume->Release();
	microphone->Release();
}

void DeviceWrapper::AttachWebsocket(websocketpp::connection_hdl handler) {
	volume->AttachWebsocket(handler);
	microphone->AttachWebsocket(handler);
	s->send(handler, "{\"text\": \"CVolumeMonitor attached to websocket\"}",
		websocketpp::frame::opcode::text);
}

void DeviceWrapper::SetMasterVolume(float value) {
	volume->SetMasterVolume(value);
}

void DeviceWrapper::SetMicVolume(float value) {
	microphone->SetMasterVolume(value);
}

void DeviceWrapper::SetMasterMute(BOOL mute) {
	volume->SetMasterMute(mute);
}

void DeviceWrapper::SetMicMute(BOOL mute) {
	microphone->SetMasterMute(mute);
}

void DeviceWrapper::GetDeviceState() {
	volume->DispatchWebsocketMessage(volume->GetLevelInfoJson());
	microphone->DispatchWebsocketMessage(microphone->GetLevelInfoJson());
}