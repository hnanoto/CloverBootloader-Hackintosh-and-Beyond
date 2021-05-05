/*
 * CompareSettings.h
 *
 *  Created on: Feb 2, 2021
 *      Author: jief
 */

#ifndef _CONFIGPLIST_COMPARESETTINGSGUI_H_
#define _CONFIGPLIST_COMPARESETTINGSGUI_H_

#include "../../Platform/Settings.h"
#include "../../../../rEFIt_UEFI/Settings/ConfigPlist/ConfigPlistClass.h"

void CompareGUI(const XString8& label, const SETTINGS_DATA::GUIClass& oldS, const ConfigPlistClass::GUI_Class& newS);



#endif /* _CONFIGPLIST_COMPARESETTINGSGUI_H_ */