#include <Uefi.h>

// Stub function to satisfy linker for OpenCore drivers running in Clover
// environment
void EFIAPI DebugLogForOC(IN INTN DebugLevel, IN CONST CHAR8 *FormatString,
                          ...) {
  (void)DebugLevel;
  (void)FormatString;
}
