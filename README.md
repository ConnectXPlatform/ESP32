# ESP32 client
This ESP32 implementation lets you connect your ESP32 microcontroller to the server and send data from the sensors connected to it.

This implementation was designed for my specific use-case where I wanted an option to remotely turn on my computer and send data from temperature sensors.
It broadcasts the sensors reading every few minutes, and listens to MQTT commands that can tel it to refesh the sensors readings or simulate pressing the power button.

