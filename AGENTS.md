# AGENTS.md - Nobu Project Operational Guide

## Project Overview
Nobu is a **local-first, privacy-focused** AI companion system:
- **One home server** runs all AI processing (LLM, STT, TTS)
- **Multiple Nobu clients** (ESP32 devices) around the house
- **No cloud required** - everything runs locally on your own hardware

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     HOME NETWORK                             │
│                                                              │
│   [Nobu Kitchen] [Nobu Bedroom] [Nobu Office] [Nobu Kids]   │
│         │              │             │             │         │
│         └──────────────┴──────┬──────┴─────────────┘         │
│                               │ WiFi                         │
│                               ▼                              │
│   ┌─────────────────────────────────────────────────────┐   │
│   │              NOBU HOME SERVER                        │   │
│   │                                                      │   │
│   │  ┌──────────┐  ┌──────────┐  ┌──────────┐          │   │
│   │  │  Ollama  │  │ Whisper  │  │  Piper   │          │   │
│   │  │  (LLM)   │  │  (STT)   │  │  (TTS)   │          │   │
│   │  └──────────┘  └──────────┘  └──────────┘          │   │
│   │                      │                               │   │
│   │              ┌───────┴───────┐                      │   │
│   │              │  Nobu Bridge  │                      │   │
│   │              │  (Python)     │                      │   │
│   │              └───────────────┘                      │   │
│   └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

## Directory Structure
```
/c/Users/bramp/Desktop/Nobu/
├── nobu-bridge/              # Python server (runs on home server)
│   ├── bridge/
│   │   ├── main.py          # FastAPI WebSocket server
│   │   ├── stt.py           # Speech-to-text (faster-whisper)
│   │   ├── tts.py           # Text-to-speech (Piper)
│   │   ├── llm.py           # LLM client (Ollama)
│   │   └── sessions.py      # Client session management
│   ├── voices/              # Piper voice models
│   ├── .env                 # Configuration
│   └── venv/                # Python virtual environment
├── nobu-esp32/              # Arduino firmware (runs on each client)
│   └── nobu_firmware.ino    # ESP32 firmware
├── openclaw-skills/         # OpenClaw integration (optional)
│   └── nobu/
├── scripts/
│   └── setup-home-server.sh # Automated server setup
├── specs/                   # Design specifications
├── IMPLEMENTATION_PLAN.md   # Task tracking
└── loop.sh                  # Ralph Wiggum loop
```

## Local AI Stack

### Ollama (LLM)
- REST API at `http://localhost:11434`
- Recommended models: gemma2:9b, llama3.2:3b, mistral:7b
- Install: `curl -fsSL https://ollama.com/install.sh | sh`
- Pull model: `ollama pull gemma2:9b`

### faster-whisper (STT)
- Python library, CPU or CUDA
- Models: tiny, base, small, medium, large-v3
- `pip install faster-whisper`

### Piper (TTS)
- Fast, natural-sounding local TTS
- Many free voices available
- `pip install piper-tts`

## Client Session Model
```python
class NobuSession:
    device_id: str          # "nobu-kitchen"
    location: str           # "kitchen"
    connected_at: datetime
    last_activity: datetime
    conversation: list      # Message history
    current_mood: str       # "happy", "thinking", etc.
```

## Communication Protocol

### Client → Server
```json
{
  "type": "audio",
  "device_id": "nobu-kitchen",
  "data": "<base64 audio>"
}
```

### Server → Client
```json
{
  "type": "response",
  "mood": "happy",
  "text": "Good morning!",
  "audio": "<base64 audio>"
}
```

## Build Commands

### Server Setup
```bash
# Run the automated setup
./scripts/setup-home-server.sh

# Or manually:
ollama serve &
cd nobu-bridge
source venv/bin/activate  # or venv/Scripts/activate on Windows
python bridge/main.py
```

### ESP32 Client
- Use Arduino IDE 2.x
- Install ESP32 board support
- Install libraries: GxEPD2, WebSockets, ArduinoJson
- Flash to each Nobu device

## Key Design Decisions

1. **Local-first**: Privacy is paramount, no cloud dependencies
2. **Multi-client**: One brain, many bodies (scalable)
3. **Location-aware**: Each Nobu knows where it is
4. **Fallback ready**: Can use cloud APIs if local fails (optional)
5. **Mood system**: [happy], [sad], [thinking] tags control expressions

## Hardware

### Home Server Requirements
- CPU: 8+ cores recommended
- RAM: 16GB+ (32GB for larger models)
- GPU: Optional NVIDIA for faster inference
- Storage: 50GB+ for AI models

### Nobu Client (~€50 each)
- LilyGO TTGO T5 V2.3 ESP32
- MAX98357 I2S amplifier + 4Ω speaker
- INMP441 I2S microphone

## Pin Mapping (ESP32)
- Display: Built into T5 (CS:5, DC:17, RST:16, BUSY:4)
- Speaker: BCLK:26, LRC:25, DIN:22
- Mic: SCK:32, WS:33, SD:27, L/R:GND
- Button: GPIO 39

## Learned Patterns
- Ollama REST API: POST to /api/generate with {"model": "...", "prompt": "..."}
- faster-whisper: Load model once, transcribe audio segments
- Piper: Generates WAV audio from text, very fast
- ESP32 WebSocket: Use ArduinoWebSockets library for reliability
- E-paper partial refresh: Only update changed areas for speed

## Quality Gates
- Server must handle 10+ concurrent clients
- STT latency < 2 seconds for typical utterance
- TTS latency < 1 second for typical response
- Client must auto-reconnect within 30 seconds
- All mood expressions render correctly on 250x122 display
