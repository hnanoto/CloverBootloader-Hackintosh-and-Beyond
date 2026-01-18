#!/bin/bash
#
# clover-diagnostic.sh
#
# Diagnostic script for Clover Híbrido Inteligente
# Checks if Hardware Validator, Self-Healing, and Auto-Reset are working
#

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   Clover Híbrido Inteligente - Diagnostic Tool            ║${NC}"
echo -e "${BLUE}║   Build #139 - Hardware Validator & Self-Healing          ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
echo ""

# Function to print status
print_status() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✓${NC} $2"
    else
        echo -e "${RED}✗${NC} $2"
    fi
}

print_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

print_info() {
    echo -e "${BLUE}ℹ${NC} $1"
}

# ============================================================================
# 1. CHECK BOOT FAILURE COUNTER
# ============================================================================
echo -e "${BLUE}[1/5] Checking Boot Failure Counter...${NC}"
FAIL_COUNT=$(nvram -p 2>/dev/null | grep "CloverBootFailCount" | awk -F'\t' '{print $2}')

if [ -z "$FAIL_COUNT" ]; then
    print_status 0 "Boot fail counter: 0 (normal)"
else
    if [ "$FAIL_COUNT" -ge 3 ]; then
        print_warning "Boot fail counter: $FAIL_COUNT (Safe Mode will activate!)"
    else
        print_info "Boot fail counter: $FAIL_COUNT"
    fi
fi
echo ""

# ============================================================================
# 2. CHECK LAUNCHDAEMON STATUS
# ============================================================================
echo -e "${BLUE}[2/5] Checking Auto-Reset LaunchDaemon...${NC}"

if [ -f "/Library/LaunchDaemons/com.clover.bootfailreset.plist" ]; then
    print_status 0 "LaunchDaemon plist installed"
    
    if launchctl list 2>/dev/null | grep -q "com.clover.bootfailreset"; then
        print_status 0 "LaunchDaemon is loaded and active"
    else
        print_warning "LaunchDaemon plist exists but not loaded"
        echo "   Run: sudo launchctl load /Library/LaunchDaemons/com.clover.bootfailreset.plist"
    fi
else
    print_warning "LaunchDaemon not installed"
    echo "   Run: cd Scripts && sudo bash install-bootfail-reset.sh"
fi

if [ -f "/usr/local/bin/CloverBootFailReset.sh" ]; then
    print_status 0 "Reset script installed"
else
    print_warning "Reset script not found"
fi
echo ""

# ============================================================================
# 3. CHECK RESET LOGS
# ============================================================================
echo -e "${BLUE}[3/5] Checking Auto-Reset Logs...${NC}"

if [ -f "/var/log/clover-bootfail-reset.log" ]; then
    print_status 0 "Reset log file exists"
    echo ""
    echo "   Last 5 entries:"
    tail -5 /var/log/clover-bootfail-reset.log 2>/dev/null | sed 's/^/   /'
else
    print_info "No reset logs yet (daemon hasn't run)"
fi
echo ""

# ============================================================================
# 4. CHECK CLOVER DEBUG LOG
# ============================================================================
echo -e "${BLUE}[4/5] Checking Clover Debug Logs...${NC}"

# Try to find EFI partition
EFI_MOUNT=$(mount | grep -i "EFI" | awk '{print $3}' | head -1)

if [ -z "$EFI_MOUNT" ]; then
    print_warning "EFI partition not mounted"
    echo "   Mount it with: sudo diskutil mount disk0s1"
    DEBUG_LOG=""
else
    DEBUG_LOG="$EFI_MOUNT/EFI/CLOVER/misc/debug.log"
    
    if [ -f "$DEBUG_LOG" ]; then
        print_status 0 "Debug log found at: $DEBUG_LOG"
        
        # Check for HardwareValidator messages
        if grep -q "HardwareValidator:" "$DEBUG_LOG" 2>/dev/null; then
            print_status 0 "Hardware Validator is active (found in logs)"
            
            echo ""
            echo "   Recent Hardware Validator messages:"
            grep "HardwareValidator:" "$DEBUG_LOG" | tail -10 | sed 's/^/   /'
        else
            print_warning "No HardwareValidator messages found in debug.log"
            echo "   This might mean:"
            echo "   - Debug logging is not enabled in config.plist"
            echo "   - You're not using the Build #139 CLOVERX64.efi"
        fi
    else
        print_warning "Debug log not found at: $DEBUG_LOG"
        echo "   Enable debug in config.plist: <key>Debug</key><true/>"
    fi
fi
echo ""

# ============================================================================
# 5. CHECK CONFIG.PLIST DEBUG SETTINGS
# ============================================================================
echo -e "${BLUE}[5/5] Checking config.plist Debug Settings...${NC}"

if [ -n "$EFI_MOUNT" ] && [ -f "$EFI_MOUNT/EFI/CLOVER/config.plist" ]; then
    CONFIG_PLIST="$EFI_MOUNT/EFI/CLOVER/config.plist"
    
    if grep -q "<key>Debug</key>" "$CONFIG_PLIST" 2>/dev/null; then
        if grep -A1 "<key>Debug</key>" "$CONFIG_PLIST" | grep -q "<true/>"; then
            print_status 0 "Debug logging is enabled in config.plist"
        else
            print_warning "Debug logging is disabled in config.plist"
            echo "   Enable it to see Hardware Validator messages"
        fi
    else
        print_info "Debug key not found in config.plist"
    fi
else
    print_info "config.plist not found or EFI not mounted"
fi
echo ""

# ============================================================================
# SUMMARY
# ============================================================================
echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   Diagnostic Summary                                       ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
echo ""

# Count issues
ISSUES=0

if [ -n "$FAIL_COUNT" ] && [ "$FAIL_COUNT" -ge 3 ]; then
    ((ISSUES++))
    echo -e "${YELLOW}⚠ WARNING:${NC} Boot failure counter is at $FAIL_COUNT (Safe Mode active)"
fi

if ! launchctl list 2>/dev/null | grep -q "com.clover.bootfailreset"; then
    ((ISSUES++))
    echo -e "${YELLOW}⚠ WARNING:${NC} Auto-Reset LaunchDaemon is not running"
fi

if [ -n "$DEBUG_LOG" ] && [ -f "$DEBUG_LOG" ]; then
    if ! grep -q "HardwareValidator:" "$DEBUG_LOG" 2>/dev/null; then
        ((ISSUES++))
        echo -e "${YELLOW}⚠ WARNING:${NC} Hardware Validator messages not found in logs"
    fi
fi

if [ $ISSUES -eq 0 ]; then
    echo -e "${GREEN}✓ All systems operational!${NC}"
    echo ""
    echo "Your Clover Híbrido Inteligente is working correctly."
else
    echo -e "${YELLOW}Found $ISSUES potential issue(s).${NC}"
    echo ""
    echo "Review the warnings above and check VERIFICATION_GUIDE.md for solutions."
fi

echo ""
echo "For more information, see: VERIFICATION_GUIDE.md"
echo ""
