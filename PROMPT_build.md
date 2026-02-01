# Build Mode Instructions

You are building Nobu, a physical AI companion powered by OpenClaw.

## Your Role
You are an autonomous coding agent implementing the Nobu project. Work through the IMPLEMENTATION_PLAN.md task by task.

## Working Guidelines

### Before Starting Any Task
1. Read the current task from IMPLEMENTATION_PLAN.md
2. Check specs/ folder for relevant specifications
3. Review existing code to understand context

### While Implementing
1. Write clean, well-documented code
2. Handle errors gracefully
3. Use type hints in Python code
4. Follow Arduino best practices for ESP32 code
5. Test incrementally when possible

### After Completing a Task
1. Mark the task as complete in IMPLEMENTATION_PLAN.md
2. Update AGENTS.md with any learned patterns
3. Commit changes with clear messages

## Code Standards

### Python (Nobu Bridge)
- Use async/await for all I/O operations
- Type hints on all functions
- Docstrings for public functions
- Error handling with specific exceptions

### C++ (ESP32 Firmware)
- Descriptive variable names
- Comment complex logic
- Use constants for pin definitions
- Modular function design

### OpenClaw Skills
- YAML must be valid and well-formatted
- Markdown prompts should be clear and specific
- JavaScript handlers must export properly

## File Locations
- Python code: `/c/Users/bramp/Desktop/Nobu/nobu-bridge/`
- ESP32 code: `/c/Users/bramp/Desktop/Nobu/nobu-esp32/`
- Skills: `/c/Users/bramp/Desktop/Nobu/openclaw-skills/`

## Remember
- The project uses OpenClaw as the AI brain - don't rebuild what it provides
- Nobu hardware stays simple - complexity lives in the bridge and OpenClaw
- Mood tags [happy], [sad], [thinking], etc. control the face expression
