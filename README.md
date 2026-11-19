# npacket

`npacket` ist ein Netzwerk-Paketsniffer in **C für Linux**. Das Programm erfasst Netzwerkpakete mit **libpcap**, liest ausgewählte Header-Felder selbst aus und zeigt die Ergebnisse in einer **ncurses-Terminaloberfläche** an.

Das Projekt ist ein persönliches Lernprojekt und befindet sich in Entwicklung.

## Aktueller Funktionsumfang

- Live-Paketerfassung über ein auswählbares Netzwerkinterface
- Ethernet-Auswertung: Quell- und Ziel-MAC-Adresse, EtherType und Frame-Länge
- IPv4-Auswertung: Gesamtlänge, TTL, Protokollnummer sowie Quell- und Ziel-IP-Adresse
- Erkennung von TCP, UDP, ICMP und ARP (noch keine vollständige Header-Analyse dieser Protokolle)
- Anzeige von Frame-Nummer und relativer Aufnahmezeit
- Fortlaufende, teilweise farbige Terminalausgabe der letzten bis zu 50 Frames
- Dynamische Speicherung erfasster Frame-Daten

## Voraussetzungen

- Linux (z. B. Ubuntu oder Debian)
- C-Compiler und CMake 3.20+
- libpcap und ncurses
- Berechtigung zum Mitschneiden von Netzwerkverkehr

Unter Ubuntu/Debian die benötigten Pakete installieren:

```bash
sudo apt update
sudo apt install git build-essential cmake libpcap-dev libncurses-dev libcap2-bin
```

## Repository klonen und kompilieren

```bash
git clone --branch newView https://github.com/neulii/npacket.git
cd npacket
cmake -S . -B build
cmake --build build
```

**Hinweis:** Die aktuelle CMake-Konfiguration setzt nach dem Build mit `sudo -n setcap` die Linux-Capabilities `cap_net_raw` und `cap_net_admin` auf die ausführbare Datei. Dafür muss der ausführende Benutzer die entsprechende `sudo`-Berechtigung besitzen.

## Programm starten

```bash
./build/npacket <interface>
```

Beispiel:

```bash
./build/npacket enp3s0
```

Ohne Interface-Argument listet das Programm die gefundenen Netzwerkinterfaces auf und verwendet derzeit automatisch das erste Interface.

Mit **`q`** wird das Programm beendet.

## Lizenz

Dieses Projekt ist unter der **PolyForm Noncommercial License 1.0.0** lizenziert. Nichtkommerzielle Nutzung, Änderungen und Weitergabe sind im Rahmen der Lizenz erlaubt. Siehe [LICENSE](LICENSE).
