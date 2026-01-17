/*
 * HardwareValidator.cpp
 *
 *  Created on: Jan 17, 2026
 *      Author: CloverHackyColor Hybrid Team
 */

#include "HardwareValidator.h"
#include "../libeg/XTheme.h"
#include <Platform.h> // Must be first to define types

// Intel Vendor ID
#define PCI_VENDOR_INTEL 0x8086

// Network Class Codes
#define PCI_CLASS_NETWORK 0x02
#define PCI_SUBCLASS_NETWORK_OTHER 0x80

void HardwareValidator::ValidateHardware(const XString8Array &LoadedKexts) {
  // Global validation entry point
  CheckIntelWifi(LoadedKexts);
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
        }
      }
    }
    gBS->CloseProtocol(HandleBuffer[i], &gEfiPciIoProtocolGuid, gImageHandle,
                       NULL);
  }

  if (HandleBuffer) {
    gBS->FreePool(HandleBuffer);
  }

  if (intelWifiFound) {
    // Explicitly construct XString8 to avoid conversion errors
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
