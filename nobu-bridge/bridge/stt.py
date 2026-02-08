from typing import Optional


class SpeechToText:
    def __init__(self) -> None:
        self.model_loaded = False

    def load(self) -> None:
        # Placeholder for faster-whisper model load
        self.model_loaded = True

    def transcribe(self, audio_bytes: bytes) -> Optional[str]:
        # Placeholder for actual STT
        if not audio_bytes:
            return None
        return ""

