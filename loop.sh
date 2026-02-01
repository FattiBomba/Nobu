#!/bin/bash
# Ralph Wiggum Loop for Nobu Project
# This script runs Claude Code in an autonomous loop until all tasks are complete

set -e

# Configuration
MAX_ITERATIONS=50
SLEEP_BETWEEN_RUNS=5
LOG_FILE="ralph_loop.log"

echo "========================================" | tee -a "$LOG_FILE"
echo "Ralph Wiggum Loop Started: $(date)" | tee -a "$LOG_FILE"
echo "========================================" | tee -a "$LOG_FILE"

iteration=0

while [ $iteration -lt $MAX_ITERATIONS ]; do
    iteration=$((iteration + 1))
    echo "" | tee -a "$LOG_FILE"
    echo ">>> Iteration $iteration / $MAX_ITERATIONS" | tee -a "$LOG_FILE"
    echo ">>> $(date)" | tee -a "$LOG_FILE"

    # Run Claude Code with the build prompt
    # Note: Adjust the claude command based on your setup
    claude --print "$(cat PROMPT_build.md)

Current iteration: $iteration

Read IMPLEMENTATION_PLAN.md and complete the next uncompleted task.
After completing a task, mark it as done with [x] and update AGENTS.md if you learned anything new.
If all tasks are complete, respond with 'ALL_TASKS_COMPLETE'.
" 2>&1 | tee -a "$LOG_FILE"

    # Check if all tasks are complete
    if grep -q "ALL_TASKS_COMPLETE" "$LOG_FILE"; then
        echo "" | tee -a "$LOG_FILE"
        echo "========================================" | tee -a "$LOG_FILE"
        echo "All tasks completed! Exiting loop." | tee -a "$LOG_FILE"
        echo "========================================" | tee -a "$LOG_FILE"
        exit 0
    fi

    # Check if implementation plan exists and has uncompleted tasks
    if [ -f "IMPLEMENTATION_PLAN.md" ]; then
        uncompleted=$(grep -c "^\- \[ \]" IMPLEMENTATION_PLAN.md 2>/dev/null || echo "0")
        echo ">>> Remaining tasks: $uncompleted" | tee -a "$LOG_FILE"

        if [ "$uncompleted" -eq "0" ]; then
            echo ">>> No uncompleted tasks found. Exiting." | tee -a "$LOG_FILE"
            exit 0
        fi
    fi

    echo ">>> Sleeping $SLEEP_BETWEEN_RUNS seconds..." | tee -a "$LOG_FILE"
    sleep $SLEEP_BETWEEN_RUNS
done

echo "" | tee -a "$LOG_FILE"
echo "========================================" | tee -a "$LOG_FILE"
echo "Max iterations reached. Exiting loop." | tee -a "$LOG_FILE"
echo "========================================" | tee -a "$LOG_FILE"
