#!/bin/bash
#
# install-bootfail-reset.sh
#
# Automated installer for Clover Boot Failure Counter Auto-Reset
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if running as root
if [ "$EUID" -ne 0 ]; then
  echo -e "${RED}Error: This installer must be run as root${NC}"
  echo "Please run: sudo bash install-bootfail-reset.sh"
  exit 1
fi

echo -e "${GREEN}=== Clover Boot Failure Counter Auto-Reset Installer ===${NC}"
echo ""

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Define paths
SCRIPT_SRC="$SCRIPT_DIR/CloverBootFailReset.sh"
PLIST_SRC="$SCRIPT_DIR/com.clover.bootfailreset.plist"
SCRIPT_DEST="/usr/local/bin/CloverBootFailReset.sh"
PLIST_DEST="/Library/LaunchDaemons/com.clover.bootfailreset.plist"

# Check if source files exist
if [ ! -f "$SCRIPT_SRC" ]; then
  echo -e "${RED}Error: CloverBootFailReset.sh not found in $SCRIPT_DIR${NC}"
  exit 1
fi

if [ ! -f "$PLIST_SRC" ]; then
  echo -e "${RED}Error: com.clover.bootfailreset.plist not found in $SCRIPT_DIR${NC}"
  exit 1
fi

# Unload existing LaunchDaemon if it exists
if launchctl list | grep -q "com.clover.bootfailreset"; then
  echo -e "${YELLOW}Unloading existing LaunchDaemon...${NC}"
  launchctl unload "$PLIST_DEST" 2>/dev/null || true
fi

# Create /usr/local/bin if it doesn't exist
if [ ! -d "/usr/local/bin" ]; then
  echo -e "${YELLOW}Creating /usr/local/bin directory...${NC}"
  mkdir -p /usr/local/bin
fi

# Copy script
echo -e "${GREEN}Installing script to $SCRIPT_DEST...${NC}"
cp "$SCRIPT_SRC" "$SCRIPT_DEST"
chmod +x "$SCRIPT_DEST"
chown root:wheel "$SCRIPT_DEST"

# Copy plist
echo -e "${GREEN}Installing LaunchDaemon to $PLIST_DEST...${NC}"
cp "$PLIST_SRC" "$PLIST_DEST"
chmod 644 "$PLIST_DEST"
chown root:wheel "$PLIST_DEST"

# Load LaunchDaemon
echo -e "${GREEN}Loading LaunchDaemon...${NC}"
launchctl load "$PLIST_DEST"

# Verify installation
echo ""
echo -e "${GREEN}=== Installation Complete ===${NC}"
echo ""

if launchctl list | grep -q "com.clover.bootfailreset"; then
  echo -e "${GREEN}✓ LaunchDaemon is loaded and active${NC}"
else
  echo -e "${YELLOW}⚠ Warning: LaunchDaemon may not be loaded properly${NC}"
fi

echo ""
echo "Files installed:"
echo "  - $SCRIPT_DEST"
echo "  - $PLIST_DEST"
echo ""
echo "The boot failure counter will be automatically reset on each successful boot."
echo ""
echo "To verify: sudo launchctl list | grep clover"
echo "To view logs: cat /var/log/clover-bootfail-reset.log"
echo ""
