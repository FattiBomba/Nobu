#!/bin/bash
# Nobu Home Server Setup Script
# Run this on your home server (Linux/Mac recommended)

set -e

echo "========================================"
echo "   Nobu Home Server Setup"
echo "========================================"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if running as root
if [ "$EUID" -eq 0 ]; then
    echo -e "${RED}Please don't run as root. Run as a normal user.${NC}"
    exit 1
fi

# Detect OS
OS="unknown"
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="mac"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    OS="windows"
fi

echo "Detected OS: $OS"
echo ""

# ==========================================
# 1. Install Ollama (Local LLM)
# ==========================================
echo -e "${YELLOW}[1/5] Installing Ollama...${NC}"

if command -v ollama &> /dev/null; then
    echo -e "${GREEN}Ollama already installed!${NC}"
    ollama --version
else
    if [ "$OS" == "linux" ] || [ "$OS" == "mac" ]; then
        curl -fsSL https://ollama.com/install.sh | sh
    else
        echo "On Windows, download Ollama from: https://ollama.com/download"
        echo "Press Enter after installing..."
        read
    fi
fi

# Pull recommended model
echo ""
echo "Pulling recommended LLM model (gemma2:9b)..."
echo "This may take a while (~5GB download)"
ollama pull gemma2:9b || echo "You can pull the model later with: ollama pull gemma2:9b"

echo ""

# ==========================================
# 2. Setup Python Environment
# ==========================================
echo -e "${YELLOW}[2/5] Setting up Python environment...${NC}"

# Check Python version
PYTHON_VERSION=$(python3 --version 2>&1 | cut -d' ' -f2 | cut -d'.' -f1,2)
echo "Python version: $PYTHON_VERSION"

# Recommend Python 3.10-3.12 for AI libraries
if [[ "$PYTHON_VERSION" == "3.10" ]] || [[ "$PYTHON_VERSION" == "3.11" ]] || [[ "$PYTHON_VERSION" == "3.12" ]]; then
    echo -e "${GREEN}Python version is compatible with AI libraries!${NC}"
else
    echo -e "${YELLOW}Warning: Python $PYTHON_VERSION may have compatibility issues.${NC}"
    echo "Recommended: Python 3.10, 3.11, or 3.12"
fi

# Create venv if not exists
if [ ! -d "nobu-bridge/venv" ]; then
    echo "Creating Python virtual environment..."
    python3 -m venv nobu-bridge/venv
fi

# Activate and install
source nobu-bridge/venv/bin/activate || source nobu-bridge/venv/Scripts/activate

pip install --upgrade pip
pip install -r nobu-bridge/requirements.txt

echo ""

# ==========================================
# 3. Install Local Speech-to-Text
# ==========================================
echo -e "${YELLOW}[3/5] Installing local Speech-to-Text (faster-whisper)...${NC}"

pip install faster-whisper || {
    echo -e "${YELLOW}faster-whisper failed, trying openai-whisper...${NC}"
    pip install openai-whisper
}

echo ""

# ==========================================
# 4. Install Local Text-to-Speech
# ==========================================
echo -e "${YELLOW}[4/5] Installing local Text-to-Speech (Piper)...${NC}"

pip install piper-tts || {
    echo -e "${YELLOW}piper-tts pip install failed.${NC}"
    echo "You can install Piper manually from: https://github.com/rhasspy/piper/releases"
}

# Download a voice
echo "Downloading Piper voice (en_US-amy-medium)..."
mkdir -p nobu-bridge/voices
cd nobu-bridge/voices

if [ ! -f "en_US-amy-medium.onnx" ]; then
    curl -L -o en_US-amy-medium.onnx "https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/amy/medium/en_US-amy-medium.onnx"
    curl -L -o en_US-amy-medium.onnx.json "https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/amy/medium/en_US-amy-medium.onnx.json"
fi

cd ../..

echo ""

# ==========================================
# 5. Install OpenClaw
# ==========================================
echo -e "${YELLOW}[5/5] Installing OpenClaw...${NC}"

if command -v openclaw &> /dev/null; then
    echo -e "${GREEN}OpenClaw already installed!${NC}"
    openclaw --version
else
    npm install -g openclaw
fi

echo ""

# ==========================================
# Setup Complete
# ==========================================
echo "========================================"
echo -e "${GREEN}   Setup Complete!${NC}"
echo "========================================"
echo ""
echo "Next steps:"
echo ""
echo "1. Configure OpenClaw:"
echo "   openclaw onboard"
echo ""
echo "2. Copy and edit the environment file:"
echo "   cp nobu-bridge/.env.example nobu-bridge/.env"
echo "   nano nobu-bridge/.env"
echo ""
echo "3. Start Ollama (if not running as service):"
echo "   ollama serve"
echo ""
echo "4. Start Nobu Bridge:"
echo "   cd nobu-bridge"
echo "   source venv/bin/activate"
echo "   python bridge/main.py"
echo ""
echo "5. Configure your Nobu ESP32 devices to connect to:"
echo "   ws://$(hostname -I | awk '{print $1}'):8765/nobu"
echo ""
