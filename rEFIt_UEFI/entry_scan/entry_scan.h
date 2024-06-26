/*
 * refit/scan/entry_scan.h
 *
 * Copyright (c) 2006-2010 Christoph Pfisterer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *  * Neither the name of Christoph Pfisterer nor the names of the
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "../gui/menu_items/menu_items.h"

extern REFIT_MENU_ITEM_RETURN MenuEntryReturn;
extern REFIT_MENU_ITEM_OPTIONS MenuEntryOptions;
extern REFIT_MENU_ITEM_ABOUT MenuEntryAbout;
extern REFIT_MENU_ITEM_RESET MenuEntryReset;
extern REFIT_MENU_ITEM_SHUTDOWN MenuEntryShutdown;
//extern REFIT_MENU_ENTRY MenuEntryHelp;
//extern REFIT_MENU_ENTRY MenuEntryExit;

// common
const XIcon& ScanVolumeDefaultIcon(REFIT_VOLUME *Volume, IN UINT8 OSType, const EFI_DEVICE_PATH_PROTOCOL *DevicePath);


// Ask user for file path from directory menu
XBool AskUserForFilePathFromDir(const CHAR16 *Title OPTIONAL, IN REFIT_VOLUME *Volume,
                                  const CHAR16 *ParentPath, const EFI_FILE *Dir,
                                  OUT EFI_DEVICE_PATH_PROTOCOL **Result);
// Ask user for file path from volumes menu
XBool AskUserForFilePathFromVolumes(const CHAR16 *Title OPTIONAL, OUT EFI_DEVICE_PATH_PROTOCOL **Result);
// Ask user for file path
XBool AskUserForFilePath(IN CHAR16 *Title OPTIONAL, IN EFI_DEVICE_PATH_PROTOCOL *Root OPTIONAL, OUT EFI_DEVICE_PATH_PROTOCOL **Result);

// legacy
void ScanLegacy(void);
void AddCustomLegacy(void);

// loader
void ScanLoader(void);
void AddCustomEntries(void);
XBool IsCustomBootEntry(IN LOADER_ENTRY *Entry);

// tool
void ScanTool(void);
void AddCustomTool(void);

// locked graphics
EFI_STATUS LockBootScreen(void);
EFI_STATUS UnlockBootScreen(void);

// secure boot
#ifdef ENABLE_SECURE_BOOT
#define PLATFORM_DATABASE_NAME L"PK"
#define PLATFORM_DATABASE_GUID gEfiGlobalVariableGuid
#define EXCHANGE_DATABASE_NAME L"KEK"
#define EXCHANGE_DATABASE_GUID gEfiGlobalVariableGuid
#define AUTHORIZED_DATABASE_NAME EFI_IMAGE_SECURITY_DATABASE
#define AUTHORIZED_DATABASE_GUID gEfiImageSecurityDatabaseGuid
#define UNAUTHORIZED_DATABASE_NAME EFI_IMAGE_SECURITY_DATABASE1
#define UNAUTHORIZED_DATABASE_GUID gEfiImageSecurityDatabaseGuid
#define DEFAULT_PLATFORM_DATABASE_NAME L"PKDefault"
#define DEFAULT_PLATFORM_DATABASE_GUID gEfiGlobalVariableGuid
#define DEFAULT_EXCHANGE_DATABASE_NAME L"KEKDefault"
#define DEFAULT_EXCHANGE_DATABASE_GUID gEfiGlobalVariableGuid
#define DEFAULT_AUTHORIZED_DATABASE_NAME L"dbDefault"
#define DEFAULT_AUTHORIZED_DATABASE_GUID gEfiGlobalVariableGuid
#define DEFAULT_UNAUTHORIZED_DATABASE_NAME L"dbxDefault"
#define DEFAULT_UNAUTHORIZED_DATABASE_GUID gEfiGlobalVariableGuid

void AddSecureBootTool(void);
EFI_STATUS InstallSecureBoot(void);
void UninstallSecureBoot(void);
void EnableSecureBoot(void);
void DisableSecureBoot(void);
XBool ConfigureSecureBoot(void);
CONST CHAR16 *SecureBootPolicyToStr(IN UINTN Policy);
EFI_STATUS VerifySecureBootImage(IN CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath);
UINTN QuerySecureBootUser(IN CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath);
EFI_STATUS EnrollSecureBootKeys(IN void    *AuthorizedDatabase,
                                IN UINTN    AuthorizedDatabaseSize,
                                IN XBool    WantDefaultKeys);
EFI_STATUS ClearSecureBootKeys(void);

// secure boot database
void *GetSignatureDatabase(const wchar_t* DatabaseName,
                           const EFI_GUID& DatabaseGuid,
                           OUT UINTN    * DatabaseSize);
EFI_STATUS SetSignatureDatabase(const wchar_t* DatabaseName,
                                const GUID&  DatabaseGuid,
                                IN void     *  Database,
                                IN UINTN       DatabaseSize);

// secure boot authorized database
void *GetAuthorizedDatabase(UINTN *DatabaseSize);
EFI_STATUS SetAuthorizedDatabase(IN void  *Database,
                                 IN UINTN  DatabaseSize);
EFI_STATUS ClearAuthorizedDatabase(void);
void *GetImageSignatureDatabase(IN void    *FileBuffer,
                                IN UINT64   FileSize,
                                IN UINTN   *DatabaseSize,
                                IN XBool    HashIfNoDatabase);
EFI_STATUS AppendImageDatabaseToAuthorizedDatabase(IN void  *Database,
                                                   IN UINTN  DatabaseSize);
EFI_STATUS RemoveImageDatabaseFromAuthorizedDatabase(IN void  *Database,
                                                     IN UINTN  DatabaseSize);
EFI_STATUS AppendImageToAuthorizedDatabase(IN CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath,
                                           IN void                           *FileBuffer,
                                           IN UINTN                           FileSize);
EFI_STATUS RemoveImageFromAuthorizedDatabase(IN CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath,
                                             IN void                           *FileBuffer,
                                             IN UINTN                           FileSize);
EFI_STATUS AppendSignatureDatabaseToDatabase(IN OUT void  **Database,
                                             IN OUT UINTN  *DatabaseSize,
                                             IN     void   *SignatureDatabase,
                                             IN     UINTN   SignatureDatabaseSize);
EFI_STATUS AppendSignatureToDatabase(IN OUT void**         Database,
                                     IN OUT UINTN*         DatabaseSize,
                                     const EFI_GUID&  SignatureType,
                                     IN     void*          Signature,
                                     IN     UINTN      SignatureSize);
#endif //ENABLE_SECURE_BOOT
