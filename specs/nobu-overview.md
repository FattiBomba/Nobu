# Nobu Project Specification

## Overview
Nobu is a physical AI companion that gives OpenClaw a physical body - a cute face on an e-paper display, a voice through a speaker, and ears through a microphone.

## System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     NOBU UNIT (ESP32)                           │
│                                                                 │
│  Microphone ──→ Captures your voice                            │
│  Speaker    ←── Plays Nobu's responses                         │
│  E-Paper    ←── Shows facial expressions                       │
│  Button     ──→ Wake/interact trigger                          │
│                                                                 │
│  Connects via WiFi to Nobu Bridge                              │
└─────────────────────────────────────────────────────────────────┘
                              │
                          WebSocket
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                     NOBU BRIDGE (Python)                        │
│                                                                 │
│  Audio In  ──→ Whisper API ──→ Text                            │
│  Text      ──→ OpenClaw    ──→ Response + Mood                 │
│  Response  ──→ TTS API     ──→ Audio Out                       │
│  Mood      ──→ Expression  ──→ Face Command                    │
└─────────────────────────────────────────────────────────────────┘
                              │
                          HTTP/WS
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                        OPENCLAW                                 │
│  Conversation & personality (via Nobu Skill)                   │
│  Gmail, Calendar, Tasks, Browser automation                    │
│  Memory system, Community skills                               │
└─────────────────────────────────────────────────────────────────┘
```

## Hardware Components
| Component | Model | Purpose |
|-----------|-------|---------|
| Microcontroller + Display | LilyGO TTGO T5 V2.3 ESP32 | Main brain + e-paper face |
| Speaker Amplifier | MAX98357 I2S | Audio output |
| Speaker | 4ohm 3W | Voice output |
| Microphone | INMP441 I2S MEMS | Voice input |

## Pin Mapping
### Display (built into T5)
- CS: GPIO 5
- DC: GPIO 17
- RST: GPIO 16
- BUSY: GPIO 4

### Audio Amplifier (MAX98357)
- BCLK: GPIO 26
- LRC: GPIO 25
- DIN: GPIO 22

### Microphone (INMP441)
- SCK: GPIO 32
- WS: GPIO 33
- SD: GPIO 27
- L/R: GND

### Button (built into T5)
- GPIO 39

## Facial Expressions
Nobu has 5+ expressions controlled by mood tags:
- `[happy]` - Default pleasant mood
- `[sad]` - Empathizing or bad news
- `[thinking]` - Processing or considering
- `[excited]` - Great news or achievements
- `[sleepy]` - Late night or low energy
- `[curious]` - Questions or interest

## Communication Protocol
### ESP32 → Bridge (WebSocket)
- Binary audio data (WAV format, 16kHz)
- JSON status messages

### Bridge → ESP32 (WebSocket)
- JSON: `{"type": "response", "mood": "happy", "text": "..."}`
- Binary: Audio data for playback

## Required APIs
- OpenAI Whisper API (Speech-to-Text)
- OpenAI TTS or ElevenLabs (Text-to-Speech)
- OpenClaw API (localhost:3000)
