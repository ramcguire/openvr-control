import io from "socket.io-client";
import React from "react";

export const socket = io("http://localhost:8080", {
  autoConnect: false,
});

export const SocketContext = React.createContext();
