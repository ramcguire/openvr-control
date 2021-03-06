// App.js
import React, { Component } from "react";
import "./App.css";
import { connect, sendMsg } from "./api";
import GenericSlider from "./components/generic-slider";
import Button from "@material-ui/core/Button";

class App extends Component {
  constructor(props) {
    super(props);
    connect();
  }

  send() {
    console.log("hello");
    sendMsg("hello");
  }

  render() {
    return (
      <div className="App">
        <Button variant="contained" onClick={this.send}>
          Hit
        </Button>
        <br></br>
        <GenericSlider volume={true}></GenericSlider>
        <br></br>
        <GenericSlider volume={false}></GenericSlider>
      </div>
    );
  }
}

export default App;