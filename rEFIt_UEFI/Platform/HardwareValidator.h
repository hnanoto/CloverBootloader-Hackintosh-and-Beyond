/*
 * HardwareValidator.h
 *
 *  Created on: Jan 17, 2026
 *      Author: CloverHackyColor Hybrid Team
 *
 *  Purpose: Intelligent hardware detection and configuration validation.
 *           Checks if critical kexts are present for detected hardware.
 *           Implements self-healing via boot failure detection.
 */

#ifndef __HARDWARE_VALIDATOR_H__
#define __HARDWARE_VALIDATOR_H__

#include "../cpp_foundation/XStringArray.h"
#include <Platform.h>

class HardwareValidator {
public:
  static void ValidateHardware(const XString8Array &LoadedKexts);

  // Self-healing / Safe Mode functions
  static void IncrementBootFailCount();
  static UINT32 GetBootFailCount();
  static void ResetBootFailCount();
  static bool ShouldEnterSafeMode();
  static void ApplySafeModeSettings();

private:
  // Wi-Fi validation
  static void CheckIntelWifi(const XString8Array &LoadedKexts);

  // Ethernet validation
  static void CheckIntelEthernet(const XString8Array &LoadedKexts);
  static void CheckRealtekEthernet(const XString8Array &LoadedKexts);

  // GPU validation
  static void CheckAMDGPU(const XString8Array &LoadedKexts);

  // Helper functions
  static bool IsKextLoaded(const XString8Array &LoadedKexts,
                           const XString8 &KextName);
  static bool IsBootArgPresent(const XString8 &BootArg);
};

#endif
