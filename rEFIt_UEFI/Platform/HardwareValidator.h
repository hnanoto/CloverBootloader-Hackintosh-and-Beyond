/*
 * HardwareValidator.h
 *
 *  Created on: Jan 17, 2026
 *      Author: CloverHackyColor Hybrid Team
 *
 *  Purpose: Intelligent hardware detection and configuration validation.
 *           Checks if critical kexts are present for detected hardware.
 */

#ifndef __HARDWARE_VALIDATOR_H__
#define __HARDWARE_VALIDATOR_H__

#include "../cpp_foundation/XStringArray.h"
#include <Platform.h>

class HardwareValidator {
public:
  static void ValidateHardware(const XString8Array &LoadedKexts);

private:
  static void CheckIntelWifi(const XString8Array &LoadedKexts);
  static bool IsKextLoaded(const XString8Array &LoadedKexts,
                           const XString8 &KextName);
};

#endif
