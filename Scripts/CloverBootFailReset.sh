#!/bin/bash
#
# CloverBootFailReset.sh
#
# Automatically resets the Clover boot failure counter after a successful boot.
# This script should be executed by a LaunchDaemon on macOS startup.
#
# Installation:
#   1. Copy this script to /usr/local/bin/CloverBootFailReset.sh
#   2. Make it executable: chmod +x /usr/local/bin/CloverBootFailReset.sh
#   3. Copy the LaunchDaemon plist to /Library/LaunchDaemons/
#   4. Load it: sudo launchctl load /Library/LaunchDaemons/com.clover.bootfailreset.plist

# Check if running as root
if [ "$EUID" -ne 0 ]; then
  echo "Error: This script must be run as root"
  exit 1
fi

# Log file
LOG_FILE="/var/log/clover-bootfail-reset.log"

# Function to log messages
log_message() {
  echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$LOG_FILE"
}

log_message "=== Clover Boot Failure Counter Reset Script Started ==="

# Check if the NVRAM variable exists
FAIL_COUNT=$(nvram -p | grep "CloverBootFailCount" | awk -F'\t' '{print $2}')

if [ -z "$FAIL_COUNT" ]; then
  log_message "INFO: CloverBootFailCount variable not found. Nothing to reset."
else
  log_message "INFO: Current CloverBootFailCount = $FAIL_COUNT"
  
  # Delete the NVRAM variable
  nvram -d CloverBootFailCount 2>&1 | tee -a "$LOG_FILE"
  
  if [ $? -eq 0 ]; then
    log_message "SUCCESS: CloverBootFailCount has been reset."
  else
    log_message "ERROR: Failed to reset CloverBootFailCount."
    exit 1
  fi
fi

log_message "=== Script Completed Successfully ==="
exit 0
