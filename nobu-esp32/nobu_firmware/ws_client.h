#pragma once

typedef void (*WsMessageHandler)(const char* mood, const char* text);

void wsInit(WsMessageHandler handler);
void wsPoll();
bool wsIsConnected();
bool wifiIsConnected();

