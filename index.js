var express = require("express");
var path = require("path");
var app = express();
app.use(express.static("public"));
app.use("/images", express.static(__dirname + "public/images"));
app.set("view engine", "ejs");
app.set("views", "./views");
var server = require("http").Server(app);
var io = require("socket.io").listen(server);
var myport = process.env.PORT || 3000;
server.listen(myport);

io.on("connection", function (socket) {
  console.log("Có người truy cập id:" + socket.id);
  socket.on("disconnect", function () {
    console.log(socket.id + " ngắt kết nối server!!!!");
  });


  socket.on("espConnect", function (data) {
    console.log(data);
	socket.emit('espConnected','Done')
	//esp---------------------------------
  });
  socket.on("HenDen", function (data) {
    console.log(data);
	//esp---------------------------------
  });
  socket.on("tathengioden", function (data) {
    console.log(data);
	//esp---------------------------------
  });




  // lắng nghe sự kiện app flutter
  socket.on("flutter", function (data) {
    console.log("AppFlutter id:" + socket.id + " Sendata:" + data);
    // gửi xuống cho esp
    io.emit("ESP", data);
  });

  // lắng nghe sự kiện ESP
  socket.on("ESPJSON", function (data) {
    var DataJson = JSON.stringify(data);

    console.log("Data ESP JSON: " + DataJson);

    // gửi  web
    io.emit("web", DataJson);
    // gửi app
    io.emit("app", DataJson);
  });
});
app.get("/", function (req, res) {
  //res.render("html"); hoặc
  res.sendFile(path.join(__dirname, "./views/new.html"));
});
