# Clover Self-Healing Auto-Reset Script

## Overview
This script automatically resets the Clover boot failure counter (`CloverBootFailCount`) after a successful macOS boot. This prevents false Safe Mode triggers and completes the self-healing cycle.

## How It Works
1. **Boot Failure Detection**: Clover increments `CloverBootFailCount` in NVRAM at each boot attempt
2. **Safe Mode Trigger**: If count reaches 3+, Clover automatically enters Safe Mode
3. **Successful Boot**: macOS boots successfully
4. **Auto-Reset**: This LaunchDaemon runs and resets the counter to 0

## Installation

### Automatic Installation (Recommended)
```bash
# Run the installer script
sudo bash install-bootfail-reset.sh
```

### Manual Installation
```bash
# 1. Copy the script to /usr/local/bin/
sudo cp CloverBootFailReset.sh /usr/local/bin/
sudo chmod +x /usr/local/bin/CloverBootFailReset.sh

# 2. Copy the LaunchDaemon plist
sudo cp com.clover.bootfailreset.plist /Library/LaunchDaemons/

# 3. Set correct permissions
sudo chown root:wheel /Library/LaunchDaemons/com.clover.bootfailreset.plist
sudo chmod 644 /Library/LaunchDaemons/com.clover.bootfailreset.plist

# 4. Load the LaunchDaemon
sudo launchctl load /Library/LaunchDaemons/com.clover.bootfailreset.plist
```

## Verification

### Check if the LaunchDaemon is loaded:
```bash
sudo launchctl list | grep clover
```

### View the logs:
```bash
# Main log
cat /var/log/clover-bootfail-reset.log

# Standard output
cat /var/log/clover-bootfailreset.stdout.log

# Standard error
cat /var/log/clover-bootfailreset.stderr.log
```

### Check current boot fail count:
```bash
nvram -p | grep CloverBootFailCount
```

## Uninstallation
```bash
# Unload the LaunchDaemon
sudo launchctl unload /Library/LaunchDaemons/com.clover.bootfailreset.plist

# Remove files
sudo rm /Library/LaunchDaemons/com.clover.bootfailreset.plist
sudo rm /usr/local/bin/CloverBootFailReset.sh
sudo rm /var/log/clover-bootfail-reset.log
sudo rm /var/log/clover-bootfailreset.*.log
```

## Manual Reset (Without Script)
If you need to manually reset the counter:
```bash
sudo nvram -d CloverBootFailCount
```

## Troubleshooting

### Script not running at boot
- Check if LaunchDaemon is loaded: `sudo launchctl list | grep clover`
- Check permissions: `ls -l /Library/LaunchDaemons/com.clover.bootfailreset.plist`
- Check logs for errors

### Permission denied errors
- Ensure the script is executable: `chmod +x /usr/local/bin/CloverBootFailReset.sh`
- Ensure LaunchDaemon runs as root (UserName = root in plist)

## Files
- `CloverBootFailReset.sh` - Main reset script
- `com.clover.bootfailreset.plist` - LaunchDaemon configuration
- `install-bootfail-reset.sh` - Automated installer
- `README.md` - This file

## License
Part of the Clover Hybrid Intelligent Bootloader project.
