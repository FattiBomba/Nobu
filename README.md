# Nobu

Local-first, privacy-focused AI companion system with one home server and many ESP32 clients.

## Quick Start

### Bridge (server)
```bash
cd nobu-bridge
python -m bridge
```

Default host/port: `0.0.0.0:8765`  
Override with env vars:
```bash
set NOBU_HOST=0.0.0.0
set NOBU_PORT=8765
python -m bridge
```

### ESP32 firmware
The active firmware lives in:
`nobu-esp32/nobu_firmware/`

Configure WiFi + WebSocket + device identity using the config tool:
```bash
python scripts/configure-nobu-client.py \
  --ssid "YOUR_WIFI" \
  --password "YOUR_PASS" \
  --ws-host "192.168.1.100" \
  --ws-port 8765 \
  --ws-path "/ws" \
  --device-id "nobu-kitchen" \
  --location "kitchen"
```

Compile and upload (ESP32 Dev Module):
```bash
arduino-cli compile --fqbn esp32:esp32:esp32 .\nobu-esp32\nobu_firmware\nobu_firmware.ino
arduino-cli upload -p COM3 --fqbn esp32:esp32:esp32 .\nobu-esp32\nobu_firmware\nobu_firmware.ino
```

## Repo Structure
- `nobu-bridge/` Python server (FastAPI WebSocket)
- `nobu-esp32/` ESP32 firmware
- `openclaw-skills/` Optional OpenClaw integration
- `scripts/` Utilities and setup
- `specs/` Design specs
- `IMPLEMENTATION_PLAN.md` Task tracking

## Notes
- The bridge currently implements a minimal WebSocket protocol.
- STT/LLM/TTS are scaffolded and will be wired in next.

