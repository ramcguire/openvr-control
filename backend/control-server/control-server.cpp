// websocket-backend.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include "VolumeControl.h"
#include "DeviceWrapper.h"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

using std::cout;
using std::endl;
using std::move;
using std::string;

GUID g_guidMyContext = GUID_NULL;

void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg, DeviceWrapper* wrapper) {
	string payload = msg->get_payload();

	if (payload == "stop-listening") {
		s->stop_listening();
		cout << "Stopped listening" << endl;
	}
	else {
		Document d;
		d.Parse(payload.c_str());
		if (d["type"] == "VOL") {
			// change volume
			wrapper->SetMasterVolume(d["value"].GetFloat() / 100);
		}
		else if (d["type"] == "MIC") {
			wrapper->SetMicVolume(d["value"].GetFloat() / 100);
		}
		else if (d["type"] == "MUTE_VOL") {
			wrapper->SetMasterMute(d["value"].GetBool());
		}
		else if (d["type"] == "MUTE_MIC") {
			wrapper->SetMicMute(d["value"].GetBool());
		}
	}
}

void on_connect(server* s, websocketpp::connection_hdl hdl, /*Volume vol CVolumeMonitor* vol*/ DeviceWrapper* wrapper) {
	VOLUME_INFO* info = new VOLUME_INFO();
	//vol->GetLevelInfo(info);
	//vol->AttachWebsocket(hdl);
	//vol->DispatchWebsocketMessage(vol->GetLevelInfoJson());
	wrapper->AttachWebsocket(hdl);
	wrapper->GetDeviceState();
}

int main() {
	cout << "Hello World" << endl;
	HRESULT hr = S_OK;
	hr = CoInitialize(NULL);
	hr = CoCreateGuid(&g_guidMyContext);
	// check for error?

	server s;
	//CVolumeMonitor* vol = new CVolumeMonitor(&s, TRUE);
	//vol->Initialize();
	DeviceWrapper* wrapper = new DeviceWrapper(&s);

	s.set_message_handler(bind(&on_message, &s, ::_1, ::_2, wrapper));
	s.set_open_handler(bind(&on_connect, &s, ::_1, wrapper));
	s.set_access_channels(websocketpp::log::alevel::all);
	s.set_error_channels(websocketpp::log::elevel::all);

	s.init_asio();
	s.listen(8080);
	cout << "Server listening on 8080" << endl;
	s.start_accept();

	s.run();

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
