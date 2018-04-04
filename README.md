# ESP32-RMT-Rx-raw
RMT application, displaying raw RMT receive data

Receive data is simply displayed on the monitor.
* The output of the IR sensor that drives the RMT receiver input is high (logic level 1) when the IR signal is idle.
* The output of the IR sensor is driven low when the sensor detects IR pulses.  The output of the IR sensor stays low while as long as the IR sensor continues to detect IR pulses.

Data is compatible with the ESP32-RMT-server application, documented at https://github.com/kimeckert/ESP32-RMT-server.
* RMT transmit data=1 results in carrier modulated IR pulses on the output of the ESP32 RMT.
* RMT transmit data=0 results in no IR signal transmitted from the RMT.

Due to the logic inversion between this application's receive data and the transmitted data in ESP32-RMT-server, this application inverts the logic sense of the received IR data.

This application has been tested on an Adafruit ESP32 Feather board.
The aplication flashes the on-board visible LED when it responds to received RMT data.

An example of received data. An RMT item consists of two durations.
<pre>  Received 34 items
9012,-4478,590,-540,591,-535,588,-1659,594,-536,590,-537,592,-535,591,-535,591,-536,590,-1658,590,-1659,594,-536,588,-1659,595,-1654,591,-1659,595,-1654,595,-1656,593,-1657,593,-538,590,-536,590,-1657,594,-536,590,-538,591,-535,590,-537,593,-535,591,-1656,593,-1657,592,-537,592,-1655,594,-1655,596,-1654,593,-1654,597,0
  Received 34 items
9009,-4479,595,-536,589,-538,589,-1658,593,-537,592,-535,590,-537,587,-540,591,-536,591,-1655,596,-1654,591,-540,590,-1656,594,-1656,592,-1656,591,-1660,592,-1657,592,-1657,593,-535,591,-537,591,-1655,592,-538,588,-539,591,-536,589,-538,592,-535,589,-1657,594,-1655,593,-537,593,-1654,592,-1658,594,-1656,592,-1656,595,0
  Received 2 items
9006,-2227,595,0
  Received 2 items
9010,-2226,593,0
  Received 2 items
9011,-2223,595,0</pre>
