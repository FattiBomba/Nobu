import json
from typing import Any, Dict, Optional

from fastapi import FastAPI, WebSocket, WebSocketDisconnect

from .sessions import SessionManager
from .stt import SpeechToText
from .tts import TextToSpeech
from .llm import LLMClient


app = FastAPI()
sessions = SessionManager()
stt = SpeechToText()
tts = TextToSpeech()
llm = LLMClient()


@app.on_event("startup")
async def on_startup() -> None:
    stt.load()
    tts.load()


async def send_response(ws: WebSocket, mood: str, text: str) -> None:
    payload = {
        "type": "response",
        "mood": mood,
        "text": text,
    }
    await ws.send_text(json.dumps(payload))


async def handle_hello(ws: WebSocket, payload: Dict[str, Any]) -> None:
    device_id = payload.get("device_id", "unknown")
    location = payload.get("location", "unknown")
    sessions.upsert(device_id, location)
    await ws.send_text(json.dumps({"type": "ack", "device_id": device_id}))
    await send_response(ws, "happy", f"Hello from Nobu Bridge ({device_id})")


async def handle_text(ws: WebSocket, payload: Dict[str, Any]) -> None:
    text: Optional[str] = payload.get("text")
    if not text:
        await ws.send_text(json.dumps({"type": "error", "message": "missing_text"}))
        return

    # Placeholder for LLM + mood extraction
    await send_response(ws, "thinking", f"Received: {text}")


async def handle_audio(ws: WebSocket, payload: Dict[str, Any]) -> None:
    # Placeholder for audio pipeline (base64 audio expected)
    if "data" not in payload:
        await ws.send_text(json.dumps({"type": "error", "message": "missing_audio_data"}))
        return

    await send_response(ws, "thinking", "Audio received (STT not wired yet)")


@app.websocket("/ws")
async def websocket_endpoint(ws: WebSocket) -> None:
    await ws.accept()
    try:
        while True:
            raw = await ws.receive_text()
            try:
                payload = json.loads(raw)
            except json.JSONDecodeError:
                await ws.send_text(json.dumps({"type": "error", "message": "invalid_json"}))
                continue

            msg_type = payload.get("type")
            if msg_type == "hello":
                await handle_hello(ws, payload)
                continue

            if msg_type == "text":
                await handle_text(ws, payload)
                continue

            if msg_type == "audio":
                await handle_audio(ws, payload)
                continue

            await ws.send_text(json.dumps({"type": "error", "message": "unsupported_type"}))
    except WebSocketDisconnect:
        return
