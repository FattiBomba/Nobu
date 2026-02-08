import json
from typing import Any, Dict

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


async def handle_hello(ws: WebSocket, payload: Dict[str, Any]) -> None:
    device_id = payload.get("device_id", "unknown")
    location = payload.get("location", "unknown")
    sessions.upsert(device_id, location)
    await ws.send_text(json.dumps({"type": "ack", "device_id": device_id}))


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

            # TODO: handle audio messages and responses
            await ws.send_text(json.dumps({"type": "error", "message": "unsupported_type"}))
    except WebSocketDisconnect:
        return

