# Nobu Bridge Specification

## Purpose
The Nobu Bridge is a Python FastAPI server that connects the ESP32 hardware to OpenClaw.

## Responsibilities
1. Accept WebSocket connections from Nobu ESP32 devices
2. Convert speech to text using Whisper API
3. Send messages to OpenClaw and receive responses
4. Convert responses to speech using TTS API
5. Extract mood from responses and send face commands
6. Stream audio back to ESP32

## File Structure
```
nobu-bridge/
├── bridge/
│   └── main.py          # Main FastAPI application
├── handlers/            # Custom message handlers
├── venv/                # Python virtual environment
├── .env                 # Environment variables
├── .env.example         # Environment template
└── requirements.txt     # Python dependencies
```

## Dependencies
Already installed in venv:
- fastapi
- uvicorn
- websockets
- httpx
- openai
- python-dotenv
- numpy
- soundfile

## Environment Variables
```
OPENAI_API_KEY=sk-...           # For Whisper STT and TTS
ELEVENLABS_API_KEY=...          # Optional: for ElevenLabs TTS
OPENCLAW_URL=http://localhost:3000
USE_OPENAI_TTS=true             # Set to false for ElevenLabs
```

## API Endpoints

### WebSocket: `/nobu`
Handles bidirectional communication with ESP32 devices.

**Incoming Messages:**
- Binary: Audio data from microphone
- JSON: Status/control messages

**Outgoing Messages:**
- JSON: `{"type": "response", "mood": "happy", "text": "response text"}`
- Binary: TTS audio data

### HTTP: `/health`
Health check endpoint.
```json
{"status": "ok", "nobu_devices": 1}
```

## Core Classes

### NobuBridge
Main bridge logic class.

```python
class NobuBridge:
    async def speech_to_text(self, audio_bytes: bytes) -> str:
        """Convert speech to text using Whisper"""

    async def text_to_speech(self, text: str) -> bytes:
        """Convert text to speech using OpenAI/ElevenLabs"""

    async def send_to_openclaw(self, message: str) -> dict:
        """Send message to OpenClaw and get response"""

    def extract_mood(self, response: str) -> str:
        """Extract mood tag from response"""

    def clean_response(self, response: str) -> str:
        """Remove mood tags from response text"""
```

## Mood Extraction
The bridge extracts mood tags from OpenClaw responses:
- Looks for `[mood]` pattern in response
- Valid moods: happy, sad, thinking, excited, sleepy, curious
- Defaults to "happy" if no mood found
- Removes mood tag from text before TTS

## Error Handling
- Reconnection logic for OpenClaw connection
- Graceful handling of STT/TTS API failures
- Device disconnect handling
- Logging for debugging

## Running the Server
```bash
cd nobu-bridge
source venv/Scripts/activate  # Windows
# source venv/bin/activate    # Mac/Linux
python bridge/main.py
```

Server runs on `0.0.0.0:8765`
