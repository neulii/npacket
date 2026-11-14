# 📡 npacket – Eigenentwickelter Packet Sniffer in C

> *Selbstgebautes Network Protocol Analyzer Tool für Lernzwecke*

Ein **manuell implementierter** Paket-Analysator, der zeigt, was wirklich in Ethernet-, IPv4- und TCP/IP-Paketen steckt. Kein Blackbox-Magie – alles händisch parsen! 💪

---

## 🎯 Lernziel

Dieses Projekt dient dazu zu verstehen:
- Wie **Ethernet Frames** aufgebaut sind (Header → Payload)
- Wie **IPv4 Header** funktioniert (Version, IHL, Flags, TTL...)
- Wie **TCP Header** strukturiert ist (Flags, Fenstergröße, Checksumme...)
- Wie man **pcaplib** selbstständig nutzt
- Warum **manuelles Parsen** tieferes Verständnis bringt als `Wireshark dissectors`

---

## 🚀 Features

| Feature | Status | Beschreibung |
|---------|--------|--------------|
| Ethernet Header Parsing | ✅ | MAC-Adressen, EtherType (IPv4/IPv6) |
| IPv4 Header Extraction | ✅ | Version, IHL, Flags, TTL, Protocol |
| TCP Header Analysis | ✅ | Source/Dest Port, Sequence/Ack Numbers, Flags |
| Live Capture | ✅ | PCAP Format, Loop-Modus für Endlosschnappen |
| Curses Terminal UI | ✅ | ASCII-Fenster mit Paketausgabe |
| Memory Management | ✅ | `malloc()`/`free()` für Dynamische Allokation |
| Error Handling | ✅ | Buffer-Limits, Header-Validierung (TODO) |

---

## ⚙️ Systemanforderungen

### Benötigte Libraries:
```bash
sudo apt install libpcap-dev libncurses5-dev libncursesw5-dev
# oder bei Ubuntu 22.04+:
sudo apt install libpcap-dev libncurses-dev
```

### Compiler-Anforderungen:
- GCC 7+ (oder Clang)
- C99 Standard
- CMake 3.20+

---

## 📦 Build-Anleitung

### 1. Repository klonen oder Quellcode herunterladen

```bash
cd /home/neulii/npacket
```

### 2. Dependencies installieren

```bash
sudo apt install libpcap-dev libncurses5-dev libncursesw5-dev build-essential
```

### 3. Build Verzeichnis erstellen und konfigurieren

```bash
mkdir -p build && cd build
cmake ..
```

### 4. Kompilieren (mit Compiler-Warnungen aktivieren)

```bash
make
```

**Erwartete Warnung:**  
Der Code enthält absichtlich keine `Werror` Flags, um Lernfehler sichtbar zu machen!

---

## 🎮 Usage

Starte den Sniffer:

```bash
./build/npacket
```

### Tastatur-Steuerung:

| Taste | Funktion |
|-------|----------|
| `q` | Programm beenden (`Ctrl+C` als Backup) |
| `l` | Loop-Modus aktivieren/deaktivieren (Endlosschnappen) |
| `?` | Hilfe anzeigen |

### Beispiel-Ausgabe:

```
=== npacket Packet Sniffer v1.0 ===
  [Loop Mode] [Live Capture] [Ethernet/IP/TCP Parsing]

Frame #0001 || 
  Ethernet Header: dst=aa:bb:cc:dd:ee:ff src=11:22:33:44:55:66 type=0x0800 (IPv4)
  
  IPv4 Header: version=4 ihl=5 total_len=60 flags=0 ttl=64 protocol=6 (TCP)
  
  TCP Header: src_port=12345 dst_port=80 seq=123456789 ack=0 data_offset=5 
              fin=0 syn=1 rst=0 psh=1 ack=0 urg=0 window=65535 checksum=valid
```

---

## 📁 Projektstruktur

```
npacket/
├── CMakeLists.txt          # Build-Konfiguration (CMake)
├── README.md               # Diese Datei
├── src/
│   ├── npacket.c           # Hauptprogramm + pcap Integration
│   └── parsingPrinting.c    # Manuelle Header-Parsing-Logik
├── info/                    # Paketausgabe & Debugging
├── debug/                   # Test-Daten (falls vorhanden)
├── tags                     # Tag-Historie für Versionskontrolle
└── .vscode/                 # IDE-Einstellungen
```

---

## ⚠️ TODO / Offene Fragen

- [ ] **Sichere String-Funktionen** (`snprintf` statt `sprintf`)
- [ ] **IPv6 Support** (aktuell nur IPv4)
- [ ] **TCP Payload Dekodierung** (HTTP/FTP/TLS...?)
- [ ] **VLAN Tag Erkennung** (IEEE 802.1Q für EtherType 0x8100)
- [ ] **BPF Filter Integration** (`tcp port 80 and ip src 192.168.1.1`)

---

## 🛠️ Technischer Stack

| Komponente | Technologie |
|------------|-------------|
| Sprache | C (C99) |
| Capture Library | libpcap / libpcap-ng |
| Terminal UI | ncurses |
| Build System | CMake |
| Memory Model | malloc/free (kein `new/delete`) |

---

## 📝 Lizenz & Urheberrecht

Dieses Projekt steht unter **MIT License**.  
Du darfst es kostenlos nutzen, modifizieren und verteilen – auch kommerziell.

⚠️ **Wichtig:** Dies ist ein Lernprojekt mit experimentellen Features. Keine kommerzielle Nutzung ohne vorherige Absprache!
## 📝 Lizenzen & Credits

Dieses Projekt wurde **für Lernzwecke** entwickelt.  
Kein kommerzieller Zweck – alle Rechte vorbehalten für den Autor.

Inspiration von [Wireshark](https://www.wireshark.org/) und [tcpdump](https://www.tcpdump.org/).

---

## 📮 Kontakt / Issues?

Falls du Fragen hast oder Fehler gefunden:  
→ Schreibe mir einfach! 💬

