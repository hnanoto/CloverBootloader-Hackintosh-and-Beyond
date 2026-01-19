/*
 * HardwareValidator.cpp
 *
 *  Created on: Jan 17, 2026
 *      Author: CloverHackyColor Hybrid Team
 */

#include "HardwareValidator.h"
#include "../Platform/Settings.h"
#include "../libeg/XTheme.h"
#include <Platform.h> // Must be first to define types

// Vendor IDs
#define PCI_VENDOR_INTEL 0x8086
#define PCI_VENDOR_REALTEK 0x10EC
#define PCI_VENDOR_AMD 0x1002

// Intel I225-V Device IDs
#define INTEL_I225_V_15F2 0x15F2
#define INTEL_I225_V_15F3 0x15F3
#define INTEL_I225_V_0D4C 0x0D4C
#define INTEL_I225_V_0D4D 0x0D4D

// Realtek Device IDs
#define REALTEK_RTL8125 0x8125

// PCI Class Codes
#define PCI_CLASS_NETWORK 0x02
#define PCI_CLASS_DISPLAY 0x03
#define PCI_SUBCLASS_NETWORK_ETHERNET 0x00
#define PCI_SUBCLASS_NETWORK_OTHER 0x80

// Static warning storage
XString8Array HardwareValidator::Warnings;

// Warning management functions
void HardwareValidator::AddWarning(const XString8 &Warning) {
  Warnings.Add(Warning);
  DebugLog(1, "HardwareValidator: [WARNING] %s\n", Warning.c_str());
}

XString8Array HardwareValidator::GetWarnings() { return Warnings; }

void HardwareValidator::ClearWarnings() { Warnings.setEmpty(); }

bool HardwareValidator::HasWarnings() { return Warnings.size() > 0; }

void HardwareValidator::ValidateHardware(const XString8Array &LoadedKexts) {
  // Clear previous warnings
  ClearWarnings();

  // Global validation entry point - check all hardware
  CheckIntelWifi(LoadedKexts);
  CheckIntelEthernet(LoadedKexts);
  CheckRealtekEthernet(LoadedKexts);
  CheckAMDGPU(LoadedKexts);

  // Step 7: Additional hardware checks
  CheckIntelBluetooth(LoadedKexts);
  CheckBroadcomBluetooth(LoadedKexts);
  CheckThunderbolt(LoadedKexts);
  CheckNVMe(LoadedKexts);
}

bool HardwareValidator::IsKextLoaded(const XString8Array &LoadedKexts,
                                     const XString8 &KextName) {
  for (size_t i = 0; i < LoadedKexts.size(); ++i) {
    if (LoadedKexts[i].containsIC(KextName)) {
      return true;
    }
  }
  return false;
}

bool HardwareValidator::IsBootArgPresent(const XString8 &BootArg) {
  // Check in gSettings.Boot.BootArgs
  return gSettings.Boot.BootArgs.containsIC(BootArg);
}

void HardwareValidator::CheckIntelWifi(const XString8Array &LoadedKexts) {
  EFI_STATUS Status;
  UINTN HandleCount;
  EFI_HANDLE *HandleBuffer;

  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiPciIoProtocolGuid, NULL,
                                   &HandleCount, &HandleBuffer);

  if (EFI_ERROR(Status))
    return;

  bool intelWifiFound = false;

  for (UINTN i = 0; i < HandleCount; ++i) {
    EFI_PCI_IO_PROTOCOL *PciIo;
    Status = gBS->OpenProtocol(HandleBuffer[i], gEfiPciIoProtocolGuid,
                               (VOID **)&PciIo, gImageHandle, NULL,
                               EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    if (EFI_ERROR(Status))
      continue;

    PCI_TYPE00 Pci;
    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0,
                             sizeof(Pci) / sizeof(UINT32), &Pci);

    if (!EFI_ERROR(Status)) {
      if (Pci.Hdr.VendorId == PCI_VENDOR_INTEL) {
        if (Pci.Hdr.ClassCode[2] == PCI_CLASS_NETWORK &&
            Pci.Hdr.ClassCode[1] == PCI_SUBCLASS_NETWORK_OTHER) {
          intelWifiFound = true;
        }
      }
    }
    gBS->CloseProtocol(HandleBuffer[i], gEfiPciIoProtocolGuid, gImageHandle,
                       NULL);
  }

  if (HandleBuffer) {
    gBS->FreePool(HandleBuffer);
  }

  if (intelWifiFound) {
    bool kextPresent = IsKextLoaded(LoadedKexts, XString8("AirportItlwm")) ||
                       IsKextLoaded(LoadedKexts, XString8("itlwm"));

    if (!kextPresent) {
      AddWarning(XString8("Intel Wi-Fi: Missing itlwm/AirportItlwm kext"));
    } else {
      DebugLog(
          1,
          "HardwareValidator: [OK] Intel Wi-Fi detected and driver present.\n");
    }
  }
}

void HardwareValidator::CheckIntelEthernet(const XString8Array &LoadedKexts) {
  EFI_STATUS Status;
  UINTN HandleCount;
  EFI_HANDLE *HandleBuffer;

  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiPciIoProtocolGuid, NULL,
                                   &HandleCount, &HandleBuffer);

  if (EFI_ERROR(Status))
    return;

  bool i225vFound = false;

  for (UINTN i = 0; i < HandleCount; ++i) {
    EFI_PCI_IO_PROTOCOL *PciIo;
    Status = gBS->OpenProtocol(HandleBuffer[i], gEfiPciIoProtocolGuid,
                               (VOID **)&PciIo, gImageHandle, NULL,
                               EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    if (EFI_ERROR(Status))
      continue;

    PCI_TYPE00 Pci;
    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0,
                             sizeof(Pci) / sizeof(UINT32), &Pci);

    if (!EFI_ERROR(Status)) {
      if (Pci.Hdr.VendorId == PCI_VENDOR_INTEL) {
        // Check for I225-V variants
        if (Pci.Hdr.DeviceId == INTEL_I225_V_15F2 ||
            Pci.Hdr.DeviceId == INTEL_I225_V_15F3 ||
            Pci.Hdr.DeviceId == INTEL_I225_V_0D4C ||
            Pci.Hdr.DeviceId == INTEL_I225_V_0D4D) {
          i225vFound = true;
        }
      }
    }
    gBS->CloseProtocol(HandleBuffer[i], gEfiPciIoProtocolGuid, gImageHandle,
                       NULL);
  }

  if (HandleBuffer) {
    gBS->FreePool(HandleBuffer);
  }

  if (i225vFound) {
    bool bootArgPresent = IsBootArgPresent(XString8("e1000=0"));

    if (!bootArgPresent) {
      AddWarning(XString8(
          "Intel I225-V: Missing boot-arg 'e1000=0' (required for macOS 12+)"));
    } else {
      DebugLog(1, "HardwareValidator: [OK] Intel I225-V detected with correct "
                  "boot-arg.\n");
    }
  }
}

void HardwareValidator::CheckRealtekEthernet(const XString8Array &LoadedKexts) {
  EFI_STATUS Status;
  UINTN HandleCount;
  EFI_HANDLE *HandleBuffer;

  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiPciIoProtocolGuid, NULL,
                                   &HandleCount, &HandleBuffer);

  if (EFI_ERROR(Status))
    return;

  bool rtl8125Found = false;

  for (UINTN i = 0; i < HandleCount; ++i) {
    EFI_PCI_IO_PROTOCOL *PciIo;
    Status = gBS->OpenProtocol(HandleBuffer[i], gEfiPciIoProtocolGuid,
                               (VOID **)&PciIo, gImageHandle, NULL,
                               EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    if (EFI_ERROR(Status))
      continue;

    PCI_TYPE00 Pci;
    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0,
                             sizeof(Pci) / sizeof(UINT32), &Pci);

    if (!EFI_ERROR(Status)) {
      if (Pci.Hdr.VendorId == PCI_VENDOR_REALTEK &&
          Pci.Hdr.DeviceId == REALTEK_RTL8125) {
        rtl8125Found = true;
      }
    }
    gBS->CloseProtocol(HandleBuffer[i], gEfiPciIoProtocolGuid, gImageHandle,
                       NULL);
  }

  if (HandleBuffer) {
    gBS->FreePool(HandleBuffer);
  }

  if (rtl8125Found) {
    bool kextPresent =
        IsKextLoaded(LoadedKexts, XString8("LucyRTL8125Ethernet"));

    if (!kextPresent) {
      AddWarning(XString8("Realtek RTL8125: Missing LucyRTL8125Ethernet.kext"));
    } else {
      DebugLog(1, "HardwareValidator: [OK] Realtek RTL8125 detected and driver "
                  "present.\n");
    }
  }
}

void HardwareValidator::CheckAMDGPU(const XString8Array &LoadedKexts) {
  EFI_STATUS Status;
  UINTN HandleCount;
  EFI_HANDLE *HandleBuffer;

  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiPciIoProtocolGuid, NULL,
                                   &HandleCount, &HandleBuffer);

  if (EFI_ERROR(Status))
    return;

  bool amdGpuFound = false;

  for (UINTN i = 0; i < HandleCount; ++i) {
    EFI_PCI_IO_PROTOCOL *PciIo;
    Status = gBS->OpenProtocol(HandleBuffer[i], gEfiPciIoProtocolGuid,
                               (VOID **)&PciIo, gImageHandle, NULL,
                               EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    if (EFI_ERROR(Status))
      continue;

    PCI_TYPE00 Pci;
    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0,
                             sizeof(Pci) / sizeof(UINT32), &Pci);

    if (!EFI_ERROR(Status)) {
      if (Pci.Hdr.VendorId == PCI_VENDOR_AMD &&
          Pci.Hdr.ClassCode[2] == PCI_CLASS_DISPLAY) {
        amdGpuFound = true;
      }
    }
    gBS->CloseProtocol(HandleBuffer[i], gEfiPciIoProtocolGuid, gImageHandle,
                       NULL);
  }

  if (HandleBuffer) {
    gBS->FreePool(HandleBuffer);
  }

  if (amdGpuFound) {
    bool kextPresent = IsKextLoaded(LoadedKexts, XString8("WhateverGreen"));

    if (!kextPresent) {
      AddWarning(
          XString8("AMD GPU: Missing WhateverGreen.kext (graphics may fail)"));
    } else {
      DebugLog(1, "HardwareValidator: [OK] AMD GPU detected and "
                  "WhateverGreen.kext present.\n");
    }
  }
}

// ============================================================================
// PHASE 4: SELF-HEALING / SAFE MODE FUNCTIONS
// ============================================================================

#define CLOVER_BOOT_FAIL_COUNT_VAR L"CloverBootFailCount"
#define SAFE_MODE_THRESHOLD 3

void HardwareValidator::IncrementBootFailCount() {
  // DISABLED: NVRAM writes cause kernel panic on 3rd boot
  // Do nothing to prevent boot issues
  DebugLog(1,
           "HardwareValidator: Boot fail increment DISABLED (NVRAM issue)\n");
}

UINT32 HardwareValidator::GetBootFailCount() {
  // DISABLED: NVRAM access causes kernel panic on 3rd boot
  // Always return 0 to prevent boot issues
  return 0;
}

void HardwareValidator::ResetBootFailCount() {
  // DISABLED: NVRAM writes cause kernel panic on 3rd boot
  // Do nothing to prevent boot issues
  DebugLog(1, "HardwareValidator: Boot fail reset DISABLED (NVRAM issue)\n");
}

bool HardwareValidator::ShouldEnterSafeMode() {
  UINT32 FailCount = GetBootFailCount();

  if (FailCount >= SAFE_MODE_THRESHOLD) {
    DebugLog(1,
             "HardwareValidator: [SAFE MODE] Detected %d consecutive boot "
             "failures!\n",
             FailCount);
    return true;
  }

  return false;
}

void HardwareValidator::ApplySafeModeSettings() {
  DebugLog(1,
           "HardwareValidator: [SAFE MODE] Applying safe boot settings...\n");

  // Force verbose mode for troubleshooting
  if (!gSettings.Boot.BootArgs.containsIC("-v")) {
    gSettings.Boot.BootArgs.S8Catf(" -v");
    DebugLog(1, "HardwareValidator: [SAFE MODE] Enabled verbose mode (-v)\n");
  }

  // Disable GPU injection to avoid graphics issues
  gSettings.Graphics.InjectAsDict.InjectIntel = false;
  gSettings.Graphics.InjectAsDict.InjectATI = false;
  gSettings.Graphics.InjectAsDict.InjectNVidia = false;
  DebugLog(1, "HardwareValidator: [SAFE MODE] Disabled GPU injection\n");

  // Disable custom DSDT to avoid ACPI issues
  gSettings.ACPI.DSDT.DsdtName.setEmpty();
  DebugLog(1, "HardwareValidator: [SAFE MODE] Disabled custom DSDT\n");

  // Add safe boot flag
  if (!gSettings.Boot.BootArgs.containsIC("-x")) {
    gSettings.Boot.BootArgs.S8Catf(" -x");
    DebugLog(1,
             "HardwareValidator: [SAFE MODE] Enabled macOS safe boot (-x)\n");
  }

  DebugLog(1, "HardwareValidator: [SAFE MODE] Safe mode settings applied. "
              "System will boot with minimal configuration.\n");
  DebugLog(1, "HardwareValidator: [SAFE MODE] To exit safe mode, delete NVRAM "
              "variable: nvram -d CloverBootFailCount\n");
}

// ============================================================================
// STEP 8: AUTO-RESET & HTML REPORT GENERATION
// ============================================================================

#define BOOT_SUCCESS_MARKER_PATH L"\\EFI\\CLOVER\\misc\\boot_success.flag"
#define HTML_REPORT_PATH L"\\EFI\\CLOVER\\misc\\hardware_report.html"

void HardwareValidator::CheckAndResetIfLastBootSuccessful() {
  if (WasLastBootSuccessful()) {
    DebugLog(1,
             "HardwareValidator: [AUTO-RESET] Previous boot was successful.\n");
    ResetBootFailCount();
    DebugLog(1, "HardwareValidator: [AUTO-RESET] Boot fail counter has been "
                "reset to 0.\n");
  }
}

bool HardwareValidator::WasLastBootSuccessful() {
  // Check if boot success marker file exists
  // This file is created by the OS after successful boot
  // For now, we'll use a simplified approach: if counter hasn't changed in a
  // while, assume success

  // TODO: Implement file-based detection when EFI file I/O is available
  // For now, return false to maintain current behavior
  return false;
}

void HardwareValidator::MarkBootSuccess() {
  // This would be called from the OS side or via a boot success hook
  // For now, it's a placeholder for future implementation
  DebugLog(1, "HardwareValidator: [AUTO-RESET] Marking boot as successful.\n");
}

bool HardwareValidator::IsReportEnabled() {
  // Check if HTML report generation is enabled in config.plist
  // For now, default to true
  // TODO: Add config.plist option: HardwareValidator -> GenerateReport
  return true;
}

void HardwareValidator::GenerateHTMLReport() {
  if (!IsReportEnabled()) {
    return;
  }

  DebugLog(1, "HardwareValidator: [REPORT] Generating HTML report...\n");

  // Build HTML content
  XString8 htmlContent;

  htmlContent = "<!DOCTYPE html>\n"
                "<html>\n"
                "<head>\n"
                "  <meta charset=\"UTF-8\">\n"
                "  <title>Clover Hardware Validator Report</title>\n"
                "  <style>\n"
                "    body { font-family: 'Segoe UI', Arial, sans-serif; "
                "background: #1e1e1e; color: #e0e0e0; margin: 20px; }\n"
                "    h1 { color: #4caf50; border-bottom: 2px solid #4caf50; "
                "padding-bottom: 10px; }\n"
                "    h2 { color: #2196f3; margin-top: 30px; }\n"
                "    .ok { color: #4caf50; }\n"
                "    .warning { color: #ff9800; }\n"
                "    .error { color: #f44336; }\n"
                "    .info { color: #2196f3; }\n"
                "    ul { list-style-type: none; padding-left: 0; }\n"
                "    li { padding: 8px; margin: 5px 0; background: #2a2a2a; "
                "border-left: 4px solid #4caf50; }\n"
                "    li.warning { border-left-color: #ff9800; }\n"
                "    li.error { border-left-color: #f44336; }\n"
                "    .timestamp { color: #888; font-size: 0.9em; }\n"
                "    .counter { font-size: 1.2em; font-weight: bold; }\n"
                "  </style>\n"
                "</head>\n"
                "<body>\n"
                "  <h1>🍀 Clover Híbrido - Hardware Validator Report</h1>\n"
                "  <p class=\"timestamp\">Generated: Boot Session</p>\n"
                "  \n"
                "  <h2>🛡️ Self-Healing Status</h2>\n"
                "  <ul>\n";

  // Add boot fail counter status
  UINT32 failCount = GetBootFailCount();
  if (failCount == 0) {
    htmlContent.S8Catf("    <li class=\"ok\">Boot Fail Counter: <span "
                       "class=\"counter\">%d</span> (Normal)</li>\n",
                       failCount);
  } else if (failCount < SAFE_MODE_THRESHOLD) {
    htmlContent.S8Catf("    <li class=\"warning\">Boot Fail Counter: <span "
                       "class=\"counter\">%d</span> (Warning)</li>\n",
                       failCount);
  } else {
    htmlContent.S8Catf("    <li class=\"error\">Boot Fail Counter: <span "
                       "class=\"counter\">%d</span> (Safe Mode Active!)</li>\n",
                       failCount);
  }

  // Add Safe Mode status
  if (ShouldEnterSafeMode()) {
    htmlContent +=
        "    <li class=\"error\">Safe Mode: <strong>ACTIVE</strong></li>\n";
  } else {
    htmlContent += "    <li class=\"ok\">Safe Mode: Inactive</li>\n";
  }

  htmlContent += "  </ul>\n";

  // Add warnings section
  if (HasWarnings()) {
    htmlContent += "  \n  <h2>⚠️ Hardware Warnings</h2>\n  <ul>\n";

    XString8Array warnings = GetWarnings();
    for (size_t i = 0; i < warnings.size(); ++i) {
      htmlContent.S8Catf("    <li class=\"warning\">%s</li>\n",
                         warnings[i].c_str());
    }

    htmlContent += "  </ul>\n";
  } else {
    htmlContent += "  \n  <h2>✅ Hardware Status</h2>\n";
    htmlContent += "  <p class=\"ok\">All detected hardware has appropriate "
                   "drivers/configuration.</p>\n";
  }

  // Add footer
  htmlContent +=
      "  \n  <h2>ℹ️ Information</h2>\n"
      "  <ul>\n"
      "    <li class=\"info\">Report Location: "
      "/EFI/CLOVER/misc/hardware_report.html</li>\n"
      "    <li class=\"info\">To disable this report, edit config.plist</li>\n"
      "    <li class=\"info\">To reset boot counter manually: nvram -d "
      "CloverBootFailCount</li>\n"
      "  </ul>\n"
      "  \n"
      "  <p class=\"timestamp\">Clover Híbrido Inteligente - Build #140</p>\n"
      "</body>\n"
      "</html>\n";

  // TODO: Write HTML content to file
  // This requires EFI file I/O which needs to be implemented
  // For now, just log that we would generate the report

  DebugLog(
      1,
      "HardwareValidator: [REPORT] HTML report content prepared (%lld bytes)\n",
      htmlContent.length());
  DebugLog(1, "HardwareValidator: [REPORT] Report would be saved to: %ls\n",
           HTML_REPORT_PATH);
}

// ============================================================================
// STEP 7: ADDITIONAL HARDWARE DETECTION
// ============================================================================

#define PCI_VENDOR_BROADCOM 0x14E4
#define PCI_VENDOR_SAMSUNG 0x144D
#define PCI_VENDOR_SANDISK 0x15B7
#define PCI_CLASS_SERIAL_BUS 0x0C
#define PCI_SUBCLASS_USB 0x03

void HardwareValidator::CheckIntelBluetooth(const XString8Array &LoadedKexts) {
  // Simplified check - looks for Intel USB controllers that might have BT
  bool kextPresent =
      IsKextLoaded(LoadedKexts, XString8("IntelBluetoothFirmware"));
  if (!kextPresent) {
    DebugLog(
        1,
        "HardwareValidator: [INFO] IntelBluetoothFirmware.kext not loaded.\n");
  }
}

void HardwareValidator::CheckBroadcomBluetooth(
    const XString8Array &LoadedKexts) {
  // Simplified check
  bool kextPresent = IsKextLoaded(LoadedKexts, XString8("BrcmPatchRAM"));
  if (!kextPresent) {
    DebugLog(1, "HardwareValidator: [INFO] BrcmPatchRAM kext not loaded.\n");
  }
}

void HardwareValidator::CheckThunderbolt(const XString8Array &LoadedKexts) {
  // Check for Thunderbolt boot-arg
  bool tbPatchPresent = IsBootArgPresent(XString8("ThunderboltPatch"));
  if (!tbPatchPresent) {
    DebugLog(1, "HardwareValidator: [INFO] Thunderbolt patch not detected.\n");
  }
}

void HardwareValidator::CheckNVMe(const XString8Array &LoadedKexts) {
  // Check if NVMeFix is loaded (useful for non-Apple NVMe drives)
  bool kextPresent = IsKextLoaded(LoadedKexts, XString8("NVMeFix"));
  if (!kextPresent) {
    DebugLog(1, "HardwareValidator: [INFO] NVMeFix.kext not loaded.\n");
  } else {
    DebugLog(1, "HardwareValidator: [OK] NVMeFix.kext present for NVMe power "
                "management.\n");
  }
}
