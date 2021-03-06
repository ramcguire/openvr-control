import React, { useState, useContext, useEffect } from "react";
import { SocketContext } from "../api/socket";
import { makeStyles } from "@material-ui/core/styles";
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

// define the width of the slider
const useStyles = makeStyles({
  root: {
    width: 400,
  },
});

export default function GenericSlider(props) {
  const classes = useStyles();
  const socket = useContext(SocketContext);

  const handleChange = (_, newValue) => {
    // send updated slider value
    socket.send(
      JSON.stringify({
        type: props.volume ? "VOL" : "MIC",
        value: props.value,
      })
    );
    props.setValue(newValue);
  };

  const handleMute = (_) => {
    socket.send(
      JSON.stringify({
        type: props.volume ? "MUTE_VOL" : "MUTE_MIC",
        value: !props.muted,
      })
    );
    props.setMute(!props.muted);
  };

  // called when component is mounted
  // useEffect(() => {
  //   // get current volumes?
  //   // socket.emit(props.volume ? "VOLUME_SLIDER" : "MIC_SLIDER", value);
  // }, [socket, value, props.volume]);
  return (
    <div className={classes.root}>
      <Typography id="continuous-slider" gutterBottom>
        Volume
      </Typography>
      <Grid container spacing={2}>
        <Grid item>{props?.volume ? <VolumeMuteIcon /> : <MicNoneIcon />}</Grid>
        <Grid item xs>
          <Slider
            disabled={!props.connected}
            value={props.value}
            onChange={handleChange}
            aria-labelledby="continuous-slider"
          />
        </Grid>
        <Grid item>{props?.volume ? <VolumeUp /> : <MicIcon />}</Grid>
        <Grid item>
          <Button
            variant="contained"
            disabled={props.value === 0 || !props.connected}
            onClick={handleMute}
          >
            {props?.volume ? (props?.muted ? <VolumeUp /> : <VolumeOffIcon />) : (props?.muted ? <MicOffIcon /> : <MicIcon />) }
          </Button>
        </Grid>
      </Grid>
    </div>
  );
}
