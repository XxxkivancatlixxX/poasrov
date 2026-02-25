# QEMU Testing - Quick Reference

## One-Line Startup (Fully Automated)
```bash
cd /home/vujuvuju/rov/PCside && ./test_qemu_full.sh
```
Then run GUI in another terminal, connect to `localhost:5760`

---

## Three-Terminal Manual Setup

### Terminal 1: QEMU
```bash
cd /home/vujuvuju/rov/PCside/firmware && ./run_qemu.sh
```

### Terminal 2: Telemetry Bridge  
```bash
python3 /home/vujuvuju/rov/PCside/telemetry_bridge.py --stdin 5760
```

### Terminal 3: GUI
```bash
cd /home/vujuvuju/rov/PCside && ./gui localhost 5760
```

---

## Verify Components

| Component | Command | Expected |
|-----------|---------|----------|
| **QEMU** | `which qemu-system-arm` | `/usr/bin/qemu-system-arm` |
| **Firmware** | `ls firmware/build/src/firmware` | `firmware` (executable) |
| **Python** | `python3 --version` | `Python 3.x.x` |
| **Bridge port** | `netstat -ln \| grep 5760` | `LISTEN` |

---

## Troubleshooting Checklist

- [ ] QEMU installed? → `sudo pacman -S qemu-system-arm`
- [ ] Firmware compiled? → `cd firmware/build && make`
- [ ] Bridge running? → Check Terminal 2 output
- [ ] Port 5760 free? → `lsof -i :5760` should be empty
- [ ] GUI can reach bridge? → `telnet localhost 5760` (Ctrl+])

---

## What You Should See

### QEMU Terminal
```
[2/3] Starting QEMU emulation...
Output:
---
<binary telemetry packets>
```

### Bridge Terminal  
```
[HH:MM:SS] TCP server listening on port 5760
[HH:MM:SS] Packets: 100 | Clients: 0
[HH:MM:SS] Packets: 200 | Clients: 1  ← GUI connected!
```

### GUI
- All graphs animating (depth, temperature, battery)
- Motor values 1000-2000µs range
- 10 tabs responding to clicks

---

## Test Motor Commands

1. **GUI → Motor Config tab**
2. **Slide Motor 1 to 50%**
3. **Watch QEMU output**
4. **Check PWM values changing**

---

## Kill Everything
```bash
pkill -f qemu-system-arm
pkill -f telemetry_bridge.py
pkill -f "gui localhost"
```

---

## File Locations

```
/home/vujuvuju/rov/PCside/
├── firmware/
│   ├── build/src/firmware         ← Compiled binary
│   ├── run_qemu.sh               ← QEMU launcher
│   ├── QEMU_TESTING.md           ← Detailed guide
│   └── flash_pixhawk.sh          ← For real hardware
├── telemetry_bridge.py           ← TCP bridge
├── QEMU_MANUAL_TEST.md           ← Step-by-step guide
├── test_qemu_full.sh             ← Auto launcher
└── gui                           ← GUI binary (if built)
```

---

## Network Diagram

```
QEMU (serial output)
  │ 57600 baud
  ↓
[stdin]
  │
  ↓
Telemetry Bridge (Python)
  │ TCP :5760
  ↓
GUI Client (SDL2/ImGui)
  │ TCP :5760
  ↓
Telemetry Parser + Display
```

---

## Performance Notes

- **Telemetry rate**: 50-100 packets/sec (configurable in firmware)
- **Packet size**: 512 bytes each
- **Network throughput**: ~25-50 KB/sec (well below TCP limits)
- **GUI FPS**: 30-60 FPS (depends on ImGui rendering)
- **CPU usage**: ~20-30% per component on 4-core CPU

---

## Next Steps

1. ✅ QEMU test (this document)
2. Real hardware on Pixhawk 2.4.8
3. Pi 5 TCP bridge (use same Python script)
4. GUI on PC connecting to Pi
5. Motor spin test with real ESCs

See [FLASH_PIXHAWK.md](firmware/FLASH_PIXHAWK.md) for hardware flashing.
