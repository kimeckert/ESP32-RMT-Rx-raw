# ESP32-RMT-Rx-raw
RMT application, displaying raw RMT receive data

Receive data is simply displayed on the monitor.
* The output of the IR sensor that drives the RMT receiver input is high (logic level 1) when the IR signal is idle.
* The output of the IR sensor is driven low when the sensor detects IR pulses.  The output of the IR sensor stays low while as long as the IR sensor continues to detect IR pulses.

Data is compatible with the ESP32-RMT-server application, documented at https://github.com/kimeckert/ESP32-RMT-server.
* RMT transmit data=1 results in carrier modulated IR pulses on the output of the ESP32 RMT.
* RMT transmit data=0 results in no IR signal transmitted from the RMT.

Due to the logic inversion between this applicaiton receive data and the transmitted data in ESP32-RMT-server, this application inverts the logic sense of the received IR data.

This application has been tested on an Adafruit ESP32 Feather board.
The aplication flashes the on-board visible LED when it responds to received RMT data.
