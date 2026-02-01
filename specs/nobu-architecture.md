# Nobu System Architecture - Local-First Design

## Design Philosophy
- **Privacy-first**: All processing happens locally, no cloud dependencies
- **Multi-client**: One powerful home server, many tiny Nobu devices
- **Offline-capable**: Works without internet connection
- **Scalable**: Add new Nobu units without additional server load per unit

## System Overview

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        HOME NETWORK                                      │
│                                                                          │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐                │
│  │  Nobu    │  │  Nobu    │  │  Nobu    │  │  Nobu    │                │
│  │ Kitchen  │  │ Bedroom  │  │  Office  │  │  Kids    │   ...more      │
│  │  (ESP32) │  │  (ESP32) │  │  (ESP32) │  │  (ESP32) │                │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘                │
│       │             │             │             │                        │
│       └─────────────┴──────┬──────┴─────────────┘                        │
│                            │ WiFi / WebSocket                            │
│                            ▼                                             │
│  ┌──────────────────────────────────────────────────────────────────┐   │
│  │                    NOBU HOME SERVER                               │   │
│  │                (Powerful PC / Mini PC / NUC)                      │   │
│  │                                                                   │   │
│  │  ┌─────────────────────────────────────────────────────────────┐ │   │
│  │  │                    NOBU BRIDGE                               │ │   │
│  │  │  • WebSocket server (handles all Nobu clients)              │ │   │
│  │  │  • Client session management                                 │ │   │
│  │  │  • Audio routing                                             │ │   │
│  │  │  • Location-aware context                                    │ │   │
│  │  └─────────────────────────────────────────────────────────────┘ │   │
│  │                            │                                      │   │
│  │         ┌──────────────────┼──────────────────┐                  │   │
│  │         ▼                  ▼                  ▼                  │   │
│  │  ┌─────────────┐   ┌─────────────┐   ┌─────────────┐            │   │
│  │  │ LOCAL STT   │   │ LOCAL LLM   │   │ LOCAL TTS   │            │   │
│  │  │             │   │             │   │             │            │   │
│  │  │ faster-     │   │  Ollama     │   │   Piper     │            │   │
│  │  │ whisper     │   │  (Gemma/    │   │   or        │            │   │
│  │  │             │   │   Llama/    │   │   Coqui     │            │   │
│  │  │             │   │   Mistral)  │   │             │            │   │
│  │  └─────────────┘   └─────────────┘   └─────────────┘            │   │
│  │                            │                                      │   │
│  │                            ▼                                      │   │
│  │  ┌─────────────────────────────────────────────────────────────┐ │   │
│  │  │                    OPENCLAW                                  │ │   │
│  │  │  • Conversation & memory (per-user, per-location)           │ │   │
│  │  │  • Home automation skills                                    │ │   │
│  │  │  • Calendar, tasks, reminders                                │ │   │
│  │  │  • Nobu personality skill                                    │ │   │
│  │  └─────────────────────────────────────────────────────────────┘ │   │
│  └──────────────────────────────────────────────────────────────────┘   │
│                                                                          │
└─────────────────────────────────────────────────────────────────────────┘
```

## Hardware Requirements

### Home Server (One per household)
| Component | Minimum | Recommended |
|-----------|---------|-------------|
| CPU | 8-core | 12+ core |
| RAM | 16GB | 32GB+ |
| GPU | None (CPU inference) | NVIDIA RTX 3060+ (12GB VRAM) |
| Storage | 256GB SSD | 512GB+ NVMe |
| Network | Gigabit Ethernet | Gigabit + WiFi AP |

**Suggested Hardware:**
- Intel NUC 13 Pro
- Beelink SER7 (AMD Ryzen 7)
- Mini PC with NVIDIA GPU for faster inference
- Old gaming PC repurposed

### Nobu Client (Many per household)
- ESP32 + E-Paper + Mic + Speaker
- ~€50 per unit
- WiFi connected
- Battery or USB powered

## Local AI Stack

### 1. Local LLM via Ollama
```bash
# Install Ollama
curl -fsSL https://ollama.com/install.sh | sh

# Pull a model (choose based on your hardware)
ollama pull gemma2:9b      # Good balance (needs ~8GB RAM)
ollama pull llama3.2:3b    # Lighter, faster
ollama pull mistral:7b     # Great for conversation

# Ollama runs on http://localhost:11434
```

### 2. Local Speech-to-Text: faster-whisper
```bash
# Install faster-whisper (CUDA optional)
pip install faster-whisper

# Models: tiny, base, small, medium, large-v3
# 'small' is good balance of speed/accuracy
```

**Performance (on RTX 3060):**
| Model | Speed | Accuracy | VRAM |
|-------|-------|----------|------|
| tiny | 30x realtime | Basic | 1GB |
| base | 15x realtime | Good | 1GB |
| small | 8x realtime | Great | 2GB |
| medium | 4x realtime | Excellent | 5GB |

### 3. Local Text-to-Speech: Piper
```bash
# Install Piper
pip install piper-tts

# Download voices (many free voices available)
# Recommended: en_US-amy-medium (natural, ~50MB)
```

**Why Piper?**
- Runs on CPU efficiently
- Natural sounding voices
- Very fast (faster than realtime)
- Small model sizes
- Works offline

## Multi-Client Architecture

### Client Identification
Each Nobu device has:
- **Device ID**: Unique hardware identifier
- **Location**: Kitchen, Bedroom, Office, etc.
- **User Association**: Optional, for personalization

### Session Management
```python
class NobuSession:
    device_id: str          # "nobu-kitchen-01"
    location: str           # "kitchen"
    user: Optional[str]     # Voice recognition could identify user
    connected_at: datetime
    last_activity: datetime
    conversation_context: list  # Recent exchanges
```

### Location-Aware Features
- "Turn off the lights" → knows which room
- "Set a timer" → alarm on the right Nobu
- Morning briefing → only in bedroom Nobu
- Cooking help → only in kitchen Nobu

### Shared vs. Per-Device State
| Shared (Server) | Per-Device |
|-----------------|------------|
| User preferences | Current expression |
| Calendar/tasks | Audio playback state |
| Memory/history | Wake word listening |
| Home automation | Local timers |

## Configuration

### Server Config (nobu-server.yaml)
```yaml
server:
  host: 0.0.0.0
  port: 8765
  max_clients: 20

llm:
  provider: ollama
  model: gemma2:9b
  base_url: http://localhost:11434

stt:
  provider: faster-whisper
  model: small
  device: cuda  # or cpu
  compute_type: float16  # or int8 for CPU

tts:
  provider: piper
  voice: en_US-amy-medium
  speed: 1.0

clients:
  - id: nobu-kitchen
    location: kitchen
    wake_word: "hey nobu"
  - id: nobu-bedroom
    location: bedroom
    wake_word: "hey nobu"
  - id: nobu-office
    location: office
    wake_word: "hey nobu"
```

### Client Config (on ESP32)
```cpp
// Stored in ESP32 NVS (non-volatile storage)
const char* DEVICE_ID = "nobu-kitchen";
const char* LOCATION = "kitchen";
const char* SERVER_IP = "192.168.1.100";
const int SERVER_PORT = 8765;
const char* WIFI_SSID = "HomeNetwork";
const char* WIFI_PASS = "password";
```

## Scaling Considerations

### Concurrent Audio Processing
- STT can process one stream at a time per model instance
- Solution: Queue incoming audio, process sequentially
- With GPU: Can run multiple model instances

### Memory Management
- Each Nobu session: ~1KB
- LLM context per conversation: ~10KB
- Audio buffer per client: ~100KB
- 20 clients = ~2MB overhead (negligible)

### Network Bandwidth
- Audio stream: ~32kbps per client (16kHz, 16-bit)
- 10 active conversations: ~320kbps (trivial for gigabit)

## Fallback Strategy

### If Server Unavailable
1. Nobu shows "sleepy" face
2. Plays local "I can't reach home base" audio
3. Retries connection every 30 seconds
4. Basic button interactions still work locally

### Cloud Fallback (Optional)
For users who want cloud backup when local fails:
```yaml
fallback:
  enabled: false  # Privacy-first default
  provider: google  # or openai
  only_when_local_fails: true
```

## Future Enhancements
- **Voice ID**: Recognize different family members
- **Wake word customization**: Per-room wake words
- **Intercom mode**: Nobu-to-Nobu communication
- **Home Assistant integration**: Control smart home
- **Local wake word**: On-device wake word detection (no always-streaming)
