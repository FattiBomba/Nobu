#!/usr/bin/env python3
"""
Configure Nobu ESP32 client settings by updating config.h.

This is a simple, local-first workflow:
- Edits nobu-esp32/nobu_firmware/config.h
- Updates WiFi, WebSocket, and device identity fields
"""

from __future__ import annotations

import argparse
import re
from pathlib import Path


DEFAULT_CONFIG_PATH = Path(__file__).resolve().parents[1] / "nobu-esp32" / "nobu_firmware" / "config.h"


def _replace_const_line(text: str, name: str, value: str) -> str:
    pattern = re.compile(rf'^(const\s+char\*\s+{re.escape(name)}\s*=\s*")[^"]*(";)\s*$')
    lines = text.splitlines()
    out = []
    replaced = False
    for line in lines:
        m = pattern.match(line)
        if m:
            out.append(f'{m.group(1)}{value}{m.group(2)}')
            replaced = True
        else:
            out.append(line)
    if not replaced:
        raise ValueError(f"Could not find {name} in config.h")
    return "\n".join(out) + ("\n" if text.endswith("\n") else "")


def _replace_uint_line(text: str, name: str, value: int) -> str:
    pattern = re.compile(rf'^(const\s+uint16_t\s+{re.escape(name)}\s*=\s*)\d+(;\s*)$')
    lines = text.splitlines()
    out = []
    replaced = False
    for line in lines:
        m = pattern.match(line)
        if m:
            out.append(f"{m.group(1)}{value}{m.group(2)}")
            replaced = True
        else:
            out.append(line)
    if not replaced:
        raise ValueError(f"Could not find {name} in config.h")
    return "\n".join(out) + ("\n" if text.endswith("\n") else "")


def configure(
    config_path: Path,
    ssid: str,
    password: str,
    ws_host: str,
    ws_port: int,
    ws_path: str,
    device_id: str,
    location: str,
) -> str:
    text = config_path.read_text(encoding="utf-8")
    text = _replace_const_line(text, "WIFI_SSID", ssid)
    text = _replace_const_line(text, "WIFI_PASSWORD", password)
    text = _replace_const_line(text, "WS_HOST", ws_host)
    text = _replace_uint_line(text, "WS_PORT", ws_port)
    text = _replace_const_line(text, "WS_PATH", ws_path)
    text = _replace_const_line(text, "DEVICE_ID", device_id)
    text = _replace_const_line(text, "DEVICE_LOCATION", location)
    return text


def main() -> int:
    parser = argparse.ArgumentParser(description="Configure Nobu ESP32 client settings.")
    parser.add_argument("--config", type=Path, default=DEFAULT_CONFIG_PATH, help="Path to config.h")
    parser.add_argument("--ssid", required=True, help="WiFi SSID")
    parser.add_argument("--password", required=True, help="WiFi password")
    parser.add_argument("--ws-host", required=True, help="WebSocket host (IP or hostname)")
    parser.add_argument("--ws-port", type=int, default=8765, help="WebSocket port")
    parser.add_argument("--ws-path", default="/ws", help="WebSocket path")
    parser.add_argument("--device-id", required=True, help="Device id (e.g., nobu-kitchen)")
    parser.add_argument("--location", required=True, help="Device location (e.g., kitchen)")
    parser.add_argument("--dry-run", action="store_true", help="Show changes without writing")
    args = parser.parse_args()

    config_path: Path = args.config
    if not config_path.exists():
        raise SystemExit(f"Config not found: {config_path}")

    updated = configure(
        config_path=config_path,
        ssid=args.ssid,
        password=args.password,
        ws_host=args.ws_host,
        ws_port=args.ws_port,
        ws_path=args.ws_path,
        device_id=args.device_id,
        location=args.location,
    )

    if args.dry_run:
        print(updated)
        return 0

    config_path.write_text(updated, encoding="utf-8")
    print(f"Updated: {config_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

