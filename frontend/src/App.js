// App.js
import React, { Component } from "react";
import "./App.css";
import { SocketContext } from "./api/socket";
import GenericSlider from "./components/generic-slider";
import ConnectMenu from "./components/connect";

class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      connected: false,
      connecting: false,
      error: false,
      volume: 50,
      vMuted: false,
      mVolume: 50,
      mMute: false,
    };
    this.connect = this.connect.bind(this);
    this.handleVolumeChange = this.handleVolumeChange.bind(this);
    this.handleVolumeMute = this.handleVolumeMute.bind(this);
    this.handleMicMute = this.handleMicMute.bind(this);
    this.handleMicChange = this.handleMicChange.bind(this);
    this.handleMessage = this.handleMessage.bind(this);

    this.socket = null;
  }

  handleVolumeChange(value) {
    this.setState({
      volume: value,
    });
  }

  handleVolumeMute(value) {
    this.setState({
      vMuted: value,
    });
  }

  handleMicChange(value) {
    this.setState({
      mVolume: value,
    });
  }

  handleMicMute(value) {
    this.setState({
      mMuted: value,
    });
  }

  handleMessage(message) {
    let msg = JSON.parse(message.data);
    console.log(msg);
    if (msg.state) {
      if (msg.type === "v") {
        this.setState({
          volume: msg.vVolume,
          vMuted: msg.vMuted,
        });
      } else if (msg.type === "m") {
        this.setState({
          mVolume: msg.mVolume,
          mMuted: msg.mMuted,
        });
      }
      
    }
  }

  connect() {
    this.setState({
      connecting: true,
    });
    if (this.socket === null) {
      this.socket = new WebSocket("ws://127.0.0.1:8080");
    }
    this.socket.onopen = (event) => {
      this.setState({
        connected: this.socket.readyState === 1,
        connecting: false,
      });
    };
    this.socket.onmessage = (event) => {
      this.handleMessage(event);
    };
  }

  render() {
    return (
      <div className="App">
        <SocketContext.Provider value={this.socket}>
          <ConnectMenu
            socket={this.socket}
            connecting={this.state.connecting}
            connected={this.state.connected}
            error={this.state.error}
            connect={this.connect}
          />
          <br></br>
          <GenericSlider
            volume={true}
            value={this.state.volume}
            muted={this.state.vMuted}
            setValue={this.handleVolumeChange}
            setMute={this.handleVolumeMute}
            connected={this.state.connected}
          />
          <br></br>
          <GenericSlider
            volume={false}
            value={this.state.mVolume}
            muted={this.state.mMuted}
            setValue={this.handleMicChange}
            setMute={this.handleMicMute}
            connected={this.state.connected}
          />
        </SocketContext.Provider>
      </div>
    );
  }
}

export default App;
