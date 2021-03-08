const http = require("http").createServer();
const io = require("socket.io")(http, {
  cors: {
    origin: "http://localhost:3000",
    methods: ["GET", "POST"],
  },
});

io.on("connection", (socket) => {
  console.log("a user connected");
  // socket.onAny((eventName, ...args) => {
  //   console.log(eventName);
  //   console.log(args);
  // });
  socket.on("VOL_CHANGE", (...args) => {
    console.log(args);
  });
});


http.listen(8080, () => {
  console.log("listening on *:8080");
});
