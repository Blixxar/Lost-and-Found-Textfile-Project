# Campus Lost & Found Log Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a console-based Campus Lost & Found Log in C (C99) that stores reports in a text file, with menu-driven CRUD operations, search, status updates, and export functionality — following the professor's exact file handling style.

**Architecture:** Single-file `main.c` with modular functions, a `Report` struct, in-memory array of reports, and all persistence via text file with `|` delimiter. Three team members own distinct function modules per CONTRIBUTIONS.md.

**Tech Stack:** C99, standard library only (stdio.h, stdlib.h, string.h, ctype.h, time.h), gcc/MinGW and MSVC compatible.

**Spec:** User requirements in conversation (Project 2 – Text File Handling for CP1 at TUP Manila)

## Global Constraints

- C99 only: no VLAs, no POSIX, no compiler extensions
- `#define _CRT_SECURE_NO_WARNINGS` at top of every .c file
- `system("cls")` wrapped in `#ifdef _WIN32` with fallback
- Data file: `lost_found.txt` (relative path, #define constant)
- Compile cleanly: `gcc -Wall -Wextra -pedantic` and MSVC
- Input validation: fgets + sscanf, clear stdin, bounds checking
- Replace/reject `|` in user input to protect delimiter
- Date format DD/MM/YYYY with validation (day 1-31, month 1-12, year 1900-2100)
- Status labels: **Unclaimed**, **Claimed**, **Returned** (title case)
- Prof's file handling style: FILE* fp; fopen + immediate NULL check; fprintf per line; fclose; short comments; deliberate "w" vs "a" modes
- All six file functions used meaningfully: fopen/fclose, fprintf, fscanf (check return), fgetc (count lines), fputc (separators)
- Modes: "r" (load), "a" (append report), "w" (rewrite on update/edit/delete, fresh export), "a" (append export)
- Max reports: 1000 (define constant)
- Max string lengths: ID=10, Type=10, Item=50, Description=100, Location=50, Date=11, ReportedBy=50, Status=12, ClaimedBy=50
- Team: John (Module A: add/load/save), Bono (Module B: view/search), David (Module C: update/edit/delete/export)
- Project files: Code::Blocks .cbp, VS .sln/.vcxproj (or instructions), README.md, CONTRIBUTIONS.md, .gitignore

## Review Focus

1. **Empty/missing data file on startup** — Should create file gracefully, show friendly message, not crash
2. **Damaged/malformed line in data file** — fscanf return value check should skip bad line, continue loading others
3. **User types `|` in any text field** — Must be rejected or replaced so it never breaks record parsing
4. **Invalid date input (32/13/9999, letters, empty)** — Must validate format and ranges, re-prompt cleanly
5. **Export with zero matching reports** — Should show friendly message, NOT create blank file
6. **Update/delete non-existent ID** — Must verify ID exists, show clear error, not corrupt file
7. **Buffer overflow on long input** — fgets with sized buffer + proper stdin clearing must prevent overflow

---

### Task 1: Project Setup & Data Structures

**Files:**
- Create: `main.c` (overwrite existing)
- Create: `lost_found.txt` (sample data file)
- Create: `.gitignore`
- Create: `README.md`
- Create: `CONTRIBUTIONS.md`
- Create: `lost_found.cbp` (Code::Blocks project)
- Create: `LostAndFound.sln` + `LostAndFound.vcxproj` (Visual Studio project)

**Interfaces:**
- Produces: `Report` struct, `#define` constants, global `reports[]` array, `reportCount` variable

- [ ] **Step 1.1: Write constants and struct definition in main.c**

```c
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_REPORTS 1000
#define DATA_FILE "lost_found.txt"
#define MAX_ID_LEN 10
#define MAX_TYPE_LEN 10
#define MAX_ITEM_LEN 50
#define MAX_DESC_LEN 100
#define MAX_LOC_LEN 50
#define MAX_DATE_LEN 11
#define MAX_NAME_LEN 50
#define MAX_STATUS_LEN 12
#define MAX_LINE_LEN 512

typedef struct {
    char id[MAX_ID_LEN];
    char type[MAX_TYPE_LEN];       // "LOST" or "FOUND"
    char item[MAX_ITEM_LEN];
    char description[MAX_DESC_LEN];
    char location[MAX_LOC_LEN];
    char date[MAX_DATE_LEN];       // DD/MM/YYYY
    char reportedBy[MAX_NAME_LEN];
    char status[MAX_STATUS_LEN];   // "Unclaimed", "Claimed", "Returned"
    char claimedBy[MAX_NAME_LEN];  // "N/A" initially
} Report;

Report reports[MAX_REPORTS];
int reportCount = 0;
```

- [ ] **Step 1.2: Create .gitignore**

```
*.exe
*.obj
*.o
.vs/
Debug/
Release/
bin/
obj/
*.cbp
*.sln
*.vcxproj
*.vcxproj.filters
*.vcxproj.user
```

- [ ] **Step 1.3: Create sample lost_found.txt with 3-4 records**

Format per line: `ID|TYPE|ITEM|DESCRIPTION|LOCATION|DATE|REPORTED_BY|STATUS|CLAIMED_BY`

Example:
```
1|LOST|Black umbrella|Black compact umbrella with silver handle|Library|15/11/2024|Ana Santos|Unclaimed|N/A
2|FOUND|Student ID|Blue lanyard, ID number 2024-12345|Cafeteria|16/11/2024|Mark Reyes|Unclaimed|N/A
3|LOST|iPhone 13|Black iPhone 13 with cracked screen protector|Gym|17/11/2024|Lisa Chen|Claimed|Maria Gomez
4|FOUND|Wallet|Brown leather wallet, contains cash and cards|Parking Lot|18/11/2024|John Doe|Returned|Robert Kim
```

- [ ] **Step 1.4: Create Code::Blocks .cbp project file**
- [ ] **Step 1.5: Create Visual Studio .sln and .vcxproj files**
- [ ] **Step 1.6: Compile and verify empty main compiles**

Run: `gcc -Wall -Wextra -pedantic main.c -o lost_found`
Expected: Clean compile (main just has includes and struct)

---

### Task 2: Core File I/O — Load & Save (John - Module A)

**Files:**
- Modify: `main.c` (add loadReports, saveAllReports, getNextId)

**Interfaces:**
- Consumes: `Report` struct, `reports[]`, `reportCount`, `DATA_FILE`
- Produces: `loadReports()`, `saveAllReports()`, `getNextId()`

- [ ] **Step 2.1: Implement `loadReports()` — Author: John Hayden Nell**

```c
// Purpose: Load all reports from data file into memory array
// Parameters: none
// Returns: void
// Author: John Hayden Nell
void loadReports() {
    FILE *fp = fopen(DATA_FILE, "r"); // open for reading
    if (fp == NULL) {
        // File doesn't exist yet - that's OK, start fresh
        return;
    }
    
    char line[MAX_LINE_LEN];
    reportCount = 0;
    
    while (fgets(line, sizeof(line), fp) != NULL && reportCount < MAX_REPORTS) {
        // Use fscanf-style parsing with | delimiter, check return value
        Report r;
        int fields = sscanf(line, "%9[^|]|%9[^|]|%49[^|]|%99[^|]|%49[^|]|%10[^|]|%49[^|]|%11[^|]|%49[^\n]",
            r.id, r.type, r.item, r.description, r.location, r.date, 
            r.reportedBy, r.status, r.claimedBy);
        
        if (fields == 9) {
            reports[reportCount++] = r;
        }
        // Damaged line (fields != 9) is silently skipped per fscanf return check
    }
    
    fclose(fp); // save and close
}
```

- [ ] **Step 2.2: Implement `saveAllReports()` — Author: John Hayden Nell**

```c
// Purpose: Rewrite entire data file with current in-memory reports
// Parameters: none
// Returns: void
// Author: John Hayden Nell
void saveAllReports() {
    FILE *fp = fopen(DATA_FILE, "w"); // open for writing (overwrites)
    if (fp == NULL) {
        printf("Error: Could not open data file for writing.\n");
        return;
    }
    
    for (int i = 0; i < reportCount; i++) {
        fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
            reports[i].id, reports[i].type, reports[i].item,
            reports[i].description, reports[i].location, reports[i].date,
            reports[i].reportedBy, reports[i].status, reports[i].claimedBy);
    }
    
    fclose(fp); // save and close
}
```

- [ ] **Step 2.3: Implement `getNextId()` — Author: John Hayden Nell**

```c
// Purpose: Generate next auto-incrementing report ID
// Parameters: none
// Returns: int (next ID number)
// Author: John Hayden Nell
int getNextId() {
    int maxId = 0;
    for (int i = 0; i < reportCount; i++) {
        int id = atoi(reports[i].id);
        if (id > maxId) maxId = id;
    }
    return maxId + 1;
}
```

- [ ] **Step 2.4: Test load/save with sample data**

Run program, verify reports load correctly, add a report, verify file updated.

---

### Task 3: Input Utilities & Validation (Shared)

**Files:**
- Modify: `main.c` (add utilities)

**Interfaces:**
- Produces: `clearInputBuffer()`, `readLine()`, `sanitizePipe()`, `isValidDate()`, `toLowerCase()`, `containsKeyword()`

- [ ] **Step 3.1: Implement `clearInputBuffer()`**

```c
// Purpose: Clear leftover characters from stdin
// Parameters: none
// Returns: void
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
```

- [ ] **Step 3.2: Implement `readLine(char *buf, int maxLen)`**

```c
// Purpose: Safe line input with fgets, strips newline
// Parameters: buf - destination buffer, maxLen - buffer size
// Returns: void
void readLine(char *buf, int maxLen) {
    if (fgets(buf, maxLen, stdin) != NULL) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        else clearInputBuffer();
    } else {
        buf[0] = '\0';
        clearInputBuffer();
    }
}
```

- [ ] **Step 3.3: Implement `sanitizePipe(char *str)` — replace `|` with `-`**

```c
// Purpose: Replace pipe chars to protect delimiter
// Parameters: str - string to sanitize (modified in place)
// Returns: void
void sanitizePipe(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] == '|') str[i] = '-';
    }
}
```

- [ ] **Step 3.4: Implement `isValidDate(char *date)` — DD/MM/YYYY, ranges**

```c
// Purpose: Validate date format DD/MM/YYYY and ranges
// Parameters: date - string to validate
// Returns: 1 if valid, 0 otherwise
int isValidDate(char *date) {
    if (strlen(date) != 10) return 0;
    if (date[2] != '/' || date[5] != '/') return 0;
    
    int day, month, year;
    if (sscanf(date, "%2d/%2d/%4d", &day, &month, &year) != 3) return 0;
    
    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    
    // Basic month-day check (no leap year needed per spec)
    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (day > daysInMonth[month - 1]) return 0;
    
    return 1;
}
```

- [ ] **Step 3.5: Implement `toLowerCase(char *str)`**

```c
// Purpose: Convert string to lowercase in place
// Parameters: str - string to convert
// Returns: void
void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) str[i] = tolower(str[i]);
}
```

- [ ] **Step 3.6: Implement `containsKeyword(char *haystack, char *needle)` — case-insensitive**

```c
// Purpose: Case-insensitive substring search
// Parameters: haystack - text to search, needle - keyword
// Returns: 1 if found, 0 otherwise
int containsKeyword(char *haystack, char *needle) {
    char h[MAX_LINE_LEN], n[MAX_LINE_LEN];
    strncpy(h, haystack, MAX_LINE_LEN - 1); h[MAX_LINE_LEN - 1] = '\0';
    strncpy(n, needle, MAX_LINE_LEN - 1); n[MAX_LINE_LEN - 1] = '\0';
    toLowerCase(h); toLowerCase(n);
    return strstr(h, n) != NULL;
}
```

---

### Task 4: Module A — Add Report (John - Module A)

**Files:**
- Modify: `main.c` (add addReport)

**Interfaces:**
- Consumes: `Report` struct, `reports[]`, `reportCount`, `getNextId()`, `saveAllReports()`, input utils
- Produces: `addReport()`

- [ ] **Step 4.1: Implement `addReport()` — Author: John Hayden Nell**

```c
// Purpose: Add a new LOST or FOUND report, append to data file
// Parameters: none
// Returns: void
// Author: John Hayden Nell
void addReport() {
    if (reportCount >= MAX_REPORTS) {
        printf("Error: Maximum reports reached.\n");
        return;
    }
    
    Report r;
    char input[MAX_LINE_LEN];
    
    // Type: LOST or FOUND
    do {
        printf("Type (LOST/FOUND): ");
        readLine(input, sizeof(input));
        toLowerCase(input);
        if (strcmp(input, "lost") == 0) strcpy(r.type, "LOST");
        else if (strcmp(input, "found") == 0) strcpy(r.type, "FOUND");
        else printf("Invalid. Enter LOST or FOUND.\n");
    } while (strcmp(r.type, "LOST") != 0 && strcmp(r.type, "FOUND") != 0);
    
    // Item name
    do {
        printf("Item name: ");
        readLine(r.item, sizeof(r.item));
        if (strlen(r.item) == 0) printf("Item name cannot be empty.\n");
    } while (strlen(r.item) == 0);
    sanitizePipe(r.item);
    
    // Description
    printf("Description: ");
    readLine(r.description, sizeof(r.description));
    sanitizePipe(r.description);
    
    // Location
    do {
        printf("Location: ");
        readLine(r.location, sizeof(r.location));
        if (strlen(r.location) == 0) printf("Location cannot be empty.\n");
    } while (strlen(r.location) == 0);
    sanitizePipe(r.location);
    
    // Date
    do {
        printf("Date (DD/MM/YYYY): ");
        readLine(r.date, sizeof(r.date));
        if (!isValidDate(r.date)) printf("Invalid date format or range.\n");
    } while (!isValidDate(r.date));
    
    // Reported by
    do {
        printf("Reported by: ");
        readLine(r.reportedBy, sizeof(r.reportedBy));
        if (strlen(r.reportedBy) == 0) printf("Name cannot be empty.\n");
    } while (strlen(r.reportedBy) == 0);
    sanitizePipe(r.reportedBy);
    
    // Auto fields
    sprintf(r.id, "%d", getNextId());
    strcpy(r.status, "Unclaimed");
    strcpy(r.claimedBy, "N/A");
    
    // Append to data file using "a" mode
    FILE *fp = fopen(DATA_FILE, "a"); // open for append
    if (fp == NULL) {
        printf("Error: Could not open data file.\n");
        return;
    }
    
    fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
        r.id, r.type, r.item, r.description, r.location, r.date,
        r.reportedBy, r.status, r.claimedBy);
    
    fclose(fp); // save and close
    
    // Add to memory
    reports[reportCount++] = r;
    
    printf("Report added successfully! ID: %s\n", r.id);
}
```

---

### Task 5: Module B — View All & View by Status (Bono - Module B)

**Files:**
- Modify: `main.c` (add displayReportRow, viewAllReports, viewByStatus)

**Interfaces:**
- Consumes: `reports[]`, `reportCount`, `Report` struct
- Produces: `displayReportRow()`, `viewAllReports()`, `viewByStatus()`

- [ ] **Step 5.1: Implement `displayReportRow(Report *r)` — Author: Bono Tendencia**

```c
// Purpose: Print one report as aligned table row
// Parameters: r - pointer to report
// Returns: void
// Author: Bono Tendencia
void displayReportRow(Report *r) {
    printf("%-4s %-6s %-20s %-20s %-12s %-12s\n",
        r->id, r->type, r->item, r->location, r->date, r->status);
}
```

- [ ] **Step 5.2: Implement `printTableHeader()` — Author: Bono Tendencia**

```c
// Purpose: Print table header with separators
// Parameters: none
// Returns: void
// Author: Bono Tendencia
void printTableHeader() {
    printf("\n%-4s %-6s %-20s %-20s %-12s %-12s\n",
        "ID", "TYPE", "ITEM", "LOCATION", "DATE", "STATUS");
    printf("---- ------ -------------------- -------------------- ------------ ------------\n");
}
```

- [ ] **Step 5.3: Implement `viewAllReports()` — Author: Bono Tendencia**

```c
// Purpose: Display all reports in aligned table with counts
// Parameters: none
// Returns: void
// Author: Bono Tendencia
void viewAllReports() {
    if (reportCount == 0) {
        printf("No reports found.\n");
        return;
    }
    
    printTableHeader();
    int unclaimed = 0, claimed = 0, returned = 0;
    
    for (int i = 0; i < reportCount; i++) {
        displayReportRow(&reports[i]);
        if (strcmp(reports[i].status, "Unclaimed") == 0) unclaimed++;
        else if (strcmp(reports[i].status, "Claimed") == 0) claimed++;
        else if (strcmp(reports[i].status, "Returned") == 0) returned++;
    }
    
    printf("\nTotal: %d | Unclaimed: %d | Claimed: %d | Returned: %d\n",
        reportCount, unclaimed, claimed, returned);
}
```

- [ ] **Step 5.4: Implement `viewByStatus()` — Author: Bono Tendencia**

```c
// Purpose: Display reports filtered by status
// Parameters: none
// Returns: void
// Author: Bono Tendencia
void viewByStatus() {
    if (reportCount == 0) {
        printf("No reports found.\n");
        return;
    }
    
    char input[MAX_LINE_LEN];
    char status[MAX_STATUS_LEN];
    
    printf("Filter by status (Unclaimed/Claimed/Returned): ");
    readLine(input, sizeof(input));
    
    if (strcasecmp(input, "unclaimed") == 0) strcpy(status, "Unclaimed");
    else if (strcasecmp(input, "claimed") == 0) strcpy(status, "Claimed");
    else if (strcasecmp(input, "returned") == 0) strcpy(status, "Returned");
    else {
        printf("Invalid status.\n");
        return;
    }
    
    printTableHeader();
    int count = 0;
    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].status, status) == 0) {
            displayReportRow(&reports[i]);
            count++;
        }
    }
    
    if (count == 0) printf("No reports with status: %s\n", status);
    else printf("\nTotal matching: %d\n", count);
}
```

---

### Task 6: Module B — Search by Keyword (Bono - Module B)

**Files:**
- Modify: `main.c` (add searchByKeyword)

**Interfaces:**
- Consumes: `reports[]`, `reportCount`, `containsKeyword()`
- Produces: `searchByKeyword()`

- [ ] **Step 6.1: Implement `searchByKeyword()` — Author: Bono Tendencia**

```c
// Purpose: Case-insensitive search across item, description, location
// Parameters: none
// Returns: void
// Author: Bono Tendencia
void searchByKeyword() {
    if (reportCount == 0) {
        printf("No reports to search.\n");
        return;
    }
    
    char keyword[MAX_LINE_LEN];
    printf("Enter keyword: ");
    readLine(keyword, sizeof(keyword));
    
    if (strlen(keyword) == 0) {
        printf("Keyword cannot be empty.\n");
        return;
    }
    
    printTableHeader();
    int count = 0;
    for (int i = 0; i < reportCount; i++) {
        if (containsKeyword(reports[i].item, keyword) ||
            containsKeyword(reports[i].description, keyword) ||
            containsKeyword(reports[i].location, keyword)) {
            displayReportRow(&reports[i]);
            count++;
        }
    }
    
    if (count == 0) printf("No matches found for: %s\n", keyword);
    else printf("\nTotal matches: %d\n", count);
}
```

---

### Task 7: Module C — Update Status (David - Module C)

**Files:**
- Modify: `main.c` (add updateStatus, findReportIndex)

**Interfaces:**
- Consumes: `reports[]`, `reportCount`, `saveAllReports()`, `findReportIndex()`
- Produces: `updateStatus()`, `findReportIndex()`

- [ ] **Step 7.1: Implement `findReportIndex(char *id)` — Author: David Pansoy**

```c
// Purpose: Find report index by ID
// Parameters: id - report ID to find
// Returns: index (0-based) or -1 if not found
// Author: David Pansoy
int findReportIndex(char *id) {
    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].id, id) == 0) return i;
    }
    return -1;
}
```

- [ ] **Step 7.2: Implement `updateStatus()` — Author: David Pansoy**

```c
// Purpose: Change report status to Claimed or Returned, record claimant
// Parameters: none
// Returns: void
// Author: David Pansoy
void updateStatus() {
    if (reportCount == 0) {
        printf("No reports to update.\n");
        return;
    }
    
    char id[MAX_ID_LEN];
    printf("Enter Report ID: ");
    readLine(id, sizeof(id));
    
    int idx = findReportIndex(id);
    if (idx == -1) {
        printf("Report ID not found.\n");
        return;
    }
    
    // Show current status
    printf("Current status: %s\n", reports[idx].status);
    
    if (strcmp(reports[idx].status, "Returned") == 0) {
        printf("Cannot update: already Returned.\n");
        return;
    }
    
    char input[MAX_LINE_LEN];
    char newStatus[MAX_STATUS_LEN];
    
    do {
        printf("New status (Claimed/Returned): ");
        readLine(input, sizeof(input));
        if (strcasecmp(input, "claimed") == 0) strcpy(newStatus, "Claimed");
        else if (strcasecmp(input, "returned") == 0) strcpy(newStatus, "Returned");
        else printf("Invalid. Enter Claimed or Returned.\n");
    } while (strcmp(newStatus, "Claimed") != 0 && strcmp(newStatus, "Returned") != 0);
    
    // Claimed by
    char claimant[MAX_NAME_LEN];
    do {
        printf("Claimed by (name): ");
        readLine(claimant, sizeof(claimant));
        if (strlen(claimant) == 0) printf("Name cannot be empty.\n");
    } while (strlen(claimant) == 0);
    sanitizePipe(claimant);
    
    // Update in memory
    strcpy(reports[idx].status, newStatus);
    strcpy(reports[idx].claimedBy, claimant);
    
    // Rewrite entire data file using "w" mode
    saveAllReports();
    
    printf("Status updated to %s. Claimed by: %s\n", newStatus, claimant);
}
```

---

### Task 8: Module C — Edit Report (David - Module C)

**Files:**
- Modify: `main.c` (add editReport)

**Interfaces:**
- Consumes: `reports[]`, `reportCount`, `findReportIndex()`, `saveAllReports()`, input utils
- Produces: `editReport()`

- [ ] **Step 8.1: Implement `editReport()` — Author: David Pansoy**

```c
// Purpose: Edit details of an existing report by ID
// Parameters: none
// Returns: void
// Author: David Pansoy
void editReport() {
    if (reportCount == 0) {
        printf("No reports to edit.\n");
        return;
    }
    
    char id[MAX_ID_LEN];
    printf("Enter Report ID: ");
    readLine(id, sizeof(id));
    
    int idx = findReportIndex(id);
    if (idx == -1) {
        printf("Report ID not found.\n");
        return;
    }
    
    Report *r = &reports[idx];
    char input[MAX_LINE_LEN];
    
    // Show current values, allow empty to keep
    printf("Editing report %s (press Enter to keep current value)\n", id);
    
    // Type
    printf("Type [%s]: ", r->type);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        toLowerCase(input);
        if (strcmp(input, "lost") == 0) strcpy(r->type, "LOST");
        else if (strcmp(input, "found") == 0) strcpy(r->type, "FOUND");
        else printf("Invalid type, keeping current.\n");
    }
    
    // Item
    printf("Item [%s]: ", r->item);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->item, input, MAX_ITEM_LEN - 1);
        r->item[MAX_ITEM_LEN - 1] = '\0';
        sanitizePipe(r->item);
    }
    
    // Description
    printf("Description [%s]: ", r->description);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->description, input, MAX_DESC_LEN - 1);
        r->description[MAX_DESC_LEN - 1] = '\0';
        sanitizePipe(r->description);
    }
    
    // Location
    printf("Location [%s]: ", r->location);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->location, input, MAX_LOC_LEN - 1);
        r->location[MAX_LOC_LEN - 1] = '\0';
        sanitizePipe(r->location);
    }
    
    // Date
    printf("Date [%s]: ", r->date);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        if (isValidDate(input)) {
            strncpy(r->date, input, MAX_DATE_LEN - 1);
            r->date[MAX_DATE_LEN - 1] = '\0';
        } else {
            printf("Invalid date, keeping current.\n");
        }
    }
    
    // Reported by
    printf("Reported by [%s]: ", r->reportedBy);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->reportedBy, input, MAX_NAME_LEN - 1);
        r->reportedBy[MAX_NAME_LEN - 1] = '\0';
        sanitizePipe(r->reportedBy);
    }
    
    // Rewrite data file using "w" mode
    saveAllReports();
    
    printf("Report updated successfully.\n");
}
```

---

### Task 9: Module C — Delete Report (David - Module C)

**Files:**
- Modify: `main.c` (add deleteReport)

**Interfaces:**
- Consumes: `reports[]`, `reportCount`, `findReportIndex()`, `saveAllReports()`
- Produces: `deleteReport()`

- [ ] **Step 9.1: Implement `deleteReport()` — Author: David Pansoy**

```c
// Purpose: Delete a report by ID with confirmation
// Parameters: none
// Returns: void
// Author: David Pansoy
void deleteReport() {
    if (reportCount == 0) {
        printf("No reports to delete.\n");
        return;
    }
    
    char id[MAX_ID_LEN];
    printf("Enter Report ID: ");
    readLine(id, sizeof(id));
    
    int idx = findReportIndex(id);
    if (idx == -1) {
        printf("Report ID not found.\n");
        return;
    }
    
    // Show report details
    printTableHeader();
    displayReportRow(&reports[idx]);
    
    // Confirm
    char confirm[MAX_LINE_LEN];
    printf("\nDelete this report? (yes/no): ");
    readLine(confirm, sizeof(confirm));
    toLowerCase(confirm);
    
    if (strcmp(confirm, "yes") != 0 && strcmp(confirm, "y") != 0) {
        printf("Deletion cancelled.\n");
        return;
    }
    
    // Shift array left
    for (int i = idx; i < reportCount - 1; i++) {
        reports[i] = reports[i + 1];
    }
    reportCount--;
    
    // Rewrite data file using "w" mode
    saveAllReports();
    
    printf("Report deleted.\n");
}
```

---

### Task 10: Module C — Export to TXT (David - Module C)

**Files:**
- Modify: `main.c` (add exportToTxt, fileExists)

**Interfaces:**
- Consumes: `reports[]`, `reportCount`, `fgetc()` for counting/display, `fputc()` for separators
- Produces: `exportToTxt()`, `fileExists()`

- [ ] **Step 10.1: Implement `fileExists(char *filename)` — Author: David Pansoy**

```c
// Purpose: Check if file exists
// Parameters: filename - file to check
// Returns: 1 if exists, 0 otherwise
// Author: David Pansoy
int fileExists(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        fclose(fp);
        return 1;
    }
    return 0;
}
```

- [ ] **Step 10.2: Implement `drawSeparator(FILE *fp, char c, int len)` — uses fputc — Author: David Pansoy**

```c
// Purpose: Write a line of separator characters using fputc
// Parameters: fp - file pointer, c - character, len - length
// Returns: void
// Author: David Pansoy
void drawSeparator(FILE *fp, char c, int len) {
    for (int i = 0; i < len; i++) fputc(c, fp);
    fputc('\n', fp);
}
```

- [ ] **Step 10.3: Implement `exportToTxt()` — Author: David Pansoy**

```c
// Purpose: Export reports to a .txt file (all or unclaimed only)
// Parameters: none
// Returns: void
// Author: David Pansoy
void exportToTxt() {
    if (reportCount == 0) {
        printf("No reports to export.\n");
        return;
    }
    
    char filename[MAX_LINE_LEN];
    printf("Export filename (without .txt): ");
    readLine(filename, sizeof(filename));
    
    if (strlen(filename) == 0) {
        printf("Filename cannot be empty.\n");
        return;
    }
    
    // Add .txt if missing
    if (strstr(filename, ".txt") == NULL) {
        strcat(filename, ".txt");
    }
    
    // Choose filter
    char input[MAX_LINE_LEN];
    int exportUnclaimedOnly = 0;
    printf("Export (1) All reports or (2) Unclaimed only? ");
    readLine(input, sizeof(input));
    if (strcmp(input, "2") == 0) exportUnclaimedOnly = 1;
    
    // Count matching reports
    int matchCount = 0;
    for (int i = 0; i < reportCount; i++) {
        if (!exportUnclaimedOnly || strcmp(reports[i].status, "Unclaimed") == 0) {
            matchCount++;
        }
    }
    
    if (matchCount == 0) {
        printf("No reports match the selected filter.\n");
        return;
    }
    
    // Check if file exists
    char mode[3] = "w"; // default overwrite
    if (fileExists(filename)) {
        printf("File '%s' already exists. (o)verwrite or (a)ppend? ", filename);
        readLine(input, sizeof(input));
        toLowerCase(input);
        if (strcmp(input, "a") == 0 || strcmp(input, "append") == 0) {
            strcpy(mode, "a");
        }
    }
    
    FILE *fp = fopen(filename, mode); // "w" for fresh, "a" for append
    if (fp == NULL) {
        printf("Error: Could not create export file.\n");
        return;
    }
    
    // Write title
    fprintf(fp, "CAMPUS LOST & FOUND REPORT\n");
    drawSeparator(fp, '=', 60); // uses fputc
    
    if (exportUnclaimedOnly) {
        fprintf(fp, "BULLETIN BOARD - UNCLAIMED ITEMS\n");
        drawSeparator(fp, '-', 60);
    } else {
        fprintf(fp, "ALL REPORTS\n");
        drawSeparator(fp, '-', 60);
    }
    
    // Header row
    fprintf(fp, "%-4s %-6s %-25s %-25s %-12s %-12s %-15s\n",
        "ID", "TYPE", "ITEM", "LOCATION", "DATE", "STATUS", "CLAIMED BY");
    drawSeparator(fp, '-', 100);
    
    // Data rows
    for (int i = 0; i < reportCount; i++) {
        if (!exportUnclaimedOnly || strcmp(reports[i].status, "Unclaimed") == 0) {
            fprintf(fp, "%-4s %-6s %-25s %-25s %-12s %-12s %-15s\n",
                reports[i].id, reports[i].type, reports[i].item,
                reports[i].location, reports[i].date, reports[i].status,
                reports[i].claimedBy);
        }
    }
    
    drawSeparator(fp, '=', 60);
    fprintf(fp, "Total exported: %d\n", matchCount);
    
    fclose(fp); // save and close
    
    printf("Exported %d report(s) to %s\n", matchCount, filename);
    
    // Optional: show file content using fgetc
    printf("View exported file? (yes/no): ");
    readLine(input, sizeof(input));
    toLowerCase(input);
    if (strcmp(input, "yes") == 0 || strcmp(input, "y") == 0) {
        FILE *view = fopen(filename, "r");
        if (view != NULL) {
            int ch;
            printf("\n--- Exported File Content ---\n");
            while ((ch = fgetc(view)) != EOF) putchar(ch);
            fclose(view);
        }
    }
}
```

---

### Task 11: Menu System & Main Loop

**Files:**
- Modify: `main.c` (add displayMenu, clearScreen, main)

**Interfaces:**
- Consumes: All feature functions
- Produces: `displayMenu()`, `clearScreen()`, `main()`

- [ ] **Step 11.1: Implement `clearScreen()` — cross-platform**

```c
// Purpose: Clear console screen
// Parameters: none
// Returns: void
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
```

- [ ] **Step 11.2: Implement `displayMenu()`**

```c
// Purpose: Display main menu options
// Parameters: none
// Returns: void
void displayMenu() {
    clearScreen();
    printf("========================================\n");
    printf("   CAMPUS LOST & FOUND LOG\n");
    printf("========================================\n");
    printf("1. Report an item\n");
    printf("2. View all reports\n");
    printf("3. View by status\n");
    printf("4. Search by keyword\n");
    printf("5. Update status\n");
    printf("6. Edit a report\n");
    printf("7. Delete a report\n");
    printf("8. Export to TXT\n");
    printf("9. Exit\n");
    printf("----------------------------------------\n");
    printf("Enter choice: ");
}
```

- [ ] **Step 11.3: Implement `pause()` — Press Enter to continue**

```c
// Purpose: Wait for user to press Enter
// Parameters: none
// Returns: void
void pause() {
    printf("\nPress Enter to continue...");
    clearInputBuffer();
}
```

- [ ] **Step 11.4: Implement `main()` with menu loop**

```c
// Purpose: Program entry point, menu loop
// Parameters: argc, argv (unused)
// Returns: 0 on success
int main() {
    loadReports(); // Load on startup
    
    int choice;
    char input[MAX_LINE_LEN];
    
    do {
        displayMenu();
        readLine(input, sizeof(input));
        
        if (sscanf(input, "%d", &choice) != 1) {
            printf("Invalid input. Enter a number 1-9.\n");
            pause();
            continue;
        }
        
        switch (choice) {
            case 1: addReport(); break;
            case 2: viewAllReports(); break;
            case 3: viewByStatus(); break;
            case 4: searchByKeyword(); break;
            case 5: updateStatus(); break;
            case 6: editReport(); break;
            case 7: deleteReport(); break;
            case 8: exportToTxt(); break;
            case 9: printf("Goodbye!\n"); break;
            default: printf("Invalid choice. Enter 1-9.\n");
        }
        
        if (choice != 9) pause();
        
    } while (choice != 9);
    
    return 0;
}
```

---

### Task 12: Documentation — README.md & CONTRIBUTIONS.md

**Files:**
- Create: `README.md`
- Create: `CONTRIBUTIONS.md`

- [ ] **Step 12.1: Write README.md with compile/run instructions for gcc, Code::Blocks, Visual Studio, data file format, export sample**
- [ ] **Step 12.2: Write CONTRIBUTIONS.md listing each member's functions**
- [ ] **Step 12.3: Apply humanizer skill to both files**

---

### Task 13: Testing & Verification

**Files:**
- Test: Manual test scenarios per requirements

- [ ] **Step 13.1: Compile with `gcc -Wall -Wextra -pedantic main.c -o lost_found` — verify zero warnings**
- [ ] **Step 13.2: Test valid input flow — add, view, search, update, edit, delete, export**
- [ ] **Step 13.3: Test invalid input — menu choices, empty strings, long input, pipe chars**
- [ ] **Step 13.4: Test empty/missing data file — delete lost_found.txt, run program**
- [ ] **Step 13.5: Test damaged line in data file — manually corrupt a line, run program**
- [ ] **Step 13.6: Test invalid dates — 32/13/9999, letters, empty, leap year edge**
- [ ] **Step 13.7: Test keyword search — no matches, case variations, partial matches**
- [ ] **Step 13.8: Test update/delete non-existent ID**
- [ ] **Step 13.9: Test export with 0 reports, with several, overwrite vs append**
- [ ] **Step 13.10: Test invalid export filename**
- [ ] **Step 13.11: Verify Code::Blocks project opens and builds**
- [ ] **Step 13.12: Verify Visual Studio project opens and builds (or document steps)**
- [ ] **Step 13.13: Run code-review skill against rubric**
- [ ] **Step 13.14: Self-grade table with weighted scores**

---

### Task 14: Final Polish & Submission Prep

**Files:**
- Modify: any final fixes

- [ ] **Step 14.1: Verify all 6 file functions used meaningfully with comments**
- [ ] **Step 14.2: Verify all #define constants, no magic numbers**
- [ ] **Step 14.3: Verify function comment blocks with author names**
- [ ] **Step 14.4: Verify aligned table output, clear messages, pauses**
- [ ] **Step 14.5: Final compile check on both compilers**
- [ ] **Step 14.6: Package all files in project folder**