# Plan Mode Instructions

You are planning the implementation of Nobu, a physical AI companion.

## Your Role
Analyze the project specifications and create a detailed implementation plan in IMPLEMENTATION_PLAN.md.

## Planning Process

### 1. Read All Specifications
- Review every file in the specs/ folder
- Understand the complete system architecture
- Identify dependencies between components

### 2. Break Down into Tasks
- Each task should be completable in one focused session
- Tasks should have clear success criteria
- Order tasks by dependencies (what must be done first)

### 3. Create Implementation Plan
Write to IMPLEMENTATION_PLAN.md with this format:

```markdown
# Implementation Plan

## Phase 1: [Name]
- [ ] Task 1: Description (files: x.py, y.ino)
- [ ] Task 2: Description

## Phase 2: [Name]
...
```

## Task Sizing Guidelines
- A task should modify 1-3 files maximum
- Each task should take 15-60 minutes to implement
- Include testing as part of the task, not separate

## Dependency Considerations
- Python venv and dependencies are already set up
- OpenClaw is installed globally
- Arduino IDE setup is manual (user does this)
- API keys need to be provided by user

## Output Requirements
1. Write complete IMPLEMENTATION_PLAN.md
2. Update AGENTS.md with any new patterns discovered
3. Identify any blockers or questions for the user
