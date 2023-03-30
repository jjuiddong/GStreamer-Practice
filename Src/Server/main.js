
var express = require('express')
var http = require('http')
const cors = require('cors');
var net = require('net');
var child = require('child_process');
require('log-timestamp');   //adds timestamp in console.log()

var app = express();
// app.use(cors())
app.use(express.static(__dirname + '/'));

var httpServer = http.createServer(app);
const port = 9001;  //change port number is required
var gstMuxer = undefined
var errCnt = 0
var clientCnt = 0

//send the html page which holds the video tag
app.get('/', function (req, res) {
    res.send('index.html');
});

//stop the connection
app.post('/stop', function (req, res) {
    console.log('Connection closed using /stop endpoint.');

    if (gstMuxer != undefined) {
        gstMuxer.kill();    //killing GStreamer Pipeline
        console.log(`After gstkill in connection`);
    }
    gstMuxer = undefined;
    res.end();
});

//send the video stream
app.get('/stream', function (req, res) {

    res.writeHead(200, {
        'Content-Type': 'video/webm',
    });

    var tcpServer = net.createServer(function (socket) {
        socket.on('data', function (data) {
            const result = res.write(data);
            if (result == false) {
                if (++errCnt > 2) {
                    // console.log('write fail')
                    // res.end()
                    // close web browser
                    if (gstMuxer) {
                        // console.log('kill gstreamer')
                        // gstMuxer.kill()
                        // gstMuxer = undefined
                    }
                }
            } else {
                errCnt = 0
            }
        });
        socket.on('close', function (had_error) {
            console.log('Socket closed.');
            res.end();
        });
    });

    tcpServer.maxConnections = 1;

    tcpServer.listen(function () {
        console.log("Connection started.");
        if (gstMuxer == undefined) {
            console.log("inside gstMuxer == undefined");
            // setTimeout(() => {
                var cmd = 'gst-launch-1.0';
                var args = getGstPipelineArguments(this);
                gstMuxer = child.spawn(cmd, args);
                errCnt = 0
    
                gstMuxer.stderr.on('data', onSpawnError);
                gstMuxer.on('exit', onSpawnExit);
                
            // }, 1000);
        }
        else {
            console.log("New GST pipeline rejected because gstMuxer != undefined.");
        }
    });

});

httpServer.listen(port);
console.log(`Camera Stream App listening at http://localhost:${port}`)

process.on('uncaughtException', function (err) {
    console.log(err);
});

//functions
function onSpawnError(data) {
    console.log(data.toString());
}

function onSpawnExit(code) {
    if (code != null) {
        console.log('GStreamer error, exit code ' + code);
    }
}

function getGstPipelineArguments(tcpServer) {
    //Replace 'videotestsrc', 'pattern=ball' with camera source in below GStreamer pipeline arguments.
    //Note: Every argument should be written in single quotes as done below
    var args =
        // videotestsrc 
        // [
        //     'videotestsrc', 'pattern=ball',
        //     '!', 'video/x-raw,width=320,height=240,framerate=100/1',
        //     '!', 'x264enc', 'bitrate=2000',
        //     '!', 'mp4mux', 'fragment-duration=10',
        //     '!', 'tcpclientsink', 'host=localhost',
        //     'port=' + tcpServer.address().port
        // ];

        // mp4 file
        // [
        //     'filesrc', 'location=sintel_video.mp4',
        //     '!', 'decodebin',
        //     '!', 'x264enc', 'tune=zerolatency', 'bitrate=2000', 'speed-preset=superfast',
        //     '!', 'mp4mux', 'fragment-duration=10',
        //     '!', 'tcpclientsink', 'host=localhost',
        //     'port=' + tcpServer.address().port
        // ];

        // camera streaming
        [
            'mfvideosrc',
            '!', 'videoconvert',
            '!', 'video/x-raw,width=640,height=480',
            '!', 'x264enc', 'tune=zerolatency', 'bitrate=2000', 'speed-preset=superfast',
            '!', 'mp4mux', 'fragment-duration=10',
            '!', 'tcpclientsink', 'host=localhost',
            'port=' + tcpServer.address().port
        ];

    return args;
}