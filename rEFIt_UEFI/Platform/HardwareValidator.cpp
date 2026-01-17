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

void HardwareValidator::ValidateHardware(const XString8Array &LoadedKexts) {
  // Global validation entry point - check all hardware
  CheckIntelWifi(LoadedKexts);
  CheckIntelEthernet(LoadedKexts);
  CheckRealtekEthernet(LoadedKexts);
  CheckAMDGPU(LoadedKexts);
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
      DebugLog(1, "HardwareValidator: [WARNING] Intel Wi-Fi detected but no "
                  "itlwm/AirportItlwm kext loaded!\n");
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
      DebugLog(1, "HardwareValidator: [WARNING] Intel I225-V detected but "
                  "boot-arg 'e1000=0' is missing!\n");
      DebugLog(1, "                   Network may not work on macOS 12+ "
                  "without this boot-arg.\n");
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
      DebugLog(1, "HardwareValidator: [WARNING] Realtek RTL8125 detected but "
                  "LucyRTL8125Ethernet.kext is missing!\n");
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
      DebugLog(1, "HardwareValidator: [WARNING] AMD GPU detected but "
                  "WhateverGreen.kext is missing!\n");
      DebugLog(1, "                   Graphics acceleration may fail without "
                  "this kext.\n");
    } else {
      DebugLog(1, "HardwareValidator: [OK] AMD GPU detected and "
                  "WhateverGreen.kext present.\n");
    }
  }
}
