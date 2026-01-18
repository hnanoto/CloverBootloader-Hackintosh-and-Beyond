# 🎥 Tutorial Visual - Clover Híbrido Inteligente

## Guia Passo-a-Passo com Exemplos Práticos

Este tutorial mostra **exatamente** o que você verá ao usar o Clover Híbrido Inteligente.

---

## 📋 Índice

1. [Instalação Inicial](#1-instalação-inicial)
2. [Primeiro Boot - Verificação](#2-primeiro-boot---verificação)
3. [Testando o Hardware Validator](#3-testando-o-hardware-validator)
4. [Testando o Self-Healing / Safe Mode](#4-testando-o-self-healing--safe-mode)
5. [Instalando o Auto-Reset](#5-instalando-o-auto-reset)
6. [Verificação Final com Diagnostic Script](#6-verificação-final-com-diagnostic-script)

---

## 1. Instalação Inicial

### Passo 1.1: Baixar o Build #139

```bash
# Clone o repositório
git clone https://github.com/hnanoto/CloverBootloader-Hackintosh-and-Beyond.git
cd CloverBootloader-Hackintosh-and-Beyond
git checkout experimental/opencore-hybrid

# Ou baixe o binário compilado do GitHub Actions
# https://github.com/hnanoto/CloverBootloader-Hackintosh-and-Beyond/actions/runs/21103689308
```

### Passo 1.2: Substituir o CLOVERX64.efi

```bash
# Monte a partição EFI
sudo diskutil mount disk0s1

# Backup do Clover antigo
sudo cp /Volumes/EFI/EFI/CLOVER/CLOVERX64.efi /Volumes/EFI/EFI/CLOVER/CLOVERX64.efi.backup

# Copie o novo CLOVERX64.efi (Build #139)
sudo cp CloverV2/EFI/BOOT/BOOTX64.efi /Volumes/EFI/EFI/CLOVER/CLOVERX64.efi
```

### Passo 1.3: Habilitar Debug no config.plist

Edite `/Volumes/EFI/EFI/CLOVER/config.plist`:

```xml
<key>Boot</key>
<dict>
    <key>Debug</key>
    <true/>
    <key>Arguments</key>
    <string>debug=0x100 -v</string>
    <!-- Outros argumentos... -->
</dict>
```

**💡 Dica:** Use Xcode, PlistEdit Pro, ou ProperTree para editar.

---

## 2. Primeiro Boot - Verificação

### Passo 2.1: Reinicie o Sistema

Após reiniciar, você verá:

```
┌─────────────────────────────────────────────────────────┐
│                                                         │
│              🍀 Clover Boot Manager                     │
│                                                         │
│   ┌───────────────────────────────────────────┐        │
│   │  Boot macOS from Macintosh HD             │        │
│   │  Boot macOS Recovery                      │        │
│   │  Boot Windows from BOOTCAMP               │        │
│   └───────────────────────────────────────────┘        │
│                                                         │
│   Press F2 for Options                                 │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

**🎨 Nota:** O Dark Theme estará ativo (fundo escuro).

### Passo 2.2: Verificar Logs Durante o Boot

Se você habilitou `-v` (verbose mode), verá na tela:

```
[Clover] Clover r5160 starting...
[Clover] Loading OpenRuntime.efi... OK
[Clover] Dark theme loaded successfully
[Clover] Scanning for bootable volumes...

HardwareValidator: Starting hardware validation...
HardwareValidator: Boot fail count = 0
HardwareValidator: Checking Intel Wi-Fi...
HardwareValidator: [OK] Intel Wi-Fi detected and driver present.
HardwareValidator: Checking Intel Ethernet...
HardwareValidator: [OK] Intel I225-V detected with correct boot-arg.
HardwareValidator: Checking AMD GPU...
HardwareValidator: [OK] AMD GPU detected and WhateverGreen.kext present.
HardwareValidator: Checking Bluetooth...
HardwareValidator: [INFO] IntelBluetoothFirmware.kext not loaded.
HardwareValidator: Checking NVMe...
HardwareValidator: [OK] NVMeFix.kext present for NVMe power management.
HardwareValidator: Hardware validation complete. 0 warnings.

[Clover] Proceeding to main menu...
```

---

## 3. Testando o Hardware Validator

### Passo 3.1: Verificar o Debug Log

Após o boot, no macOS:

```bash
# Monte a EFI (se não estiver montada)
sudo diskutil mount disk0s1

# Veja o log completo
cat /Volumes/EFI/EFI/CLOVER/misc/debug.log
```

**Exemplo de saída:**

```
0:000  0:000  MemLog inited, TSC freq: 2400000000
0:000  0:000  
0:000  0:000  Now is 17.1.2026,  23:30:00 (GMT)
0:000  0:000  Starting Clover revision: 5160 on American Megatrends EFI
0:100  0:100  SelfDevicePath=PciRoot(0x0)\Pci(0x1F,0x2)\Sata(0x0,0xFFFF,0x0) @DFC8E918
0:100  0:000  SelfDirPath = \EFI\CLOVER
...
1:234  0:045  HardwareValidator: Starting hardware validation...
1:234  0:000  HardwareValidator: Boot fail count = 0
1:235  0:001  HardwareValidator: Checking Intel Wi-Fi...
1:236  0:001  HardwareValidator: Found Intel Wi-Fi device (VendorID: 0x8086, DeviceID: 0x2723)
1:236  0:000  HardwareValidator: [OK] Intel Wi-Fi detected and driver present.
1:237  0:001  HardwareValidator: Checking Intel Ethernet...
1:238  0:001  HardwareValidator: Found Intel I225-V (DeviceID: 0x15F3)
1:238  0:000  HardwareValidator: Boot-arg 'e1000=0' is present
1:238  0:000  HardwareValidator: [OK] Intel I225-V detected with correct boot-arg.
1:239  0:001  HardwareValidator: Checking AMD GPU...
1:240  0:001  HardwareValidator: Found AMD GPU (VendorID: 0x1002, DeviceID: 0x73BF)
1:240  0:000  HardwareValidator: [OK] AMD GPU detected and WhateverGreen.kext present.
1:241  0:001  HardwareValidator: Hardware validation complete. 0 warnings.
```

### Passo 3.2: Procurar por Avisos (Warnings)

Se algum hardware não tiver o driver necessário:

```bash
cat /Volumes/EFI/EFI/CLOVER/misc/debug.log | grep WARNING
```

**Exemplo de saída com avisos:**

```
1:236  0:000  HardwareValidator: [WARNING] Intel Wi-Fi: Missing itlwm/AirportItlwm kext
1:238  0:000  HardwareValidator: [WARNING] Intel I225-V: Missing boot-arg 'e1000=0' (required for macOS 12+)
1:240  0:000  HardwareValidator: [WARNING] AMD GPU: Missing WhateverGreen.kext (graphics may fail)
```

---

## 4. Testando o Self-Healing / Safe Mode

### Passo 4.1: Verificar o Contador Atual

```bash
nvram -p | grep CloverBootFailCount
```

**Saída esperada:**
- Se vazio = Contador em 0 (normal)
- Se retornar algo = `CloverBootFailCount	2` (exemplo)

### Passo 4.2: Simular Falhas de Boot

Para testar o Safe Mode, vamos simular 3 falhas:

```bash
# Incrementar o contador para 3
sudo nvram CloverBootFailCount=3

# Verificar
nvram -p | grep CloverBootFailCount
```

**Saída:**
```
CloverBootFailCount	3
```

### Passo 4.3: Reiniciar e Ver Safe Mode Ativar

Reinicie o sistema. No boot, você verá:

```
HardwareValidator: Starting hardware validation...
HardwareValidator: Boot fail count = 3
HardwareValidator: [SAFE MODE] Detected 3 consecutive boot failures!
HardwareValidator: [SAFE MODE] Applying safe boot settings...
HardwareValidator: [SAFE MODE] Enabled verbose mode (-v)
HardwareValidator: [SAFE MODE] Disabled GPU injection
HardwareValidator: [SAFE MODE] Disabled custom DSDT
HardwareValidator: [SAFE MODE] Enabled macOS safe boot (-x)
HardwareValidator: [SAFE MODE] Safe mode settings applied. System will boot with minimal configuration.
HardwareValidator: [SAFE MODE] To exit safe mode, delete NVRAM variable: nvram -d CloverBootFailCount
```

**🔴 IMPORTANTE:** O sistema agora está em Safe Mode! Boot-args foram modificados automaticamente.

### Passo 4.4: Resetar o Contador Manualmente

Após verificar que o Safe Mode funcionou:

```bash
# Resetar o contador
sudo nvram -d CloverBootFailCount

# Verificar
nvram -p | grep CloverBootFailCount
# (não deve retornar nada)

# Reiniciar normalmente
sudo reboot
```

---

## 5. Instalando o Auto-Reset

### Passo 5.1: Executar o Instalador

```bash
cd /Users/henrique/Documents/CloverBootloader-Hackintosh-and-Beyond/Scripts
sudo bash install-bootfail-reset.sh
```

**Saída esperada:**

```
=== Clover Boot Failure Counter Auto-Reset Installer ===

Installing script to /usr/local/bin/CloverBootFailReset.sh...
Installing LaunchDaemon to /Library/LaunchDaemons/com.clover.bootfailreset.plist...
Loading LaunchDaemon...

=== Installation Complete ===

✓ LaunchDaemon is loaded and active

Files installed:
  - /usr/local/bin/CloverBootFailReset.sh
  - /Library/LaunchDaemons/com.clover.bootfailreset.plist

The boot failure counter will be automatically reset on each successful boot.

To verify: sudo launchctl list | grep clover
To view logs: cat /var/log/clover-bootfail-reset.log
```

### Passo 5.2: Verificar Instalação

```bash
# Verificar se o daemon está rodando
sudo launchctl list | grep clover
```

**Saída esperada:**
```
-	0	com.clover.bootfailreset
```

### Passo 5.3: Testar o Auto-Reset

```bash
# 1. Criar um contador de teste
sudo nvram CloverBootFailCount=2

# 2. Verificar
nvram -p | grep CloverBootFailCount
# Saída: CloverBootFailCount	2

# 3. Executar o script manualmente
sudo /usr/local/bin/CloverBootFailReset.sh

# 4. Ver os logs
cat /var/log/clover-bootfail-reset.log
```

**Saída do log:**

```
[2026-01-17 23:35:00] === Clover Boot Failure Counter Reset Script Started ===
[2026-01-17 23:35:00] INFO: Current CloverBootFailCount = 2
[2026-01-17 23:35:00] SUCCESS: CloverBootFailCount has been reset.
[2026-01-17 23:35:00] === Script Completed Successfully ===
```

```bash
# 5. Verificar que foi resetado
nvram -p | grep CloverBootFailCount
# (não deve retornar nada)
```

---

## 6. Verificação Final com Diagnostic Script

### Passo 6.1: Executar o Script de Diagnóstico

```bash
cd /Users/henrique/Documents/CloverBootloader-Hackintosh-and-Beyond/Scripts
sudo bash clover-diagnostic.sh
```

### Passo 6.2: Interpretar os Resultados

**✅ EXEMPLO DE SUCESSO TOTAL:**

```
╔════════════════════════════════════════════════════════════╗
║   Clover Híbrido Inteligente - Diagnostic Tool            ║
║   Build #139 - Hardware Validator & Self-Healing          ║
╚════════════════════════════════════════════════════════════╝

[1/5] Checking Boot Failure Counter...
✓ Boot fail counter: 0 (normal)

[2/5] Checking Auto-Reset LaunchDaemon...
✓ LaunchDaemon plist installed
✓ LaunchDaemon is loaded and active
✓ Reset script installed

[3/5] Checking Auto-Reset Logs...
✓ Reset log file exists

   Last 5 entries:
   [2026-01-17 23:00:15] === Clover Boot Failure Counter Reset Script Started ===
   [2026-01-17 23:00:15] INFO: Current CloverBootFailCount = 1
   [2026-01-17 23:00:15] SUCCESS: CloverBootFailCount has been reset.
   [2026-01-17 23:00:15] === Script Completed Successfully ===

[4/5] Checking Clover Debug Logs...
✓ Debug log found at: /Volumes/EFI/EFI/CLOVER/misc/debug.log
✓ Hardware Validator is active (found in logs)

   Recent Hardware Validator messages:
   1:234  0:045  HardwareValidator: Starting hardware validation...
   1:234  0:000  HardwareValidator: Boot fail count = 0
   1:236  0:001  HardwareValidator: [OK] Intel Wi-Fi detected and driver present.
   1:238  0:000  HardwareValidator: [OK] Intel I225-V detected with correct boot-arg.
   1:240  0:000  HardwareValidator: [OK] AMD GPU detected and WhateverGreen.kext present.
   1:241  0:001  HardwareValidator: Hardware validation complete. 0 warnings.

[5/5] Checking config.plist Debug Settings...
✓ Debug logging is enabled in config.plist

╔════════════════════════════════════════════════════════════╗
║   Diagnostic Summary                                       ║
╚════════════════════════════════════════════════════════════╝

✓ All systems operational!

Your Clover Híbrido Inteligente is working correctly.

For more information, see: VERIFICATION_GUIDE.md
```

**⚠️ EXEMPLO COM AVISOS:**

```
╔════════════════════════════════════════════════════════════╗
║   Clover Híbrido Inteligente - Diagnostic Tool            ║
║   Build #139 - Hardware Validator & Self-Healing          ║
╚════════════════════════════════════════════════════════════╝

[1/5] Checking Boot Failure Counter...
⚠ Boot fail counter: 3 (Safe Mode will activate!)

[2/5] Checking Auto-Reset LaunchDaemon...
✓ LaunchDaemon plist installed
⚠ LaunchDaemon plist exists but not loaded
   Run: sudo launchctl load /Library/LaunchDaemons/com.clover.bootfailreset.plist
✓ Reset script installed

[3/5] Checking Auto-Reset Logs...
ℹ No reset logs yet (daemon hasn't run)

[4/5] Checking Clover Debug Logs...
✓ Debug log found at: /Volumes/EFI/EFI/CLOVER/misc/debug.log
⚠ No HardwareValidator messages found in debug.log
   This might mean:
   - Debug logging is not enabled in config.plist
   - You're not using the Build #139 CLOVERX64.efi

[5/5] Checking config.plist Debug Settings...
⚠ Debug logging is disabled in config.plist
   Enable it to see Hardware Validator messages

╔════════════════════════════════════════════════════════════╗
║   Diagnostic Summary                                       ║
╚════════════════════════════════════════════════════════════╝

⚠ WARNING: Boot failure counter is at 3 (Safe Mode active)
⚠ WARNING: Auto-Reset LaunchDaemon is not running
⚠ WARNING: Hardware Validator messages not found in logs

Found 3 potential issue(s).

Review the warnings above and check VERIFICATION_GUIDE.md for solutions.

For more information, see: VERIFICATION_GUIDE.md
```

---

## 🎯 Checklist Final

Use este checklist para confirmar que tudo está funcionando:

### Hardware Validator
- [ ] Mensagens `HardwareValidator:` aparecem no `debug.log`
- [ ] Hardware detectado mostra `[OK]` nos logs
- [ ] Avisos `[WARNING]` aparecem para hardware sem driver

### Self-Healing
- [ ] Contador incrementa a cada boot (verificar com `nvram`)
- [ ] Safe Mode ativa quando contador >= 3
- [ ] Mensagens `[SAFE MODE]` aparecem nos logs
- [ ] Boot-args são modificados (`-v`, `-x`)

### Auto-Reset
- [ ] LaunchDaemon aparece em `launchctl list`
- [ ] Contador é resetado após boot bem-sucedido
- [ ] Logs em `/var/log/clover-bootfail-reset.log`

### Geral
- [ ] Dark Theme está ativo
- [ ] OpenRuntime carrega sem erros
- [ ] Script de diagnóstico mostra "All systems operational"

---

## 🆘 Troubleshooting Rápido

| Problema | Solução |
|----------|---------|
| Não vejo mensagens do HardwareValidator | Habilite debug no `config.plist` |
| Safe Mode não ativa | Verifique o contador: `nvram -p \| grep Clover` |
| LaunchDaemon não funciona | Execute: `sudo bash install-bootfail-reset.sh` |
| Debug.log vazio | Adicione `<key>Debug</key><true/>` no config.plist |

---

## 📚 Recursos Adicionais

- **Guia Completo:** `VERIFICATION_GUIDE.md`
- **Script de Diagnóstico:** `Scripts/clover-diagnostic.sh`
- **Instalador Auto-Reset:** `Scripts/install-bootfail-reset.sh`
- **Repositório:** https://github.com/hnanoto/CloverBootloader-Hackintosh-and-Beyond

---

## 🎬 Próximos Passos

1. ✅ Instale o Build #139
2. ✅ Execute o diagnostic script
3. ✅ Instale o auto-reset daemon
4. ✅ Teste o Safe Mode (opcional)
5. 🎉 Aproveite seu Clover Híbrido Inteligente!

---

**Última atualização:** Build #139 (17/01/2026)
**Autor:** CloverHackyColor Hybrid Team
