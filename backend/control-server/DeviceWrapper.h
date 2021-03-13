#pragma once

#include "VolumeControl.h"

class DeviceWrapper {
private:
	CVolumeMonitor* volume;
	CVolumeMonitor* microphone;
	server* s;

public:
	DeviceWrapper(server*);
	~DeviceWrapper();
	void    AttachWebsocket(websocketpp::connection_hdl);
	void	SetMasterVolume(float);
	void	SetMicVolume(float);
	void	SetMasterMute(BOOL);
	void	SetMicMute(BOOL);
	void	GetDeviceState();
};
