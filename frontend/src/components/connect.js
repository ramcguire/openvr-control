import React, { useEffect, useContext } from "react";
import { SocketContext } from "../api/socket";
import clsx from "clsx";
import CircularProgress from "@material-ui/core/CircularProgress";
import Fab from "@material-ui/core/Fab";
import ImportExportIcon from "@material-ui/icons/ImportExport";
import ErrorIcon from "@material-ui/icons/Error";
import { makeStyles } from "@material-ui/core/styles";
import { green } from "@material-ui/core/colors";
import DoneIcon from "@material-ui/icons/Done";

const useStyles = makeStyles((theme) => ({
  root: {
    display: "flex",
    alignItems: "center",
  },
  wrapper: {
    margin: theme.spacing(1),
    position: "relative",
  },
  buttonSuccess: {
    backgroundColor: green[500],
    "&:hover": {
      backgroundColor: green[700],
    },
  },
  fabProgress: {
    color: green[500],
    position: "absolute",
    top: -6,
    left: -6,
    zIndex: 1,
  },
}));

export default function ConnectMenu(props) {
  const classes = useStyles();

  const buttonClassname = clsx({
    [classes.buttonSuccess]: props.connected,
  });

  const socket = useContext(SocketContext);

  // React.useEffect(() => {
  //   return () => {
  //     clearTimeout(timer.current);
  //   };
  // }, []);

  const handleButtonClick = () => {
    if (!props.connecting) {
      // open the socket
      props.connect();
    }
  };

  return (
    <div className={classes.root}>
      <div className={classes.wrapper}>
        <Fab
          aria-label="save"
          color="primary"
          className={buttonClassname}
          onClick={handleButtonClick}
        >
          {props.connected ? (
            <DoneIcon />
          ) : props.error ? (
            <ErrorIcon />
          ) : (
            <ImportExportIcon />
          )}
        </Fab>
        {props.connecting && (
          <CircularProgress size={68} className={classes.fabProgress} />
        )}
      </div>
    </div>
  );
}
