# Smart Voting System

> A hybrid electronic voting system built with Arduino Mega 2560, Python, SQLite, and Wokwi for secure voter verification, vote casting, vote counting, and result management.

## About the Project

The **Smart Voting System** is an academic embedded-systems project that demonstrates how a microcontroller-based voting machine can be integrated with a software database.

The system uses an **Arduino Mega 2560** as the main controller. Voters enter their voter ID through a **4×4 keypad**, which is verified using a **Python and SQLite-based backend**. Once verified, the voter can cast a vote using dedicated buttons for three candidates or **NOTA (None of the Above)**.

Vote counts are stored using the Arduino's **EEPROM**, while the SQLite database maintains voter registration and voting status to prevent duplicate voting.

The complete hardware setup can be simulated using **Wokwi**, allowing the system to be developed and tested without physical hardware.

---

## Table of Contents

- Project Overview
- Features
- System Architecture
- Hardware Components
- Pin Configuration
- Project Files
- Voting Workflow
- Admin Mode
- Database
- Communication
- Installation
- Running the Project
- Testing
- Technologies Used
- Limitations
- Future Improvements
- Project Status
- Educational Purpose
- License

---

## Project Overview

The system combines three major components:

```text
Arduino Mega 2560
        │
        │ Serial Communication
        ▼
   Python Bridge
        │
        ▼
  SQLite Database
```

The Arduino handles the physical voting interface, while Python and SQLite handle voter verification and voting status.

---

## Features

- Voter ID-based verification
- Duplicate voting prevention
- 4×4 keypad input
- 16×2 LCD interface
- Three candidate voting options
- NOTA option
- EEPROM-based vote storage
- SQLite voter database
- Python-Arduino communication
- Admin mode for viewing results
- Complete Wokwi simulation
- PlatformIO-based firmware build

---

## System Architecture

```text
┌─────────────────────────┐
│     Wokwi Hardware      │
│                         │
│ Arduino Mega 2560       │
│ LCD + Keypad + Buttons  │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│        main.cpp         │
│                         │
│ Voting & Hardware Logic │
└────────────┬────────────┘
             │
       Serial / RFC2217
             │
             ▼
┌─────────────────────────┐
│       bridge.py         │
│ Communication Bridge   │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│      database.py        │
│     SQLite Database     │
└─────────────────────────┘
```

---

## Hardware Components

| Component | Purpose |
|---|---|
| Arduino Mega 2560 | Main controller |
| 16×2 LCD | Display and user interface |
| 4×4 Membrane Keypad | Voter ID and command input |
| Candidate 1 Button | Candidate 1 voting |
| Candidate 2 Button | Candidate 2 voting |
| Candidate 3 Button | Candidate 3 voting |
| NOTA Button | None of the Above |

---

## Pin Configuration

### LCD

| LCD Pin | Arduino Pin |
|---|---:|
| RS | 12 |
| E | 11 |
| D4 | 5 |
| D5 | 4 |
| D6 | 3 |
| D7 | 2 |

### Keypad

| Keypad Pin | Arduino Pin |
|---|---:|
| R1 | 22 |
| R2 | 24 |
| R3 | 26 |
| R4 | 28 |
| C1 | 30 |
| C2 | 32 |
| C3 | 34 |
| C4 | 36 |

### Voting Buttons

| Button | Arduino Pin |
|---|---:|
| Candidate 1 | 38 |
| Candidate 2 | 40 |
| Candidate 3 | 42 |
| NOTA | 44 |

---

## Project Files

| File | Description |
|---|---|
| `main.cpp` | Main Arduino voting program |
| `diagram.json` | Wokwi circuit configuration |
| `database.py` | SQLite database management |
| `bridge.py` | Python-Arduino communication bridge |
| `wokwi.toml` | Wokwi firmware and serial configuration |
| `README.md` | Project documentation |

### `main.cpp`

Contains the main Arduino logic for:

- LCD control
- Keypad input
- Voter ID entry
- Candidate selection
- NOTA selection
- EEPROM vote storage
- Voting Mode
- Admin Mode

### `diagram.json`

Defines the complete Wokwi hardware circuit, including the Arduino Mega, LCD, keypad, and voting buttons.

### `database.py`

Handles SQLite database operations such as voter registration, voter verification, voting status, and status reset.

### `bridge.py`

Provides communication between the Arduino/Wokwi simulation and the SQLite database.

### `wokwi.toml`

Connects the Wokwi simulation to the compiled Arduino Mega firmware.

---

## Project Structure

```text
SmartVotingSystem/
│
├── main.cpp
├── diagram.json
├── database.py
├── bridge.py
├── wokwi.toml
└── README.md
```

Generated files may include:

```text
.pio/
voters.db
```

---

## Voting Workflow

```text
Start
  │
  ▼
Main Menu
  │
  ▼
Voting Mode
  │
  ▼
Enter Voter ID
  │
  ▼
Verify Voter
  │
  ├── Invalid ──────► Access Denied
  │
  ├── Already Voted ─► Access Denied
  │
  ▼
Voter Allowed
  │
  ▼
Select Candidate / NOTA
  │
  ▼
Store Vote
  │
  ▼
Mark Voter as Voted
  │
  ▼
Vote Confirmation
```

---

## Voter Verification

The voter enters their ID using the keypad.

The Arduino sends a request to the Python bridge:

```text
CHECK_VOTER:<voter_id>
```

The database checks whether the voter exists and whether they have already voted.

Possible responses:

```text
VOTER_ALLOWED
VOTER_ALREADY_VOTED
VOTER_NOT_FOUND
```

---

## Voting Options

The system provides four voting options:

| Button | Option |
|---|---|
| Candidate 1 | Candidate 1 |
| Candidate 2 | Candidate 2 |
| Candidate 3 | Candidate 3 |
| NOTA | None of the Above |

After a successful vote, the corresponding vote count is increased and stored in EEPROM.

---

## Admin Mode

The system provides a separate Admin Mode for viewing vote counts.

Press:

```text
B
```

to enter Admin Mode.

The administrator can view the counts for:

- Candidate 1
- Candidate 2
- Candidate 3
- NOTA

To exit the current mode:

```text
0000#
```

---

## Keypad Controls

| Key | Function |
|---|---|
| `A` | Voting Mode |
| `B` | Admin Mode |
| `0–9` | Enter voter ID |
| `*` | Clear input |
| `#` | Submit input |
| `0000#` | Exit current mode |

---

## Database

The project uses **SQLite** to store voter information.

The main table is:

```text
voters
```

| Field | Description |
|---|---|
| `voter_id` | Unique voter identifier |
| `has_voted` | Voting status |

The voting status is represented as:

```text
0 → Has not voted
1 → Already voted
```

The database is stored locally as:

```text
voters.db
```

---

## Communication

The Arduino communicates with the Python bridge through the Wokwi RFC2217 serial interface.

The configured interface is:

```text
rfc2217://localhost:4000
```

Example commands include:

```text
CHECK_VOTER:1001
MARK_VOTED:1001
```

---

## Wokwi Simulation

The complete hardware configuration is defined in:

```text
diagram.json
```

The Wokwi configuration is defined in:

```text
wokwi.toml
```

The firmware path is:

```text
.pio/build/megaatmega2560/firmware.elf
```

This allows the complete voting system to be tested virtually before using physical hardware.

---

## Installation

### Requirements

- Visual Studio Code
- PlatformIO
- Wokwi for VS Code
- Python 3
- PySerial

Install PySerial:

```bash
pip install pyserial
```

SQLite is included with Python.

---

## Running the Project

### 1. Open the Project

Open the project folder in Visual Studio Code.

### 2. Build the Firmware

Use PlatformIO to build the Arduino Mega 2560 firmware.

The firmware should be generated at:

```text
.pio/build/megaatmega2560/firmware.elf
```

### 3. Initialize the Database

```bash
python database.py
```

### 4. Start Wokwi

Run the Wokwi simulation using the `diagram.json` circuit.

### 5. Start the Python Bridge

```bash
python bridge.py
```

### 6. Test the System

Enter a registered voter ID and select one of the voting buttons.

---

## Testing

### Valid Voter

```text
1001#
```

Expected:

```text
VOTER_ALLOWED
```

### Invalid Voter

```text
9999#
```

Expected:

```text
VOTER_NOT_FOUND
```

### Duplicate Vote

After voter `1001` has voted, entering the same ID again should return:

```text
VOTER_ALREADY_VOTED
```

### Candidate Vote

Press a candidate button.

Expected:

```text
Vote Recorded
```

### NOTA Vote

Press the NOTA button.

Expected:

```text
NOTA
Vote Recorded
```

---

## Technologies Used

| Technology | Purpose |
|---|---|
| C++ | Arduino programming |
| Arduino Mega 2560 | Main controller |
| EEPROM | Vote storage |
| Python | Backend communication |
| SQLite | Voter database |
| PySerial | Serial communication |
| Wokwi | Hardware simulation |
| PlatformIO | Firmware build |

---

## Advantages

- Simple and intuitive voting interface
- Voter verification
- Duplicate voting prevention
- Persistent vote storage
- Separate NOTA option
- Database-backed voter management
- Hardware simulation using Wokwi
- Integration of embedded and software technologies

---

## Limitations

This project is an **academic prototype** and is not intended for use in real-world elections.

Current limitations include:

- Sample voter IDs
- Basic administrator access
- Three candidate options
- Dependency on the Python bridge
- Wokwi-based hardware simulation
- No production-level election security

---

## Future Improvements

Possible improvements include:

- Secure administrator authentication
- Stronger voter authentication
- Encrypted communication
- Web-based administration dashboard
- Result visualization
- Audit logging
- Multiple election support
- Multilingual interface
- Deployment using physical hardware

---

## Project Status

**Working Prototype / Wokwi Simulation**

The current system supports:

- Voter verification
- Duplicate vote prevention
- Candidate voting
- NOTA
- EEPROM vote counting
- SQLite voter management
- Python-Arduino communication
- Admin result display
- Wokwi simulation

---

## Educational Purpose

This project demonstrates the integration of:

- Embedded systems
- Microcontroller programming
- Database management
- Python programming
- Serial communication
- Hardware simulation
- Persistent data storage

The project is developed primarily for **academic and educational purposes**.

---

## License

This project is intended for educational and academic use.

If the project is reused or modified, appropriate credit should be given to the original contributors.
