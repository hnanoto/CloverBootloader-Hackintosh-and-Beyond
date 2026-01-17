/*
 * HardwareValidator.cpp
 *
 *  Created on: Jan 17, 2026
 *      Author: CloverHackyColor Hybrid Team
 */

#include "HardwareValidator.h"
#include "../Include/Efi.h"
#include "../Platform/Settings.h"
#include "../libeg/XTheme.h" // For future GUI alerts

// Intel Vendor ID
#define PCI_VENDOR_INTEL 0x8086

// Network Class Codes
#define PCI_CLASS_NETWORK 0x02
#define PCI_SUBCLASS_NETWORK_OTHER 0x80

void HardwareValidator::ValidateHardware(const XString8Array &LoadedKexts) {
  // Global validation entry point
  // We can add more checks here later (Ethernet, Audio, etc.)

  CheckIntelWifi(LoadedKexts);
}

bool HardwareValidator::IsKextLoaded(const XString8Array &LoadedKexts,
                                     const XString8 &KextName) {
  for (size_t i = 0; i < LoadedKexts.size(); ++i) {
    if (LoadedKexts[i].IC().contains(KextName.IC())) {
      return true;
    }
  }
  return false;
}

void HardwareValidator::CheckIntelWifi(const XString8Array &LoadedKexts) {
  EFI_STATUS Status;
  UINTN HandleCount;
  EFI_HANDLE *HandleBuffer;

  // Get all PciIo handles
  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiPciIoProtocolGuid, NULL,
                                   &HandleCount, &HandleBuffer);

  if (EFI_ERROR(Status))
    return;

  bool intelWifiFound = false;

  for (UINTN i = 0; i < HandleCount; ++i) {
    EFI_PCI_IO_PROTOCOL *PciIo;
    Status = gBS->OpenProtocol(HandleBuffer[i], &gEfiPciIoProtocolGuid,
                               (VOID **)&PciIo, gImageHandle, NULL,
                               EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    if (EFI_ERROR(Status))
      continue;

    PCI_TYPE00 Pci;
    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0,
                             sizeof(Pci) / sizeof(UINT32), &Pci);

    if (!EFI_ERROR(Status)) {
      // Check for Intel Vendor
      if (Pci.Hdr.VendorId == PCI_VENDOR_INTEL) {
        // Check for Network Controller (Class 0x02, Subclass 0x80 usually for
        // Wifi/Other)
        if (Pci.Hdr.ClassCode[2] == PCI_CLASS_NETWORK &&
            Pci.Hdr.ClassCode[1] == PCI_SUBCLASS_NETWORK_OTHER) {
          intelWifiFound = true;
          // We found one, no need to check all if we just want to warn once
          // But keeping loop if we want detailed logging later
        }
      }
    }
  }

  if (HandleBuffer) {
    gBS->FreePool(HandleBuffer);
  }

  if (intelWifiFound) {
    bool kextPresent = IsKextLoaded(LoadedKexts, "AirportItlwm") ||
                       IsKextLoaded(LoadedKexts, "itlwm");

    if (!kextPresent) {
      // ALERT: Hardware found but Driver missing!
      // For now, print to log. In phase 2, we will show a popup.
      DebugLog(1, "HardwareValidator: [WARNING] Intel Wi-Fi detected but no "
                  "itlwm/AirportItlwm kext loaded!\n");
      // GlobalConfig.HardwareWarnings.Add("Intel Wi-Fi found, but kext
      // missing.");
    } else {
      DebugLog(
          1,
          "HardwareValidator: [OK] Intel Wi-Fi detected and driver present.\n");
    }
  }
}
