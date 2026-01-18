# Clover Híbrido Inteligente - Guia de Verificação

## Como Saber se Tudo Está Funcionando?

Este guia mostra como verificar se o Hardware Validator, Self-Healing e todas as funcionalidades estão ativas.

---

## 📋 Método 1: Logs de Debug (RECOMENDADO)

### Passo 1: Habilitar Debug Logging no config.plist

Edite seu `config.plist` e adicione/modifique:

```xml
<key>Boot</key>
<dict>
    <key>Debug</key>
    <true/>
    <key>Arguments</key>
    <string>debug=0x100 -v</string>
</dict>
```

### Passo 2: Reiniciar e Coletar Logs

Após reiniciar, os logs estarão em:
- **Clover:** `/EFI/CLOVER/misc/debug.log`
- **macOS:** `/var/log/system.log`

### Passo 3: Procurar por Mensagens do Hardware Validator

Abra o `debug.log` e procure por:

```
HardwareValidator: [INFO] Starting hardware validation...
HardwareValidator: [OK] Intel Wi-Fi detected and driver present.
HardwareValidator: [WARNING] Intel I225-V: Missing boot-arg 'e1000=0'
HardwareValidator: [OK] AMD GPU detected and WhateverGreen.kext present.
```

**Mensagens Importantes:**
- `[OK]` = Hardware detectado e driver presente ✅
- `[WARNING]` = Hardware detectado mas falta driver/boot-arg ⚠️
- `[INFO]` = Informação geral 📝

---

## 🛡️ Método 2: Verificar Self-Healing / Safe Mode

### Como Testar o Safe Mode:

1. **Simular Falhas de Boot:**
   ```bash
   # Incrementar manualmente o contador (para teste)
   sudo nvram CloverBootFailCount=3
   ```

2. **Reiniciar o Sistema**

3. **Verificar nos Logs:**
   ```
   HardwareValidator: [SAFE MODE] Detected 3 consecutive boot failures!
   HardwareValidator: [SAFE MODE] Applying safe boot settings...
   HardwareValidator: [SAFE MODE] Enabled verbose mode (-v)
   HardwareValidator: [SAFE MODE] Disabled GPU injection
   HardwareValidator: [SAFE MODE] Disabled custom DSDT
   HardwareValidator: [SAFE MODE] Enabled macOS safe boot (-x)
   ```

4. **Resetar o Contador:**
   ```bash
   sudo nvram -d CloverBootFailCount
   ```

### Verificar o Contador Atual:
```bash
nvram -p | grep CloverBootFailCount
```

Se retornar vazio = contador em 0 (normal)
Se retornar um número = contador ativo

---

## 📊 Método 3: Verificar Avisos (GUI Warnings)

**NOTA:** A exibição visual dos avisos no menu do Clover ainda não foi implementada. 
Atualmente, os avisos são coletados e podem ser acessados via logs.

Para ver os avisos coletados, procure no `debug.log`:

```
HardwareValidator: [WARNING] Intel Wi-Fi: Missing itlwm/AirportItlwm kext
HardwareValidator: [WARNING] Intel I225-V: Missing boot-arg 'e1000=0' (required for macOS 12+)
HardwareValidator: [WARNING] AMD GPU: Missing WhateverGreen.kext (graphics may fail)
```

---

## 🔄 Método 4: Verificar Auto-Reset Daemon (macOS)

### Verificar se o LaunchDaemon está instalado:

```bash
# Verificar se está carregado
sudo launchctl list | grep clover

# Deve retornar:
# -    0    com.clover.bootfailreset
```

### Verificar os Logs do Daemon:

```bash
# Log principal
cat /var/log/clover-bootfail-reset.log

# Deve mostrar:
# [2026-01-17 23:00:00] === Clover Boot Failure Counter Reset Script Started ===
# [2026-01-17 23:00:00] INFO: Current CloverBootFailCount = 1
# [2026-01-17 23:00:00] SUCCESS: CloverBootFailCount has been reset.
```

### Testar Manualmente:

```bash
# 1. Criar um contador de teste
sudo nvram CloverBootFailCount=2

# 2. Verificar
nvram -p | grep CloverBootFailCount
# Deve retornar: CloverBootFailCount    2

# 3. Executar o script manualmente
sudo /usr/local/bin/CloverBootFailReset.sh

# 4. Verificar novamente
nvram -p | grep CloverBootFailCount
# Não deve retornar nada (contador resetado)
```

---

## 🎯 Checklist Completo de Verificação

### ✅ Hardware Validator Ativo:
- [ ] Logs mostram `HardwareValidator:` durante o boot
- [ ] Hardware detectado aparece nos logs
- [ ] Avisos de kexts faltando são exibidos (se aplicável)

### ✅ Self-Healing Funcionando:
- [ ] Contador incrementa a cada boot (verificar NVRAM)
- [ ] Safe Mode ativa após 3 falhas
- [ ] Logs mostram `[SAFE MODE]` quando ativado
- [ ] Boot-args são modificados (`-v`, `-x`)

### ✅ Auto-Reset Instalado:
- [ ] LaunchDaemon aparece em `launchctl list`
- [ ] Contador é resetado após boot bem-sucedido
- [ ] Logs em `/var/log/clover-bootfail-reset.log`

### ✅ Detecções de Hardware:
- [ ] Wi-Fi Intel detectado
- [ ] Ethernet (I225-V ou RTL8125) detectado
- [ ] GPU AMD detectado
- [ ] Bluetooth detectado (se aplicável)
- [ ] NVMe detectado (se aplicável)

---

## 🐛 Troubleshooting

### "Não vejo mensagens do HardwareValidator nos logs"

**Solução:**
1. Verifique se o debug está habilitado no `config.plist`
2. Certifique-se de estar usando o CLOVERX64.efi compilado da Build #139
3. Verifique se o arquivo `debug.log` está sendo criado em `/EFI/CLOVER/misc/`

### "Safe Mode não ativa mesmo com contador em 3"

**Solução:**
1. Verifique o valor exato do contador:
   ```bash
   nvram -p | grep CloverBootFailCount
   ```
2. Certifique-se de que o valor é >= 3
3. Verifique os logs para mensagens de erro

### "Auto-Reset não funciona"

**Solução:**
1. Verifique permissões:
   ```bash
   ls -l /usr/local/bin/CloverBootFailReset.sh
   # Deve ser: -rwxr-xr-x root wheel
   ```
2. Teste manualmente o script
3. Verifique logs de erro em `/var/log/clover-bootfailreset.stderr.log`

---

## 📝 Exemplo de Log Completo (Sucesso)

```
[Boot] Clover r5160 starting...
[Boot] Loading OpenRuntime.efi...
[Boot] Dark theme loaded successfully
HardwareValidator: Starting hardware validation...
HardwareValidator: Boot fail count = 0
HardwareValidator: [OK] Intel Wi-Fi detected and driver present.
HardwareValidator: [OK] Intel I225-V detected with correct boot-arg.
HardwareValidator: [OK] AMD GPU detected and WhateverGreen.kext present.
HardwareValidator: [INFO] IntelBluetoothFirmware.kext not loaded.
HardwareValidator: [OK] NVMeFix.kext present for NVMe power management.
HardwareValidator: Hardware validation complete. 0 warnings.
[Boot] Proceeding to main menu...
```

---

## 🎓 Dicas Avançadas

### Monitorar em Tempo Real (durante o boot):

1. Habilite verbose mode (`-v` nos boot-args)
2. Durante o boot, você verá as mensagens do Clover na tela
3. Procure por linhas começando com `HardwareValidator:`

### Criar um Script de Diagnóstico:

```bash
#!/bin/bash
echo "=== Clover Híbrido - Diagnóstico ==="
echo ""
echo "1. Boot Fail Counter:"
nvram -p | grep CloverBootFailCount || echo "  ✓ Counter at 0 (normal)"
echo ""
echo "2. LaunchDaemon Status:"
sudo launchctl list | grep clover || echo "  ✗ Not installed"
echo ""
echo "3. Recent Reset Logs:"
tail -5 /var/log/clover-bootfail-reset.log 2>/dev/null || echo "  No logs found"
echo ""
echo "4. Clover Debug Log (last 20 lines):"
tail -20 /Volumes/EFI/EFI/CLOVER/misc/debug.log 2>/dev/null || echo "  Debug log not found"
```

Salve como `clover-diagnostic.sh` e execute:
```bash
chmod +x clover-diagnostic.sh
sudo ./clover-diagnostic.sh
```

---

## 📞 Suporte

Se após seguir este guia você ainda tiver problemas:

1. Colete o `debug.log` completo
2. Execute o script de diagnóstico
3. Anote qual hardware você tem
4. Abra uma issue no repositório com essas informações

---

**Última atualização:** Build #139 (17/01/2026)
