import React, { Component } from "react";
import Grid from "@material-ui/core/Grid";
import Typography from "@material-ui/core/Typography";
import Slider from "@material-ui/core/Slider";
import VolumeUp from "@material-ui/icons/VolumeUp";
import VolumeMuteIcon from "@material-ui/icons/VolumeMute";
import VolumeOffIcon from "@material-ui/icons/VolumeOff";
import MicIcon from "@material-ui/icons/Mic";
import MicNoneIcon from "@material-ui/icons/MicNone";
import MicOffIcon from "@material-ui/icons/MicOff";
import Button from "@material-ui/core/Button";

class GenericSlider extends Component {
  constructor(props) {
    super(props);
    this.state = {
      value: 50,
    };
  }

  handleChange = (event, newValue) => {
    this.setState({
      value: newValue,
    });
  };

  render() {
    return (
      <div
        className={{
          width: 200,
        }}
      >
        <Typography id="continuous-slider" gutterBottom>
          Volume
        </Typography>
        <Grid container spacing={2}>
          <Grid item>
            {this.props?.volume ? <VolumeMuteIcon /> : <MicNoneIcon />}
          </Grid>
          <Grid item xs>
            <Slider
              disabled={false}
              value={this.state.value}
              onChange={this.handleChange}
              aria-labelledby="continuous-slider"
            />
          </Grid>
          <Grid item>{this.props?.volume ? <VolumeUp /> : <MicIcon />}</Grid>
          <Grid item>
            <Button variant="contained" disabled={this.state.value === 0}>
              {this.props.volume ? <VolumeOffIcon /> : <MicOffIcon />}
            </Button>
          </Grid>
        </Grid>
      </div>
    );
  }
}

export default GenericSlider;
