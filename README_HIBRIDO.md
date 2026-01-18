# 🍀 Clover Híbrido Inteligente

## Hardware Validator + Self-Healing + OpenRuntime Integration

[![Build Status](https://github.com/hnanoto/CloverBootloader-Hackintosh-and-Beyond/actions/workflows/main.yml/badge.svg)](https://github.com/hnanoto/CloverBootloader-Hackintosh-and-Beyond/actions)
[![Latest Release](https://img.shields.io/badge/Build-139-success)](https://github.com/hnanoto/CloverBootloader-Hackintosh-and-Beyond/actions/runs/21103689308)
[![License](https://img.shields.io/badge/License-BSD--2--Clause-blue.svg)](LICENSE)

---

## 🎯 O Que É?

O **Clover Híbrido Inteligente** é uma versão avançada do Clover Bootloader que combina:

- ✅ **Hardware Validator** - Detecta hardware e valida se os drivers necessários estão presentes
- ✅ **Self-Healing** - Sistema automático de recuperação que entra em Safe Mode após falhas consecutivas
- ✅ **Auto-Reset** - Daemon do macOS que reseta o contador de falhas após boot bem-sucedido
- ✅ **OpenRuntime** - Integração com OpenCore para melhor compatibilidade
- ✅ **Dark Theme** - Interface moderna e elegante

---

## 🚀 Funcionalidades

### 1️⃣ Hardware Validator (Detecção Inteligente)

Detecta automaticamente e valida 6 tipos de hardware:

| Hardware | Driver/Config Necessário | Status |
|----------|--------------------------|--------|
| **Wi-Fi Intel** | itlwm.kext ou AirportItlwm.kext | ✅ |
| **Ethernet Intel I225-V** | Boot-arg `e1000=0` | ✅ |
| **Ethernet Realtek RTL8125** | LucyRTL8125Ethernet.kext | ✅ |
| **GPU AMD** | WhateverGreen.kext | ✅ |
| **Bluetooth Intel** | IntelBluetoothFirmware.kext | ✅ |
| **Bluetooth Broadcom** | BrcmPatchRAM.kext | ✅ |
| **NVMe (não-Apple)** | NVMeFix.kext | ✅ |

**Exemplo de saída:**
```
HardwareValidator: [OK] Intel Wi-Fi detected and driver present.
HardwareValidator: [WARNING] Intel I225-V: Missing boot-arg 'e1000=0'
HardwareValidator: [OK] AMD GPU detected and WhateverGreen.kext present.
```

### 2️⃣ Self-Healing (Recuperação Automática)

Sistema inteligente que:

1. **Incrementa contador** a cada tentativa de boot
2. **Detecta falhas consecutivas** (threshold: 3 falhas)
3. **Ativa Safe Mode automaticamente** com:
   - Verbose mode (`-v`)
   - Safe boot (`-x`)
   - GPU injection desabilitado
   - DSDT customizado desabilitado

**Exemplo de Safe Mode:**
```
HardwareValidator: [SAFE MODE] Detected 3 consecutive boot failures!
HardwareValidator: [SAFE MODE] Applying safe boot settings...
HardwareValidator: [SAFE MODE] System will boot with minimal configuration.
```

### 3️⃣ Auto-Reset Daemon

LaunchDaemon para macOS que:

- ✅ Reseta o contador automaticamente após boot bem-sucedido
- ✅ Previne falsos positivos do Safe Mode
- ✅ Logs detalhados em `/var/log/clover-bootfail-reset.log`

---

## 📦 Instalação

### Método 1: Binário Pré-Compilado (Recomendado)

1. **Baixe o Build #139:**
   - [GitHub Actions - Build #139](https://github.com/hnanoto/CloverBootloader-Hackintosh-and-Beyond/actions/runs/21103689308)
   - Baixe o artefato `Clover-*.zip`

2. **Substitua o CLOVERX64.efi:**
   ```bash
   # Monte a EFI
   sudo diskutil mount disk0s1
   
   # Backup do antigo
   sudo cp /Volumes/EFI/EFI/CLOVER/CLOVERX64.efi /Volumes/EFI/EFI/CLOVER/CLOVERX64.efi.backup
   
   # Copie o novo
   sudo cp CLOVERX64.efi /Volumes/EFI/EFI/CLOVER/
   ```

3. **Habilite Debug no config.plist:**
   ```xml
   <key>Boot</key>
   <dict>
       <key>Debug</key>
       <true/>
   </dict>
   ```

4. **Instale o Auto-Reset (Opcional mas Recomendado):**
   ```bash
   cd Scripts
   sudo bash install-bootfail-reset.sh
   ```

### Método 2: Compilar do Código-Fonte

```bash
# Clone o repositório
git clone https://github.com/hnanoto/CloverBootloader-Hackintosh-and-Beyond.git
cd CloverBootloader-Hackintosh-and-Beyond
git checkout experimental/opencore-hybrid

# Compile
./buildme

# O binário estará em CloverV2/EFI/BOOT/BOOTX64.efi
```

---

## 🔍 Como Verificar se Está Funcionando?

### Método Rápido: Script de Diagnóstico

```bash
cd Scripts
sudo bash clover-diagnostic.sh
```

**Saída esperada:**
```
╔════════════════════════════════════════════════════════════╗
║   Clover Híbrido Inteligente - Diagnostic Tool            ║
╚════════════════════════════════════════════════════════════╝

[1/5] Checking Boot Failure Counter...
✓ Boot fail counter: 0 (normal)

[2/5] Checking Auto-Reset LaunchDaemon...
✓ LaunchDaemon is loaded and active

[4/5] Checking Clover Debug Logs...
✓ Hardware Validator is active (found in logs)

╔════════════════════════════════════════════════════════════╗
║   Diagnostic Summary                                       ║
╚════════════════════════════════════════════════════════════╝

✓ All systems operational!
```

### Verificação Manual

```bash
# 1. Verificar logs do Clover
cat /Volumes/EFI/EFI/CLOVER/misc/debug.log | grep HardwareValidator

# 2. Verificar contador de falhas
nvram -p | grep CloverBootFailCount

# 3. Verificar daemon
sudo launchctl list | grep clover
```

---

## 📚 Documentação

- **[TUTORIAL.md](TUTORIAL.md)** - Tutorial visual completo passo-a-passo
- **[VERIFICATION_GUIDE.md](VERIFICATION_GUIDE.md)** - Guia detalhado de verificação
- **[Scripts/README.md](Scripts/README.md)** - Documentação do Auto-Reset

---

## 🎯 Casos de Uso

### Cenário 1: Detecção de Hardware Faltante

Você instalou o macOS mas esqueceu de adicionar o kext do Wi-Fi:

```
HardwareValidator: [WARNING] Intel Wi-Fi: Missing itlwm/AirportItlwm kext
```

**Ação:** Adicione o kext apropriado em `/EFI/CLOVER/kexts/Other/`

### Cenário 2: Boot Failure Repetido

Você fez uma mudança no config.plist que quebrou o boot. Após 3 tentativas:

```
HardwareValidator: [SAFE MODE] Detected 3 consecutive boot failures!
HardwareValidator: [SAFE MODE] Applying safe boot settings...
```

O sistema entra automaticamente em Safe Mode com configuração mínima, permitindo que você corrija o problema.

### Cenário 3: Monitoramento Contínuo

O Auto-Reset daemon garante que, após cada boot bem-sucedido, o contador é resetado:

```bash
$ cat /var/log/clover-bootfail-reset.log
[2026-01-17 23:00:00] SUCCESS: CloverBootFailCount has been reset.
```

---

## 🛠️ Desenvolvimento

### Estrutura do Projeto

```
CloverBootloader-Hackintosh-and-Beyond/
├── rEFIt_UEFI/
│   ├── Platform/
│   │   ├── HardwareValidator.h      # Header do Hardware Validator
│   │   └── HardwareValidator.cpp    # Implementação
│   └── refit/
│       └── main.cpp                 # Integração no boot loop
├── Scripts/
│   ├── CloverBootFailReset.sh       # Script de reset
│   ├── com.clover.bootfailreset.plist  # LaunchDaemon
│   ├── install-bootfail-reset.sh    # Instalador
│   ├── clover-diagnostic.sh         # Diagnóstico
│   └── README.md                    # Docs do Auto-Reset
├── TUTORIAL.md                      # Tutorial visual
├── VERIFICATION_GUIDE.md            # Guia de verificação
└── README.md                        # Este arquivo
```

### Builds

| Build | Commit | Status | Descrição |
|-------|--------|--------|-----------|
| #139 | 6aab373 | ✅ | **RELEASE** - Step 7 completo (Bluetooth, Thunderbolt, NVMe) |
| #137 | a0f8f52 | ✅ | Step 5 - GUI Warning System |
| #135 | 735cea9 | ✅ | Fix GUID references |
| #134 | 2621545 | ✅ | Phase 4 - Self-Healing completo |

---

## 🤝 Contribuindo

Contribuições são bem-vindas! Por favor:

1. Fork o repositório
2. Crie uma branch para sua feature (`git checkout -b feature/MinhaFeature`)
3. Commit suas mudanças (`git commit -m 'feat: Adiciona MinhaFeature'`)
4. Push para a branch (`git push origin feature/MinhaFeature`)
5. Abra um Pull Request

---

## 📝 Changelog

### Build #139 (17/01/2026) - CURRENT

**Adicionado:**
- ✅ Detecção de Bluetooth (Intel e Broadcom)
- ✅ Detecção de Thunderbolt
- ✅ Detecção de NVMe (com NVMeFix)
- ✅ Tutorial visual completo
- ✅ Script de diagnóstico automático

### Build #137 (17/01/2026)

**Adicionado:**
- ✅ Sistema de GUI Warnings
- ✅ Funções `GetWarnings()`, `ClearWarnings()`, `HasWarnings()`

### Build #135 (17/01/2026)

**Corrigido:**
- 🐛 Referências de GUID em funções NVRAM

### Build #134 (17/01/2026)

**Adicionado:**
- ✅ Self-Healing completo
- ✅ Safe Mode automático
- ✅ Auto-Reset LaunchDaemon

---

## 📄 Licença

Este projeto é licenciado sob a BSD 2-Clause License - veja o arquivo [LICENSE](LICENSE) para detalhes.

---

## 🙏 Créditos

- **Clover Team** - Bootloader original
- **OpenCore Team** - OpenRuntime
- **Acidanthera** - Kexts e ferramentas
- **CloverHackyColor** - Manutenção do Clover

---

## 📞 Suporte

- **Issues:** [GitHub Issues](https://github.com/hnanoto/CloverBootloader-Hackintosh-and-Beyond/issues)
- **Discussões:** [GitHub Discussions](https://github.com/hnanoto/CloverBootloader-Hackintosh-and-Beyond/discussions)
- **Wiki:** [GitHub Wiki](https://github.com/hnanoto/CloverBootloader-Hackintosh-and-Beyond/wiki)

---

## ⭐ Star History

Se este projeto te ajudou, considere dar uma ⭐!

---

**Desenvolvido com ❤️ pela comunidade Hackintosh**
