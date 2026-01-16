## ESP32 Power Outage Ping Monitor

Simple ESP32 sketch that pings an MQTT broker every 10 seconds. By watching for missed pings, you can infer when the device lost power (e.g., a building-wide outage) and later came back online.

### How It Works
- Connects to Wi-Fi, syncs time via NTP, and sets up an MQTT client.
- Publishes a timestamp (fallback: a monotonically increasing counter if NTP is not ready) to a device-specific ping topic.
- Reconnects automatically to MQTT if the connection drops.
- Missing messages on the ping topic indicate the ESP32 was offline, which can signal a power outage.

### Configuration
Update the placeholders in [esp32-pod/esp32-pod.ino](esp32-pod/esp32-pod.ino):

| Placeholder | Description |
| --- | --- |
| `__SSID__` | Wi-Fi SSID |
| `__PASSWORD__` | Wi-Fi password |
| `__MQTT_SERVER__` | MQTT broker host/IP |
| `__MQTT_USER__` | MQTT username (optional, leave blank if not used) |
| `__MQTT_PASS__` | MQTT password (optional) |

Other built-in defaults:
- MQTT port: `1883`
- MQTT client ID: `esp32_pod`
- Ping interval: 10 seconds

### MQTT Topics and Payloads
- Device ID: derived from ESP32 MAC, lowercased, in the form `esp32/<chip_id>`.
- Ping topic: `<device_id>/ping` (e.g., `esp32/abcdef12/ping`).
- Payloads:
	- Timestamp string `YYYY-MM-DD HH:MM:SS` once NTP is available.
	- Fallback integer counter if time is not yet available (increments each send until NTP succeeds).

### Build and Flash
1) Install Arduino IDE (or PlatformIO) and the ESP32 board package.
2) Clone this repo and open [esp32-pod/esp32-pod.ino](esp32-pod/esp32-pod.ino).
3) Fill in the Wi-Fi and MQTT placeholders above.
4) Select the correct ESP32 board and serial port.
5) Flash the sketch to the device.

### Monitoring a Power Outage
- Subscribe to the ping topic shown above.
- Each successful ping indicates the device is online. The gaps in pings point to device/offline time (likely power loss or network down).
- After power is restored and Wi-Fi/MQTT reconnect, pings resume automatically with fresh timestamps.

### Troubleshooting
- Stuck connecting: confirm Wi-Fi credentials and signal strength.
- MQTT auth failures: verify username/password or allow unauthenticated access on the broker for this client.
- No timestamps: ensure NTP is reachable. The sketch will temporarily send counter values until time sync succeeds.

### License
This project is licensed under the MIT License. See [LICENSE](LICENSE).
