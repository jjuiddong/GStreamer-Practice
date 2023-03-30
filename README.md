# GStreamer-Practice
GStreamer Practice with VS2019 C++

- Server
	- NodeJS + GStreamer video streaming example
	- 1. NodeJs Webserver wait to request
	- 2. Webbrowser Request, NodeJs Server send local camera streaming
- Test1
	- GStreamer simple example
- gst-docs-discontinued-for-monorepo
	- GStreamer tutorials
- gstwebrtc-demos
	- https://github.com/centricular/gstwebrtc-demos
	- webrtc signalling server example
		- simple_server.py
			- need python3
			- pip install websockets
			- remove 1 line code: #!/usr/bin/env python3
				- error occure
			- generate.sh cannot create cert.pem
				- run bash
				- run 
					- openssl req -x509 -newkey rsa:4096 -keyout ${OUTDIR}key.pem -out ${OUTDIR}cert.pem -days 365 -nodes
					- copy key.pem, cert.pem from user directory to project directory

- WebRTCTest
	- https://github.com/sampleref/gstreamer-cpp-example

- WebRTCTest2
	- reference source code
		- https://github.com/GStreamer/gstreamer/blob/main/subprojects/gst-examples/webrtc/sendrecv/gst/webrtc-sendrecv.c
	- send, receive binary data with WebRTC channel

- WebRTCRecvOnly
	- reference code
		- https://github.com/GStreamer/gstreamer/blob/main/subprojects/gst-examples/webrtc/sendonly/webrtc-recvonly-h264.c


- WebRTCSendOnly
	- https://github.com/GStreamer/gstreamer/blob/main/subprojects/gst-examples/webrtc/sendonly/webrtc-unidirectional-h264.c
	- webrtc-unidirectional-h264.cp
	- open http server
	- when connect browser, send camera streaming from server camera

