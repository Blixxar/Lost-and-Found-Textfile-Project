// ============================================================================
// File: main.c
// Project: Campus Lost & Found Log System
// Description: Console-based text file management system for logging, searching,
//              updating, matching, and exporting campus lost and found records.
// Architecture: Single-file modular C99 with robust file I/O and text persistence.
// ============================================================================

#define _CRT_SECURE_NO_WARNINGS

// Standard C99 Library Headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// ============================================================================
// Preprocessor Constants and Limits
// ============================================================================
#define MAX_REPORTS 1000          // Maximum number of reports stored in memory
#define DATA_FILE "lost_found.txt"// Primary persistent data storage file
#define MAX_ID_LEN 10             // Maximum length for unique report ID
#define MAX_TYPE_LEN 10           // Maximum length for report type (LOST / FOUND)
#define MAX_ITEM_LEN 50           // Maximum length for item name / title
#define MAX_DESC_LEN 100          // Maximum length for item description
#define MAX_LOC_LEN 50            // Maximum length for campus location
#define MAX_DATE_LEN 11           // Length for date string (DD/MM/YYYY + null terminator)
#define MAX_NAME_LEN 50           // Maximum length for person names
#define MAX_STATUS_LEN 12         // Maximum length for status (Unclaimed, Claimed, Returned)
#define MAX_LINE_LEN 512          // General buffer size for line input and file lines

// ============================================================================
// Data Structure Definitions
// ============================================================================
// Purpose: Structure representing a single lost or found campus incident report
typedef struct {
    char id[MAX_ID_LEN];           // Unique identifier number for the report
    char type[MAX_TYPE_LEN];       // Type of report: "LOST" or "FOUND"
    char item[MAX_ITEM_LEN];       // Item name or title
    char description[MAX_DESC_LEN];// Detailed description of item appearance/contents
    char location[MAX_LOC_LEN];    // Campus location where item was lost or found
    char date[MAX_DATE_LEN];       // Date of report in DD/MM/YYYY format
    char reportedBy[MAX_NAME_LEN]; // Name of student/faculty/staff who filed report
    char status[MAX_STATUS_LEN];   // Status: "Unclaimed", "Claimed", "Returned"
    char claimedBy[MAX_NAME_LEN];  // Name of person claiming the item ("N/A" if unclaimed)
} Report;

// ============================================================================
// Global State Variables
// ============================================================================
// Array storing loaded reports in memory for fast querying and manipulation
Report reports[MAX_REPORTS];

// Current count of active reports loaded into memory
int reportCount = 0;

// ============================================================================
// Forward Declarations of Functions
// ============================================================================
void clearInputBuffer(void);
int readLine(char *buf, int maxLen);
void sanitizePipe(char *str);
int isValidDate(const char *date);
void toLowerCase(char *str);
int containsKeyword(const char *haystack, const char *needle);
int findReportIndex(const char *id);
int getNextId(void);
int fileExists(const char *filename);
void drawSeparator(FILE *fp, char c, int len);
void clearScreen(void);
void pauseProgram(void);
void printTableHeader(void);
void displayReportRow(const Report *r);

// Core File Persistence Functions
void loadReports(void);
void saveAllReports(void);

// Standard Operations
void addReport(void);
void viewAllReports(void);
void viewByStatus(void);
void searchByKeyword(void);
void updateStatus(void);
void editReport(void);
void deleteReport(void);
void exportToTxt(void);

// Useful Added Features
void findPotentialMatches(void);
void viewStatistics(void);
void viewReportDetails(void);
void filterByCategory(void);

// UI and Navigation
void displayMenu(void);

// ============================================================================
// Input & Utility Functions
// ============================================================================

// Purpose: Clear remaining characters from standard input stream buffer
// Parameters: none
// Returns: void
void clearInputBuffer(void) {
    int c;
    // Read and discard characters until newline or EOF is reached
    while ((c = getchar()) != '\n' && c != EOF);
}

// Purpose: Safely read a string line from stdin with buffer size limit
// Parameters: buf - destination char buffer, maxLen - buffer capacity
// Returns: 1 on success, 0 on EOF
int readLine(char *buf, int maxLen) {
    if (fgets(buf, maxLen, stdin) != NULL) {
        size_t len = strlen(buf);
        // If newline is present at the end, replace it with null terminator
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        } else {
            // If line exceeded buffer capacity, flush remaining characters
            clearInputBuffer();
        }
        return 1;
    } else {
        // If fgets failed or reached EOF, set empty string and return 0
        buf[0] = '\0';
        return 0;
    }
}

// Purpose: Replace pipe characters '|' with '-' to preserve file delimiter
// Parameters: str - pointer to string to sanitize in-place
// Returns: void
void sanitizePipe(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        // Delimiter protection: replace pipe with hyphen
        if (str[i] == '|') {
            str[i] = '-';
        }
    }
}

// Purpose: Validate date format DD/MM/YYYY and valid calendar ranges
// Parameters: date - date string to test
// Returns: 1 if date is valid, 0 if invalid
int isValidDate(const char *date) {
    // Check exact length of DD/MM/YYYY format
    if (strlen(date) != 10) {
        return 0;
    }
    // Verify slash positions
    if (date[2] != '/' || date[5] != '/') {
        return 0;
    }

    int day = 0;
    int month = 0;
    int year = 0;

    // Parse the day, month, and year components
    if (sscanf(date, "%2d/%2d/%4d", &day, &month, &year) != 3) {
        return 0;
    }

    // Verify reasonable year bounds
    if (year < 1900 || year > 2100) {
        return 0;
    }

    // Verify month bounds
    if (month < 1 || month > 12) {
        return 0;
    }

    // Days per month table (index 0 is January)
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Check for leap year adjustment in February
    int isLeap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    if (month == 2 && isLeap) {
        daysInMonth[1] = 29;
    }

    // Verify day bounds for the specified month
    if (day < 1 || day > daysInMonth[month - 1]) {
        return 0;
    }

    return 1;
}

// Purpose: Convert string characters to lowercase in place for uniform search
// Parameters: str - string to be modified
// Returns: void
void toLowerCase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = (char)tolower((unsigned char)str[i]);
    }
}

// Purpose: Perform case-insensitive substring search
// Parameters: haystack - target string, needle - search term
// Returns: 1 if needle is found inside haystack, 0 otherwise
int containsKeyword(const char *haystack, const char *needle) {
    char h[MAX_LINE_LEN];
    char n[MAX_LINE_LEN];

    // Copy strings into local buffers to preserve originals
    strncpy(h, haystack, MAX_LINE_LEN - 1);
    h[MAX_LINE_LEN - 1] = '\0';
    strncpy(n, needle, MAX_LINE_LEN - 1);
    n[MAX_LINE_LEN - 1] = '\0';

    // Lowercase both for case-insensitive matching
    toLowerCase(h);
    toLowerCase(n);

    // strstr returns non-NULL pointer if needle is found
    return (strstr(h, n) != NULL) ? 1 : 0;
}

// Purpose: Search for a report by its ID and return its index
// Parameters: id - string ID to find
// Returns: 0-based array index, or -1 if not found
int findReportIndex(const char *id) {
    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

// Purpose: Calculate the next sequential report ID based on existing records
// Parameters: none
// Returns: Next available integer ID
int getNextId(void) {
    int maxId = 0;
    for (int i = 0; i < reportCount; i++) {
        int currentId = atoi(reports[i].id);
        if (currentId > maxId) {
            maxId = currentId;
        }
    }
    return maxId + 1;
}

// Purpose: Check if a given file already exists on disk
// Parameters: filename - path to file
// Returns: 1 if file exists, 0 otherwise
int fileExists(const char *filename) {
    FILE *fp = fopen(filename, "r"); // Attempt to open in read mode
    if (fp != NULL) {
        fclose(fp); // Close if opened successfully
        return 1;
    }
    return 0;
}

// Purpose: Write a repeated separator character followed by newline using fputc
// Parameters: fp - destination file pointer, c - character, len - repetition count
// Returns: void
void drawSeparator(FILE *fp, char c, int len) {
    for (int i = 0; i < len; i++) {
        // Write each character using standard fputc
        fputc(c, fp);
    }
    // Write terminating newline
    fputc('\n', fp);
}

// Purpose: Clear console screen across Windows and Unix platforms
// Parameters: none
// Returns: void
void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Purpose: Pause console execution until user presses Enter
// Parameters: none
// Returns: void
void pauseProgram(void) {
    if (feof(stdin)) {
        return;
    }
    printf("\nPress Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Purpose: Print table column headers with aligned boundaries
// Parameters: none
// Returns: void
void printTableHeader(void) {
    printf("\n%-4s %-6s %-20s %-20s %-12s %-12s\n",
           "ID", "TYPE", "ITEM", "LOCATION", "DATE", "STATUS");
    printf("---- ------ -------------------- -------------------- ------------ ------------\n");
}

// Purpose: Print a single report row formatted to align with the table header
// Parameters: r - pointer to Report struct
// Returns: void
void displayReportRow(const Report *r) {
    // Truncate display strings if necessary to maintain table integrity
    char itemDisplay[21];
    char locDisplay[21];

    strncpy(itemDisplay, r->item, 20);
    itemDisplay[20] = '\0';
    strncpy(locDisplay, r->location, 20);
    locDisplay[20] = '\0';

    printf("%-4s %-6s %-20s %-20s %-12s %-12s\n",
           r->id, r->type, itemDisplay, locDisplay, r->date, r->status);
}

// ============================================================================
// Core File I/O Persistence
// ============================================================================

// Purpose: Load all reports from text file into memory array on startup
// Parameters: none
// Returns: void
void loadReports(void) {
    FILE *fp = fopen(DATA_FILE, "r"); // Open data file for reading
    if (fp == NULL) {
        // If file does not exist yet, initialize count to 0 and start cleanly
        reportCount = 0;
        return;
    }

    char line[MAX_LINE_LEN];
    reportCount = 0;

    // Read file line-by-line using fgets
    while (fgets(line, sizeof(line), fp) != NULL && reportCount < MAX_REPORTS) {
        Report r;

        // Parse 9 pipe-delimited fields using sscanf format specification
        int fields = sscanf(line,
            "%9[^|]|%9[^|]|%49[^|]|%99[^|]|%49[^|]|%10[^|]|%49[^|]|%11[^|]|%49[^\n]",
            r.id, r.type, r.item, r.description, r.location,
            r.date, r.reportedBy, r.status, r.claimedBy);

        // Ensure all 9 fields parsed successfully before storing in array
        if (fields == 9) {
            reports[reportCount++] = r;
        }
        // Malformed or damaged lines failing 9 fields are skipped safely
    }

    fclose(fp); // Close file stream
}

// Purpose: Rewrite all current memory reports back to persistent text file
// Parameters: none
// Returns: void
void saveAllReports(void) {
    FILE *fp = fopen(DATA_FILE, "w"); // Open for writing (overwrites contents)
    if (fp == NULL) {
        printf("Error: Could not open data file for writing.\n");
        return;
    }

    // Write each active report as a pipe-delimited line
    for (int i = 0; i < reportCount; i++) {
        fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
                reports[i].id,
                reports[i].type,
                reports[i].item,
                reports[i].description,
                reports[i].location,
                reports[i].date,
                reports[i].reportedBy,
                reports[i].status,
                reports[i].claimedBy);
    }

    fclose(fp); // Save and close file stream
}

// ============================================================================
// Report Creation & Viewing Operations
// ============================================================================

// Purpose: Create and append a new LOST or FOUND report to memory and file
// Parameters: none
// Returns: void
void addReport(void) {
    // Check if memory storage is full
    if (reportCount >= MAX_REPORTS) {
        printf("Error: Maximum report capacity reached (%d reports).\n", MAX_REPORTS);
        return;
    }

    Report r;
    char input[MAX_LINE_LEN];

    printf("\n--- Report an Item ---\n");

    // Prompt and validate Report Type: LOST or FOUND
    do {
        printf("Type (LOST/FOUND): ");
        readLine(input, sizeof(input));
        toLowerCase(input);
        if (strcmp(input, "lost") == 0) {
            strcpy(r.type, "LOST");
        } else if (strcmp(input, "found") == 0) {
            strcpy(r.type, "FOUND");
        } else {
            printf("Invalid type. Please enter either 'LOST' or 'FOUND'.\n");
        }
    } while (strcmp(r.type, "LOST") != 0 && strcmp(r.type, "FOUND") != 0);

    // Prompt and validate Item Name
    do {
        printf("Item name: ");
        readLine(r.item, sizeof(r.item));
        if (strlen(r.item) == 0) {
            printf("Item name cannot be empty.\n");
        }
    } while (strlen(r.item) == 0);
    sanitizePipe(r.item);

    // Prompt for Description (optional but encouraged)
    printf("Description: ");
    readLine(r.description, sizeof(r.description));
    if (strlen(r.description) == 0) {
        strcpy(r.description, "No additional description provided.");
    }
    sanitizePipe(r.description);

    // Prompt and validate Campus Location
    do {
        printf("Location (e.g., Library, Cafeteria, Gym): ");
        readLine(r.location, sizeof(r.location));
        if (strlen(r.location) == 0) {
            printf("Location cannot be empty.\n");
        }
    } while (strlen(r.location) == 0);
    sanitizePipe(r.location);

    // Prompt and validate Date (DD/MM/YYYY)
    do {
        printf("Date (DD/MM/YYYY): ");
        readLine(r.date, sizeof(r.date));
        if (!isValidDate(r.date)) {
            printf("Invalid date format or range. Use DD/MM/YYYY with valid calendar dates.\n");
        }
    } while (!isValidDate(r.date));

    // Prompt and validate Reporter Name
    do {
        printf("Reported by (Full Name): ");
        readLine(r.reportedBy, sizeof(r.reportedBy));
        if (strlen(r.reportedBy) == 0) {
            printf("Reporter name cannot be empty.\n");
        }
    } while (strlen(r.reportedBy) == 0);
    sanitizePipe(r.reportedBy);

    // Auto-assign ID, initial Unclaimed status, and N/A claimant
    sprintf(r.id, "%d", getNextId());
    strcpy(r.status, "Unclaimed");
    strcpy(r.claimedBy, "N/A");

    // Open data file in append mode to add record to disk
    FILE *fp = fopen(DATA_FILE, "a"); // Append mode "a"
    if (fp == NULL) {
        printf("Error: Could not open data file for saving.\n");
        return;
    }

    // Write record to file
    fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
            r.id, r.type, r.item, r.description, r.location,
            r.date, r.reportedBy, r.status, r.claimedBy);

    fclose(fp); // Flush and close file

    // Add report into memory array
    reports[reportCount++] = r;

    printf("\n[SUCCESS] Report added successfully! Assigned Report ID: %s\n", r.id);

    // Useful Feature: Immediate match check when logging an item
    // If reporting a LOST item, search existing FOUND items, and vice versa
    int matchCount = 0;
    const char *oppType = (strcmp(r.type, "LOST") == 0) ? "FOUND" : "LOST";

    for (int i = 0; i < reportCount - 1; i++) {
        // Compare with opposite type that is still Unclaimed
        if (strcmp(reports[i].type, oppType) == 0 && strcmp(reports[i].status, "Unclaimed") == 0) {
            if (containsKeyword(reports[i].item, r.item) || containsKeyword(r.item, reports[i].item)) {
                if (matchCount == 0) {
                    printf("\n------------------------------------------------------------\n");
                    printf("[!] SMART MATCH ALERT: Potential matching items found!\n");
                    printf("------------------------------------------------------------\n");
                }
                matchCount++;
                printf(" -> ID %-3s | %-5s | %-15s | Location: %-15s | Date: %s\n",
                       reports[i].id, reports[i].type, reports[i].item,
                       reports[i].location, reports[i].date);
            }
        }
    }

    if (matchCount > 0) {
        printf("Please review these existing reports to help reunite this item!\n");
    }
}

// Purpose: Display all recorded reports in an aligned tabular view
// Parameters: none
// Returns: void
void viewAllReports(void) {
    if (reportCount == 0) {
        printf("\nNo reports found in the system.\n");
        return;
    }

    printTableHeader();
    int unclaimed = 0;
    int claimed = 0;
    int returned = 0;

    // Display each report and calculate status tallies
    for (int i = 0; i < reportCount; i++) {
        displayReportRow(&reports[i]);
        if (strcmp(reports[i].status, "Unclaimed") == 0) {
            unclaimed++;
        } else if (strcmp(reports[i].status, "Claimed") == 0) {
            claimed++;
        } else if (strcmp(reports[i].status, "Returned") == 0) {
            returned++;
        }
    }

    printf("\nTotal Reports: %d | Unclaimed: %d | Claimed: %d | Returned: %d\n",
           reportCount, unclaimed, claimed, returned);
}

// Purpose: Filter and display reports matching a selected status
// Parameters: none
// Returns: void
void viewByStatus(void) {
    if (reportCount == 0) {
        printf("\nNo reports found in the system.\n");
        return;
    }

    char input[MAX_LINE_LEN];
    char targetStatus[MAX_STATUS_LEN];

    printf("\nFilter by Status (1: Unclaimed, 2: Claimed, 3: Returned): ");
    readLine(input, sizeof(input));

    // Allow user to enter number or word
    if (strcmp(input, "1") == 0 || containsKeyword(input, "unclaim")) {
        strcpy(targetStatus, "Unclaimed");
    } else if (strcmp(input, "2") == 0 || containsKeyword(input, "claim")) {
        strcpy(targetStatus, "Claimed");
    } else if (strcmp(input, "3") == 0 || containsKeyword(input, "return")) {
        strcpy(targetStatus, "Returned");
    } else {
        printf("Invalid status selection.\n");
        return;
    }

    printTableHeader();
    int count = 0;

    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].status, targetStatus) == 0) {
            displayReportRow(&reports[i]);
            count++;
        }
    }

    if (count == 0) {
        printf("No reports currently marked as '%s'.\n", targetStatus);
    } else {
        printf("\nTotal matching '%s' reports: %d\n", targetStatus, count);
    }
}

// Purpose: Search reports across Item, Description, and Location fields
// Parameters: none
// Returns: void
void searchByKeyword(void) {
    if (reportCount == 0) {
        printf("\nNo reports to search.\n");
        return;
    }

    char keyword[MAX_LINE_LEN];
    printf("\nEnter search keyword: ");
    readLine(keyword, sizeof(keyword));

    if (strlen(keyword) == 0) {
        printf("Keyword cannot be empty.\n");
        return;
    }

    printTableHeader();
    int count = 0;

    // Search across item name, detailed description, and campus location
    for (int i = 0; i < reportCount; i++) {
        if (containsKeyword(reports[i].item, keyword) ||
            containsKeyword(reports[i].description, keyword) ||
            containsKeyword(reports[i].location, keyword)) {
            displayReportRow(&reports[i]);
            count++;
        }
    }

    if (count == 0) {
        printf("No reports found matching '%s'.\n", keyword);
    } else {
        printf("\nTotal matching reports: %d\n", count);
    }
}

// ============================================================================
// Useful Added Features
// ============================================================================

// Purpose: Automatically detect and display potential matches between LOST and FOUND reports
// Parameters: none
// Returns: void
void findPotentialMatches(void) {
    if (reportCount == 0) {
        printf("\nNo reports available for match detection.\n");
        return;
    }

    printf("\n====================================================================\n");
    printf("                SMART MATCH FINDER: LOST vs. FOUND\n");
    printf("====================================================================\n");

    int matchFound = 0;

    // Iterate through all LOST items that are still Unclaimed
    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].type, "LOST") != 0 || strcmp(reports[i].status, "Unclaimed") != 0) {
            continue;
        }

        // Compare against FOUND items that are still Unclaimed
        for (int j = 0; j < reportCount; j++) {
            if (strcmp(reports[j].type, "FOUND") != 0 || strcmp(reports[j].status, "Unclaimed") != 0) {
                continue;
            }

            // Check if item names or descriptions match
            int itemMatch = containsKeyword(reports[i].item, reports[j].item) ||
                            containsKeyword(reports[j].item, reports[i].item);
            int descMatch = containsKeyword(reports[i].description, reports[j].item) ||
                            containsKeyword(reports[j].description, reports[i].item);
            int locMatch = containsKeyword(reports[i].location, reports[j].location) ||
                           containsKeyword(reports[j].location, reports[i].location);

            if (itemMatch || descMatch) {
                matchFound++;
                printf("\n[Match #%d]\n", matchFound);
                printf("  LOST Item  [ID %s]: %s | Reported By: %s | Date: %s\n",
                       reports[i].id, reports[i].item, reports[i].reportedBy, reports[i].date);
                printf("  Location: %s | Description: %s\n",
                       reports[i].location, reports[i].description);
                printf("  ------------------------------------------------------------\n");
                printf("  FOUND Item [ID %s]: %s | Reported By: %s | Date: %s\n",
                       reports[j].id, reports[j].item, reports[j].reportedBy, reports[j].date);
                printf("  Location: %s | Description: %s\n",
                       reports[j].location, reports[j].description);

                if (locMatch) {
                    printf("  >> Strong Match: Both reports share the same location (%s)!\n", reports[i].location);
                }
            }
        }
    }

    if (matchFound == 0) {
        printf("\nNo potential matches detected among current unclaimed items.\n");
    } else {
        printf("\nTotal potential pair match(es) detected: %d\n", matchFound);
    }
}

// Purpose: Display statistical analysis and campus recovery metrics
// Parameters: none
// Returns: void
void viewStatistics(void) {
    if (reportCount == 0) {
        printf("\nNo data available to generate statistics.\n");
        return;
    }

    int lostCount = 0;
    int foundCount = 0;
    int unclaimedCount = 0;
    int claimedCount = 0;
    int returnedCount = 0;

    // Collect tallies
    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].type, "LOST") == 0) lostCount++;
        else if (strcmp(reports[i].type, "FOUND") == 0) foundCount++;

        if (strcmp(reports[i].status, "Unclaimed") == 0) unclaimedCount++;
        else if (strcmp(reports[i].status, "Claimed") == 0) claimedCount++;
        else if (strcmp(reports[i].status, "Returned") == 0) returnedCount++;
    }

    // Calculate resolution recovery rate percentage
    int resolved = claimedCount + returnedCount;
    double resolutionRate = (reportCount > 0) ? ((double)resolved / reportCount) * 100.0 : 0.0;

    printf("\n========================================\n");
    printf("     CAMPUS LOST & FOUND STATISTICS\n");
    printf("========================================\n");
    printf(" Total Reports Logged    : %d\n", reportCount);
    printf("   - Lost Item Reports   : %d (%.1f%%)\n", lostCount, reportCount ? ((double)lostCount/reportCount)*100.0 : 0.0);
    printf("   - Found Item Reports  : %d (%.1f%%)\n", foundCount, reportCount ? ((double)foundCount/reportCount)*100.0 : 0.0);
    printf("----------------------------------------\n");
    printf(" Resolution Status:\n");
    printf("   - Unclaimed Items     : %d\n", unclaimedCount);
    printf("   - Claimed Items       : %d\n", claimedCount);
    printf("   - Returned to Owner   : %d\n", returnedCount);
    printf("----------------------------------------\n");
    printf(" Overall Resolution Rate : %.1f%%\n", resolutionRate);
    printf("========================================\n");
}

// Purpose: Display full detailed information for a single report card
// Parameters: none
// Returns: void
void viewReportDetails(void) {
    if (reportCount == 0) {
        printf("\nNo reports available to view.\n");
        return;
    }

    char id[MAX_ID_LEN];
    printf("\nEnter Report ID to inspect: ");
    readLine(id, sizeof(id));

    int idx = findReportIndex(id);
    if (idx == -1) {
        printf("Report ID '%s' not found.\n", id);
        return;
    }

    const Report *r = &reports[idx];
    printf("\n========================================\n");
    printf("         REPORT DETAILS [ID: %s]\n", r->id);
    printf("========================================\n");
    printf(" Type        : %s\n", r->type);
    printf(" Item Name   : %s\n", r->item);
    printf(" Description : %s\n", r->description);
    printf(" Location    : %s\n", r->location);
    printf(" Date        : %s\n", r->date);
    printf(" Reported By : %s\n", r->reportedBy);
    printf(" Status      : %s\n", r->status);
    printf(" Claimed By  : %s\n", r->claimedBy);
    printf("========================================\n");
}

// Purpose: Filter reports exclusively by category (LOST vs FOUND)
// Parameters: none
// Returns: void
void filterByCategory(void) {
    if (reportCount == 0) {
        printf("\nNo reports available to filter.\n");
        return;
    }

    char input[MAX_LINE_LEN];
    char targetCategory[MAX_TYPE_LEN];

    printf("\nFilter by Category (1: LOST only, 2: FOUND only): ");
    readLine(input, sizeof(input));

    if (strcmp(input, "1") == 0 || containsKeyword(input, "lost")) {
        strcpy(targetCategory, "LOST");
    } else if (strcmp(input, "2") == 0 || containsKeyword(input, "found")) {
        strcpy(targetCategory, "FOUND");
    } else {
        printf("Invalid category selection.\n");
        return;
    }

    printTableHeader();
    int count = 0;

    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].type, targetCategory) == 0) {
            displayReportRow(&reports[i]);
            count++;
        }
    }

    if (count == 0) {
        printf("No reports found under '%s'.\n", targetCategory);
    } else {
        printf("\nTotal '%s' reports: %d\n", targetCategory, count);
    }
}

// ============================================================================
// Report Update, Edit & Deletion Operations
// ============================================================================

// Purpose: Update report status to Claimed or Returned and log claimant name
// Parameters: none
// Returns: void
void updateStatus(void) {
    if (reportCount == 0) {
        printf("\nNo reports to update.\n");
        return;
    }

    char id[MAX_ID_LEN];
    printf("\nEnter Report ID to update: ");
    readLine(id, sizeof(id));

    int idx = findReportIndex(id);
    if (idx == -1) {
        printf("Report ID '%s' not found.\n", id);
        return;
    }

    printf("Current Status: %s | Claimed By: %s\n",
           reports[idx].status, reports[idx].claimedBy);

    // Prevent changing if already marked as Returned
    if (strcmp(reports[idx].status, "Returned") == 0) {
        printf("This report has already been marked as Returned and closed.\n");
        return;
    }

    char input[MAX_LINE_LEN];
    char newStatus[MAX_STATUS_LEN];

    // Prompt for new status
    do {
        printf("Enter new status (1: Claimed, 2: Returned): ");
        readLine(input, sizeof(input));
        if (strcmp(input, "1") == 0 || containsKeyword(input, "claim")) {
            strcpy(newStatus, "Claimed");
        } else if (strcmp(input, "2") == 0 || containsKeyword(input, "return")) {
            strcpy(newStatus, "Returned");
        } else {
            printf("Invalid selection. Choose Claimed or Returned.\n");
        }
    } while (strcmp(newStatus, "Claimed") != 0 && strcmp(newStatus, "Returned") != 0);

    // Prompt for claimant name
    char claimant[MAX_NAME_LEN];
    do {
        printf("Enter Claimant / Recipient Full Name: ");
        readLine(claimant, sizeof(claimant));
        if (strlen(claimant) == 0) {
            printf("Claimant name cannot be empty.\n");
        }
    } while (strlen(claimant) == 0);
    sanitizePipe(claimant);

    // Update in-memory record
    strcpy(reports[idx].status, newStatus);
    strcpy(reports[idx].claimedBy, claimant);

    // Rewrite data file to persist changes
    saveAllReports();

    printf("\n[SUCCESS] Report ID %s status updated to '%s' (Claimed by: %s).\n",
           id, newStatus, claimant);
}

// Purpose: Edit details of an existing report while keeping current values if left blank
// Parameters: none
// Returns: void
void editReport(void) {
    if (reportCount == 0) {
        printf("\nNo reports to edit.\n");
        return;
    }

    char id[MAX_ID_LEN];
    printf("\nEnter Report ID to edit: ");
    readLine(id, sizeof(id));

    int idx = findReportIndex(id);
    if (idx == -1) {
        printf("Report ID '%s' not found.\n", id);
        return;
    }

    Report *r = &reports[idx];
    char input[MAX_LINE_LEN];

    printf("\nEditing Report ID %s (Press Enter to keep current value)\n", id);

    // Edit Type
    printf("Type [%s]: ", r->type);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        toLowerCase(input);
        if (strcmp(input, "lost") == 0) {
            strcpy(r->type, "LOST");
        } else if (strcmp(input, "found") == 0) {
            strcpy(r->type, "FOUND");
        } else {
            printf("Invalid type, keeping current value.\n");
        }
    }

    // Edit Item Name
    printf("Item [%s]: ", r->item);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->item, input, MAX_ITEM_LEN - 1);
        r->item[MAX_ITEM_LEN - 1] = '\0';
        sanitizePipe(r->item);
    }

    // Edit Description
    printf("Description [%s]: ", r->description);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->description, input, MAX_DESC_LEN - 1);
        r->description[MAX_DESC_LEN - 1] = '\0';
        sanitizePipe(r->description);
    }

    // Edit Location
    printf("Location [%s]: ", r->location);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->location, input, MAX_LOC_LEN - 1);
        r->location[MAX_LOC_LEN - 1] = '\0';
        sanitizePipe(r->location);
    }

    // Edit Date
    printf("Date [%s]: ", r->date);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        if (isValidDate(input)) {
            strncpy(r->date, input, MAX_DATE_LEN - 1);
            r->date[MAX_DATE_LEN - 1] = '\0';
        } else {
            printf("Invalid date format, keeping current value.\n");
        }
    }

    // Edit Reported By
    printf("Reported By [%s]: ", r->reportedBy);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->reportedBy, input, MAX_NAME_LEN - 1);
        r->reportedBy[MAX_NAME_LEN - 1] = '\0';
        sanitizePipe(r->reportedBy);
    }

    // Persist changes to disk
    saveAllReports();

    printf("\n[SUCCESS] Report ID %s updated successfully.\n", id);
}

// Purpose: Delete a report from the system by ID with user confirmation
// Parameters: none
// Returns: void
void deleteReport(void) {
    if (reportCount == 0) {
        printf("\nNo reports to delete.\n");
        return;
    }

    char id[MAX_ID_LEN];
    printf("\nEnter Report ID to delete: ");
    readLine(id, sizeof(id));

    int idx = findReportIndex(id);
    if (idx == -1) {
        printf("Report ID '%s' not found.\n", id);
        return;
    }

    // Show report details before asking for confirmation
    printTableHeader();
    displayReportRow(&reports[idx]);

    char confirm[MAX_LINE_LEN];
    printf("\nAre you sure you want to permanently delete this report? (yes/no): ");
    readLine(confirm, sizeof(confirm));
    toLowerCase(confirm);

    if (strcmp(confirm, "yes") != 0 && strcmp(confirm, "y") != 0) {
        printf("Deletion cancelled. Report remains intact.\n");
        return;
    }

    // Shift memory array elements to remove record
    for (int i = idx; i < reportCount - 1; i++) {
        reports[i] = reports[i + 1];
    }
    reportCount--;

    // Persist changes to disk
    saveAllReports();

    printf("\n[SUCCESS] Report ID %s deleted successfully.\n", id);
}

// ============================================================================
// Text Export Functionality
// ============================================================================

// Purpose: Export formatted reports to a user-specified text file with fputc & fgetc
// Parameters: none
// Returns: void
void exportToTxt(void) {
    if (reportCount == 0) {
        printf("\nNo reports to export.\n");
        return;
    }

    char filename[MAX_LINE_LEN];
    printf("\nEnter export filename (e.g. campus_summary): ");
    readLine(filename, sizeof(filename));

    if (strlen(filename) == 0) {
        printf("Filename cannot be empty.\n");
        return;
    }

    // Automatically append .txt extension if not present
    if (strstr(filename, ".txt") == NULL) {
        strcat(filename, ".txt");
    }

    // Choose export scope filter
    char input[MAX_LINE_LEN];
    int exportFilter = 1; // 1 = All, 2 = Unclaimed Only, 3 = Claimed/Returned

    printf("Export options:\n");
    printf("  1. All Reports\n");
    printf("  2. Bulletin Board (Unclaimed Items Only)\n");
    printf("  3. Resolved Items (Claimed / Returned Only)\n");
    printf("Select option (1-3): ");
    readLine(input, sizeof(input));

    if (strcmp(input, "2") == 0) exportFilter = 2;
    else if (strcmp(input, "3") == 0) exportFilter = 3;

    // Check count of matching reports before writing file
    int matchCount = 0;
    for (int i = 0; i < reportCount; i++) {
        if (exportFilter == 1) matchCount++;
        else if (exportFilter == 2 && strcmp(reports[i].status, "Unclaimed") == 0) matchCount++;
        else if (exportFilter == 3 && strcmp(reports[i].status, "Unclaimed") != 0) matchCount++;
    }

    if (matchCount == 0) {
        printf("No reports match the selected export criteria. File was not created.\n");
        return;
    }

    // Determine file write mode: check if file already exists
    char mode[3] = "w"; // Default to fresh overwrite mode
    if (fileExists(filename)) {
        printf("File '%s' already exists. Choose: (o)verwrite or (a)ppend: ", filename);
        readLine(input, sizeof(input));
        toLowerCase(input);
        if (strcmp(input, "a") == 0 || strcmp(input, "append") == 0) {
            strcpy(mode, "a");
        }
    }

    FILE *fp = fopen(filename, mode); // Open file with designated mode
    if (fp == NULL) {
        printf("Error: Could not open or create export file '%s'.\n", filename);
        return;
    }

    // Write file header and separators using fprintf and drawSeparator (fputc)
    fprintf(fp, "CAMPUS LOST & FOUND EXPORT REPORT\n");
    drawSeparator(fp, '=', 80); // uses fputc inside drawSeparator

    if (exportFilter == 1) {
        fprintf(fp, "SCOPE: ALL REGISTERED REPORTS\n");
    } else if (exportFilter == 2) {
        fprintf(fp, "SCOPE: ACTIVE BULLETIN BOARD (UNCLAIMED ITEMS ONLY)\n");
    } else {
        fprintf(fp, "SCOPE: RESOLVED ITEMS (CLAIMED / RETURNED)\n");
    }
    drawSeparator(fp, '-', 80);

    // Write table column headers
    fprintf(fp, "%-4s %-6s %-20s %-18s %-12s %-12s %-15s\n",
            "ID", "TYPE", "ITEM", "LOCATION", "DATE", "STATUS", "CLAIMED BY");
    drawSeparator(fp, '-', 95);

    // Write matching data rows
    for (int i = 0; i < reportCount; i++) {
        int include = 0;
        if (exportFilter == 1) include = 1;
        else if (exportFilter == 2 && strcmp(reports[i].status, "Unclaimed") == 0) include = 1;
        else if (exportFilter == 3 && strcmp(reports[i].status, "Unclaimed") != 0) include = 1;

        if (include) {
            fprintf(fp, "%-4s %-6s %-20s %-18s %-12s %-12s %-15s\n",
                    reports[i].id,
                    reports[i].type,
                    reports[i].item,
                    reports[i].location,
                    reports[i].date,
                    reports[i].status,
                    reports[i].claimedBy);
        }
    }

    drawSeparator(fp, '=', 80);
    fprintf(fp, "Total Reports Exported: %d\n", matchCount);

    fclose(fp); // Flush and close file

    printf("\n[SUCCESS] Successfully exported %d report(s) to '%s'.\n", matchCount, filename);

    // Character-by-character file preview using fgetc
    printf("Would you like to preview the exported file content? (yes/no): ");
    readLine(input, sizeof(input));
    toLowerCase(input);

    if (strcmp(input, "yes") == 0 || strcmp(input, "y") == 0) {
        FILE *viewFp = fopen(filename, "r"); // Open in read mode
        if (viewFp != NULL) {
            int ch;
            printf("\n--- Start of File Content (%s) ---\n", filename);
            // Read and output character-by-character using standard fgetc
            while ((ch = fgetc(viewFp)) != EOF) {
                putchar(ch);
            }
            printf("--- End of File Content ---\n");
            fclose(viewFp); // Close preview stream
        }
    }
}

// ============================================================================
// Menu System & Program Entry Point
// ============================================================================

// Purpose: Display the formatted main navigation menu
// Parameters: none
// Returns: void
void displayMenu(void) {
    clearScreen();
    printf("========================================\n");
    printf("        CAMPUS LOST & FOUND LOG\n");
    printf("========================================\n");
    printf(" 1. Report an Item (Lost / Found)\n");
    printf(" 2. View All Reports\n");
    printf(" 3. View Reports by Status\n");
    printf(" 4. Search by Keyword\n");
    printf(" 5. Find Potential Matches (Smart Match)\n");
    printf(" 6. View Campus Statistics & Summary\n");
    printf(" 7. View Detailed Report by ID\n");
    printf(" 8. Filter by Category (Lost vs Found)\n");
    printf(" 9. Update Report Status\n");
    printf("10. Edit Report Details\n");
    printf("11. Delete a Report\n");
    printf("12. Export to TXT File\n");
    printf("13. Exit System\n");
    printf("----------------------------------------\n");
    printf("Enter choice: ");
}

// Purpose: Main program entry point and event dispatch loop
// Parameters: argc - argument count, argv - argument vector
// Returns: 0 on normal termination
int main(void) {
    // Load existing records from data file into memory on startup
    loadReports();

    int choice = 0;
    char input[MAX_LINE_LEN];

    // Main interactive loop
    do {
        displayMenu();
        if (!readLine(input, sizeof(input))) {
            printf("\nExiting program...\n");
            break;
        }

        // Validate numeric input
        if (sscanf(input, "%d", &choice) != 1) {
            printf("\nInvalid input. Please enter a number corresponding to the menu (1-13).\n");
            pauseProgram();
            continue;
        }

        // Dispatch selection to designated function
        switch (choice) {
            case 1:
                addReport();
                break;
            case 2:
                viewAllReports();
                break;
            case 3:
                viewByStatus();
                break;
            case 4:
                searchByKeyword();
                break;
            case 5:
                findPotentialMatches();
                break;
            case 6:
                viewStatistics();
                break;
            case 7:
                viewReportDetails();
                break;
            case 8:
                filterByCategory();
                break;
            case 9:
                updateStatus();
                break;
            case 10:
                editReport();
                break;
            case 11:
                deleteReport();
                break;
            case 12:
                exportToTxt();
                break;
            case 13:
                printf("\nThank you for using Campus Lost & Found Log. Goodbye!\n");
                break;
            default:
                printf("\nInvalid choice. Please select an option between 1 and 13.\n");
                break;
        }

        // Pause before clearing screen for next menu display (except on exit)
        if (choice != 13) {
            pauseProgram();
        }

    } while (choice != 13);

    return 0;
}
