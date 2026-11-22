# Campus Lost & Found Log

**Project 2 – Text File Handling (File System)**  
Computer Programming 1 — TUP Manila

**Authors:** John Hayden Nell, Bono Tendencia
---

## Overview

A console-based Campus Lost & Found Log where students and staff can report lost or found items. All reports are saved in a text file and can be viewed, searched, updated, edited, deleted, and exported.

---

## Features

1. **Report an item** — Add a new LOST or FOUND report (appends to data file)
2. **View all reports** — Aligned table with counts (total / unclaimed / claimed / returned)
3. **View by status** — Filter reports by Unclaimed, Claimed, or Returned
4. **Search by keyword** — Case-insensitive search across item, description, and location
5. **Update status** — Change status to Claimed or Returned with claimant name
6. **Edit a report** — Modify details of an existing report by ID
7. **Delete a report** — Remove a report by ID with confirmation
8. **Export to TXT** — Export all or unclaimed reports to a formatted .txt file
9. **Exit** — Quit the program

---

## Data File Format

**File:** `lost_found.txt` (in the working directory)

**Format:** One report per line, fields separated by `|`

```
ID|TYPE|ITEM|DESCRIPTION|LOCATION|DATE|REPORTED_BY|STATUS|CLAIMED_BY
```

**Example:**
```
1|LOST|Black umbrella|Black compact umbrella with silver handle|Library|15/11/2024|Ana Santos|Unclaimed|N/A
2|FOUND|Student ID|Blue lanyard, ID number 2024-12345|Cafeteria|16/11/2024|Mark Reyes|Unclaimed|N/A
```

**Field details:**
- **ID** — Auto-generated, increasing integer
- **TYPE** — `LOST` or `FOUND`
- **ITEM** — Item name (e.g., "Black umbrella")
- **DESCRIPTION** — Details like color, brand, markings
- **LOCATION** — Where lost/found
- **DATE** — DD/MM/YYYY (validated)
- **REPORTED_BY** — Name of reporter
- **STATUS** — `Unclaimed`, `Claimed`, or `Returned`
- **CLAIMED_BY** — Claimant name, or `N/A` if unclaimed

**Note:** The `|` character is reserved as a delimiter. If entered in any text field, it is automatically replaced with `-`.

---

## Compilation & Running

### GCC (MinGW / Code::Blocks / Terminal)

```bash
gcc -Wall -Wextra -pedantic -std=c99 main.c -o lost_found
./lost_found
```

### Code::Blocks

1. Open `lost_found.cbp` in Code::Blocks
2. Press **F9** (Build and Run) or **Ctrl+F10** (Build) then **Ctrl+F10** (Run)

**Working directory:** The folder containing `lost_found.cbp`. The data file `lost_found.txt` and any exported `.txt` files will appear here.

### Visual Studio

1. Open `LostAndFound.sln` in Visual Studio
2. Select **Debug** or **Release** and **x64** configuration
3. Press **Ctrl+F5** (Start Without Debugging) or **F5** (Start Debugging)

**Working directory:** By default, `x64/Debug/` or `x64/Release/` under the solution folder. The data file `lost_found.txt` must be copied there, or run from the project folder using **Project > Properties > Debugging > Working Directory** set to `$(SolutionDir)`.

---

## Export File Sample

**File:** `bulletin_unclaimed.txt` (user-chosen name)

```
CAMPUS LOST & FOUND REPORT
============================================================
BULLETIN BOARD - UNCLAIMED ITEMS
------------------------------------------------------------
ID   TYPE   ITEM                      LOCATION                  DATE         STATUS       CLAIMED BY     
----------------------------------------------------------------------------------------------------
1    LOST   Black umbrella            Library                   15/11/2024   Unclaimed    N/A            
2    FOUND  Student ID                Cafeteria                 16/11/2024   Unclaimed    N/A            
============================================================
Total exported: 2
```

---

## Input Validation

- Menu choices: Must be 1–9
- Report ID: Must exist for update/edit/delete
- Type: Must be LOST or FOUND (case-insensitive)
- Status: Must be Unclaimed, Claimed, or Returned
- Date: DD/MM/YYYY format, day 1–31, month 1–12, year 1900–2100
- Required fields (type, item, location, date, reported by): Cannot be empty
- Pipe character `|`: Automatically replaced with `-` in all text fields
- Buffer overflow: Protected by `fgets` with sized buffers

---

## File Handling (Professor's Style)

The program demonstrates all six standard file functions meaningfully:

| Function | Usage |
|----------|-------|
| `fopen` / `fclose` | Every file operation with immediate NULL check |
| `fprintf` | Writing records to data file and export file (one per line) |
| `fscanf` | Loading records with return-value check (skips damaged lines) |
| `fgetc` | Reading export file back to screen character-by-character |
| `fputc` | Drawing separator lines (`=`, `-`) in export file |

**File modes used deliberately:**
- `"r"` — Load existing reports
- `"a"` — Append a new report
- `"w"` — Rewrite entire file after update/edit/delete; create fresh export
- `"a"` — Append to existing export file (user choice)

---

## Project Structure

```
Lost and Found/
├── main.c              # Complete program (single file)
├── lost_found.txt      # Sample data file
├── lost_found.cbp      # Code::Blocks project
├── LostAndFound.sln    # Visual Studio solution
├── LostAndFound.vcxproj # Visual Studio project
├── .gitignore          # Build artifacts
├── README.md           # This file
└── CONTRIBUTIONS.md    # Team member contributions
```

---

## Team Contributions

See [CONTRIBUTIONS.md](CONTRIBUTIONS.md) for detailed breakdown.

- **John Hayden Nell** — Module A: File I/O (load/save), Add Report, Module B: View All, View by Status, Search, Module C: Update Status, Edit, Delete, Export
- **Bono Tendencia** —

---

## Requirements Met

- ✅ C99 standard, no external libraries
- ✅ Compiles cleanly: `gcc -Wall -Wextra -pedantic` and MSVC
- ✅ `#define _CRT_SECURE_NO_WARNINGS` for Visual Studio compatibility
- ✅ Cross-platform `system("cls")` / `system("clear")`
- ✅ All 6 file functions used with professor's style
- ✅ Input validation on all user entries
- ✅ Graceful handling of missing/empty/corrupted data file
- ✅ Modular functions with author comments
- ✅ Aligned table output, clear messages, pause prompts
