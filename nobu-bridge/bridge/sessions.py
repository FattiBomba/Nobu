from dataclasses import dataclass, field
from datetime import datetime
from typing import Dict, List, Optional


@dataclass
class NobuSession:
    device_id: str
    location: str
    connected_at: datetime = field(default_factory=datetime.utcnow)
    last_activity: datetime = field(default_factory=datetime.utcnow)
    conversation: List[dict] = field(default_factory=list)
    current_mood: str = "neutral"


class SessionManager:
    def __init__(self) -> None:
        self._sessions: Dict[str, NobuSession] = {}

    def get(self, device_id: str) -> Optional[NobuSession]:
        return self._sessions.get(device_id)

    def upsert(self, device_id: str, location: str) -> NobuSession:
        session = self._sessions.get(device_id)
        if session is None:
            session = NobuSession(device_id=device_id, location=location)
            self._sessions[device_id] = session
        else:
            session.location = location or session.location
        session.last_activity = datetime.utcnow()
        return session

    def touch(self, device_id: str) -> None:
        session = self._sessions.get(device_id)
        if session:
            session.last_activity = datetime.utcnow()

    def all(self) -> List[NobuSession]:
        return list(self._sessions.values())

