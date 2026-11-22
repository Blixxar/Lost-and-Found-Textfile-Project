# Contributions

**Project:** Campus Lost & Found Log  
**Course:** Computer Programming 1 — TUP Manila  
**Project 2:** Text File Handling (File System)

---

## Team Members & Module Ownership

| Member | Module | Functions Owned |
|--------|--------|-----------------|
| **John Hayden Nell** | Module A: Data Management | `loadReports()`, `saveAllReports()`, `getNextId()`, `addReport()` |
| **Bono Tendencia** | Module B: Viewing & Search | `displayReportRow()`, `printTableHeader()`, `viewAllReports()`, `viewByStatus()`, `searchByKeyword()` |
| **David Pansoy** | Module C: Modification & Export | `findReportIndex()`, `updateStatus()`, `editReport()`, `deleteReport()`, `fileExists()`, `drawSeparator()`, `exportToTxt()` |

---

## Shared Utilities (Collaborative)

The following utility functions were developed collaboratively and used across all modules:

- `clearInputBuffer()` — Stdin cleanup
- `readLine()` — Safe line input with `fgets`
- `sanitizePipe()` — Replace `|` with `-` to protect delimiter
- `isValidDate()` — DD/MM/YYYY validation with range checks
- `toLowerCase()` — Case-insensitive helper
- `containsKeyword()` — Case-insensitive substring search
- `clearScreen()` — Cross-platform console clear
- `displayMenu()` — Main menu rendering
- `pause()` — Press Enter to continue
- `main()` — Program entry point and menu loop

---

## Equal Contribution Statement

Each member was responsible for a complete, independent module with well-defined interfaces:

- **John (Module A)** handles all data persistence: loading on startup, saving after changes, ID generation, and adding new records. His functions form the foundation that the other modules build on.

- **Bono (Module B)** handles all read-only display and search operations. His functions consume the in-memory array populated by John's `loadReports()` and provide the user-facing views.

- **David (Module C)** handles all write operations that modify existing records: status updates, edits, deletions, and exports. His functions use John's `saveAllReports()` to persist changes and Bono's display helpers for confirmations.

All three members:
- Wrote their functions with complete documentation (purpose, parameters, return value, author)
- Followed the professor's file handling style consistently
- Used `#define` constants (no magic numbers)
- Implemented proper input validation and error handling
- Tested their modules independently and in integration

---

## Function Reference by Author

### John Hayden Nell
```c
void loadReports();           // "r" mode, fscanf with return check
void saveAllReports();        // "w" mode, fprintf per record
int getNextId();              // Auto-increment from existing IDs
void addReport();             // "a" mode append, full input validation
```

### Bono Tendencia
```c
void displayReportRow(Report *r);    // Aligned table row
void printTableHeader();             // Header with separators
void viewAllReports();               // All reports + status counts
void viewByStatus();                 // Filter by Unclaimed/Claimed/Returned
void searchByKeyword();              // Case-insensitive item/desc/location
```

### David Pansoy
```c
int findReportIndex(char *id);       // Linear search by ID
void updateStatus();                 // Claimed/Returned + claimant name
void editReport();                   // Edit any field, keep on Enter
void deleteReport();                 // Confirm + array shift + save
int fileExists(char *filename);      // fopen "r" check
void drawSeparator(FILE*, char, int); // fputc for export lines
void exportToTxt();                  // "w"/"a" export with fgetc preview
```

---

## Verification

All functions compile cleanly under:
- `gcc -Wall -Wextra -pedantic -std=c99` (MinGW in Code::Blocks)
- MSVC (Visual Studio 2022, `/Wall` with suppressions)

All 9 menu features tested and working:
- ✅ Add report (LOST/FOUND)
- ✅ View all (aligned table + counts)
- ✅ View by status (filter works)
- ✅ Search keyword (case-insensitive, multi-field)
- ✅ Update status (Claimed/Returned + claimant)
- ✅ Edit report (partial updates supported)
- ✅ Delete report (confirmation required)
- ✅ Export TXT (all/unclaimed, overwrite/append, fgetc preview)
- ✅ Exit