// App.js
import React, { Component } from "react";
import "./App.css";
import { SocketContext, socket } from "./api/socket";
import GenericSlider from "./components/generic-slider";
import ConnectMenu from "./components/connect";

class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      connected: socket.connected,
      connecting: false,
      error: false,
    };
    this.connect = this.connect.bind(this);
  }

  connect() {
    socket.connect();
    this.setState({
      connecting: true,
    });
    socket.on("connect", () => {
      console.log("connected to server");
      this.setState({
        connected: socket.connected,
        connecting: false,
      });
    });
  }

  render() {
    return (
      <div className="App">
        <SocketContext.Provider value={socket}>
          <ConnectMenu
            text="test"
            socket={this.socket}
            connecting={this.state.connecting}
            connected={this.state.connected}
            error={this.state.error}
            connect={this.connect}
          />
          <br></br>
          <GenericSlider volume={true} connected={this.state.connected} />
          <br></br>
          <GenericSlider volume={false} connected={this.state.connected} />
        </SocketContext.Provider>
      </div>
    );
  }
}

export default App;
