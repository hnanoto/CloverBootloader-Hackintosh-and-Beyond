/*
 * KERNEL_AND_KEXT_PATCHES.cpp
 *
 *  Created on: 4 Feb 2021
 *      Author: jief
 */

#include "KERNEL_AND_KEXT_PATCHES.h"
#include "MacOsVersion.h"

#ifndef DEBUG_ALL
#define DEBUG_KEXT 1
#else
#define DEBUG_KEXT DEBUG_ALL
#endif

#if DEBUG_KEXT == 0
#define DBG(...)
#else
#define DBG(...) DebugLog(DEBUG_KEXT, __VA_ARGS__)
#endif

XBool ABSTRACT_PATCH::IsPatchEnabledByBuildNumber(const XString8& Build)
{
  XBool ret = false;

  if (MatchBuild.isEmpty() || Build.isEmpty()) {
    return true; //undefined matched corresponds to old behavior
  }

  XString8Array mos = Split<XString8Array>(MatchBuild, ","_XS8).trimEachString();

  if ( mos[0] == "All"_XS8) {
    return true;
  }

  for (size_t i = 0; i < mos.size(); ++i) {
    // dot represent MatchOS
    MacOsVersion mosv = mos[i];
    MsgLog("\n check matchBuild %s\n", mos[i].c_str());
    if ( mos[i].contains(Build) ) { // MatchBuild
      //DBG("\nthis patch will activated for OS %ls!\n", mos->array[i]);
      ret =  true;
      break;
    }
  }
  return ret;
}


XBool ABSTRACT_PATCH::IsPatchEnabled(const MacOsVersion& CurrOS)
{
  XBool ret = false;

  if (MatchOS.isEmpty() || CurrOS.isEmpty()) {
    return true; //undefined matched corresponds to old behavior
  }

  XString8Array mos = Split<XString8Array>(MatchOS, ","_XS8).trimEachString();

  if ( mos[0] == "All"_XS8) {
    return true;
  }

  for (size_t i = 0; i < mos.size(); ++i) {
    // dot represent MatchOS
    MacOsVersion mosv = mos[i];
    MsgLog("\n check MatchOS %s\n", mos[i].c_str());
    if ( CurrOS.match(mos[i]) ) {
      //DBG("\nthis patch will activated for OS %ls!\n", mos->array[i]);
      ret =  true;
      break;
    }
  }
  return ret;
}

int KEXT_TO_BLOCK::ShouldBlock(const MacOsVersion& CurrOS) const
{
  if (!MenuItem.BValue || Name.isEmpty()) {  // BValue already == !Disabled
    return 0;
  }

  XString8 matchOS = MatchOS;
  matchOS.trim();
  if (matchOS.isEmpty()) {
    return 1;
  }

  XString8 matchOSLower = matchOS;
  matchOSLower.lowerAscii();
  if (matchOSLower == "all"_XS8) {
    return 1;
  }

  if (CurrOS.isEmpty()) {
    return 0;
  }
  XString8Array mos = Split<XString8Array>(matchOS, ","_XS8).trimEachString();
  for (size_t i = 0; i < mos.size(); ++i) {
    if (mos[i].isEmpty()) {
      continue;
    }
    XString8 entryLower = mos[i];
    entryLower.lowerAscii();
    if (entryLower == "all"_XS8) {
      return 1;
    }
    if (CurrOS.match(mos[i])) {
      return 1;
    }
  }

  return 0;
}


//
//XBool KERNEL_AND_KEXT_PATCHES::IsPatchEnabledByBuildNumber(const XString8& Build)
//{
//  XBool ret = false;
//
//  if (MatchBuild.isEmpty() || Build.isEmpty()) {
//    return true; //undefined matched corresponds to old behavior
//  }
//
//  XString8Array mos = Split<XString8Array>(MatchBuild, ","_XS8).trimEachString();
//  
//  if ( mos[0] == "All"_XS8) {
//    return true;
//  }
//
//  for (size_t i = 0; i < mos.size(); ++i) {
//    // dot represent MatchOS
//    MacOsVersion mosv = mos[i];
//    if ( mos[i].contains(Build) ) { // MatchBuild
//      //DBG("\nthis patch will activated for OS %ls!\n", mos->array[i]);
//      ret =  true;
//      break;
//    }
//  }
//  return ret;
//}
