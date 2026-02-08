# Nobu Implementation Plan

Generated for Ralph Wiggum autonomous development loop.

## Architecture Overview
- **Local-first**: All AI runs on home server (Ollama + faster-whisper + Piper)
- **Multi-client**: One server, many Nobu ESP32 devices around the house
- **Privacy**: No cloud dependencies required

---

## Phase 1: Core Bridge Infrastructure (Multi-Client)

- [ ] Task 1.1: Create Nobu Bridge main.py with FastAPI WebSocket server
  - Files: `nobu-bridge/bridge/main.py`
  - Support multiple concurrent client connections
  - Track client sessions with device_id and location
  - Success: Server starts on port 8765, accepts multiple WebSocket connections
  - Progress: Added minimal FastAPI WebSocket server + hello/text/audio handlers
  - Progress: Added `python -m bridge` uvicorn entrypoint (NOBU_HOST/NOBU_PORT)

- [ ] Task 1.2: Implement local Speech-to-Text with faster-whisper
  - Files: `nobu-bridge/bridge/stt.py`, `nobu-bridge/bridge/main.py`
  - Abstract STT provider (local vs cloud fallback)
  - Queue audio for sequential processing
  - Success: Can transcribe audio bytes to text locally

- [ ] Task 1.3: Implement local Text-to-Speech with Piper
  - Files: `nobu-bridge/bridge/tts.py`, `nobu-bridge/bridge/main.py`
  - Abstract TTS provider (local vs cloud fallback)
  - Cache common phrases for speed
  - Success: Can convert text to audio bytes locally

- [ ] Task 1.4: Implement Ollama LLM communication
  - Files: `nobu-bridge/bridge/llm.py`, `nobu-bridge/bridge/main.py`
  - Direct Ollama REST API integration
  - OpenClaw integration as alternative
  - Success: Can send messages to local LLM and receive responses

- [ ] Task 1.5: Implement mood extraction and response cleaning
  - Files: `nobu-bridge/bridge/main.py`
  - Success: Extracts [mood] tags from responses, cleans text for TTS

- [ ] Task 1.6: Implement client session management
  - Files: `nobu-bridge/bridge/sessions.py`, `nobu-bridge/bridge/main.py`
  - Track connected devices, locations, conversation history
  - Handle reconnection gracefully
  - Success: Multiple clients can connect with persistent sessions
  - Progress: Added SessionManager + NobuSession scaffolding

## Phase 2: OpenClaw Nobu Skill

- [ ] Task 2.1: Create skill.yaml configuration
  - Files: `openclaw-skills/nobu/skill.yaml`
  - Success: Valid YAML with correct trigger configuration

- [ ] Task 2.2: Create Nobu personality prompt
  - Files: `openclaw-skills/nobu/prompt.md`
  - Include location-awareness in personality
  - Success: Complete personality definition with mood tag instructions

- [ ] Task 2.3: Create proactive handlers for morning briefing
  - Files: `openclaw-skills/nobu/handlers/proactive.js`
  - Location-aware notifications (bedroom gets morning briefing)
  - Success: Morning briefing and habit reminder handlers defined

## Phase 3: ESP32 Firmware (Client)

- [x] Task 3.0: Confirm Arduino CLI + ESP32 core + upload path
  - Verified `arduino-cli` works and can compile/upload on COM3
  - Sketch used: `nobu-esp32/nobu_faces/nobu_faces.ino`
  - Success: Upload completed via CLI

- [ ] Task 3.1: Create basic ESP32 firmware with display test
  - Files: `nobu-esp32/nobu_firmware/nobu_firmware.ino`
  - Success: Compiles and shows "Hello! I am Nobu" on display

- [ ] Task 3.2: Add facial expression system
  - Files: `nobu-esp32/nobu_firmware/nobu_firmware.ino`, `nobu-esp32/nobu_firmware/display_faces.cpp`
  - Success: 5+ expressions (happy, sad, thinking, excited, sleepy)
  - Progress: Demo mode cycles expressions when WS is disconnected

- [ ] Task 3.3: Add audio output (speaker) support
  - Files: `nobu-esp32/nobu_firmware/nobu_firmware.ino`
  - Success: Can play tones and mood sounds

- [ ] Task 3.4: Add audio input (microphone) support
  - Files: `nobu-esp32/nobu_firmware/nobu_firmware.ino`
  - Success: Can capture audio with voice activity detection

- [ ] Task 3.5: Add WiFi and WebSocket client with auto-reconnect
  - Files: `nobu-esp32/nobu_firmware/nobu_firmware.ino`, `nobu-esp32/nobu_firmware/ws_client.cpp`
  - Device ID and location configuration
  - Auto-reconnect on disconnect
  - Success: Connects to bridge, sends/receives messages, recovers from disconnects
  - Progress: WiFi + WebSocket scaffolding added in `nobu-esp32/nobu_firmware/*` (needs SSID/password + server URL configured)

- [ ] Task 3.6: Add NVS configuration storage
  - Files: `nobu-esp32/nobu_firmware/nobu_firmware.ino`
  - Store device_id, location, server_ip, wifi credentials
  - Success: Configuration persists across reboots

## Phase 4: Configuration & Documentation

- [x] Task 4.1: Create environment file template
  - Files: `nobu-bridge/.env.example`
  - Success: Template with local AI configuration

- [x] Task 4.2: Create requirements.txt
  - Files: `nobu-bridge/requirements.txt`
  - Success: All Python dependencies listed

- [x] Task 4.3: Create server setup script
  - Files: `scripts/setup-home-server.sh`
  - Success: Automated Ollama, Whisper, Piper installation

- [ ] Task 4.4: Create client configuration tool
  - Files: `scripts/configure-nobu-client.py`
  - Web UI or serial tool to configure ESP32 devices
  - Success: Easy way to set device_id, location, server IP
  - Progress: Added CLI tool that edits `nobu-esp32/nobu_firmware/config.h`

- [ ] Task 4.5: Create project README
  - Files: `README.md`
  - Local-first architecture, multi-client setup
  - Success: Setup instructions, architecture overview, usage guide
  - Progress: Initial README added

## Phase 5: Testing & Polish

- [ ] Task 5.1: Add error handling and fallbacks to bridge
  - Files: `nobu-bridge/bridge/main.py`
  - Cloud fallback when local AI fails
  - Graceful handling of disconnects
  - Success: System remains stable under failures

- [ ] Task 5.2: Add logging and monitoring
  - Files: `nobu-bridge/bridge/main.py`
  - Structured logging, client status dashboard
  - Success: Can monitor all connected clients

- [ ] Task 5.3: Add power management to ESP32
  - Files: `nobu-esp32/nobu_firmware.ino`
  - Deep sleep after inactivity, wake on button or voice
  - Success: Battery-powered Nobu lasts days

- [ ] Task 5.4: Add audio queue for concurrent requests
  - Files: `nobu-bridge/bridge/main.py`
  - Handle multiple clients speaking simultaneously
  - Success: No audio processing collisions

---

## Progress Tracking
- Total Tasks: 23
- Completed: 4
- Remaining: 19

## Hardware Requirements (Home Server)
- CPU: 8+ cores recommended
- RAM: 16GB+ (32GB for larger models)
- GPU: Optional NVIDIA GPU for faster inference
- Storage: 50GB+ for models

## Nobu Client Cost
- ~€50 per unit (ESP32 + display + mic + speaker)
- Unlimited clients per server

Last Updated: 2026-02-08
