#define _CRT_SECURE_NO_WARNINGS
<<<<<<< HEAD
=======

// Standard library headers
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
<<<<<<< HEAD
=======
#include <time.h>
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c

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

<<<<<<< HEAD
=======
// Report record structure
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
typedef struct {
    char id[MAX_ID_LEN];
    char type[MAX_TYPE_LEN];
    char item[MAX_ITEM_LEN];
    char description[MAX_DESC_LEN];
    char location[MAX_LOC_LEN];
    char date[MAX_DATE_LEN];
    char reportedBy[MAX_NAME_LEN];
    char status[MAX_STATUS_LEN];
    char claimedBy[MAX_NAME_LEN];
} Report;

<<<<<<< HEAD
Report reports[MAX_REPORTS];
int reportCount = 0;

void loadReports() {
    FILE *fp = fopen(DATA_FILE, "r");
    if (fp == NULL) return;
=======
// Global array and report counter
Report reports[MAX_REPORTS];
int reportCount = 0;

// Function prototypes
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
void loadReports(void);
void saveAllReports(void);
void addReport(void);
void viewAllReports(void);
void viewByStatus(void);
void searchByKeyword(void);
void findPotentialMatches(void);
void viewStatistics(void);
void viewReportDetails(void);
void filterByCategory(void);
void updateStatus(void);
void editReport(void);
void deleteReport(void);
void exportToTxt(void);
void displayMenu(void);

// Clear remaining characters from stdin buffer
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Read safe line input using fgets and strip newline
int readLine(char *buf, int maxLen) {
    if (fgets(buf, maxLen, stdin) != NULL) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        } else {
            clearInputBuffer();
        }
        return 1;
    } else {
        buf[0] = '\0';
        return 0;
    }
}

// Replace pipe character with hyphen to protect text file delimiter
void sanitizePipe(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '|') {
            str[i] = '-';
        }
    }
}

// Validate date format DD/MM/YYYY and valid calendar range
int isValidDate(const char *date) {
    if (strlen(date) != 10) return 0;
    if (date[2] != '/' || date[5] != '/') return 0;

    int day = 0, month = 0, year = 0;
    if (sscanf(date, "%2d/%2d/%4d", &day, &month, &year) != 3) return 0;
    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;

    // Check days in month including leap year
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int isLeap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    if (month == 2 && isLeap) daysInMonth[1] = 29;

    if (day < 1 || day > daysInMonth[month - 1]) return 0;
    return 1;
}

// Convert string to lowercase for case-insensitive matching
void toLowerCase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = (char)tolower((unsigned char)str[i]);
    }
}

// Check if needle exists inside haystack case-insensitively
int containsKeyword(const char *haystack, const char *needle) {
    char h[MAX_LINE_LEN], n[MAX_LINE_LEN];
    strncpy(h, haystack, MAX_LINE_LEN - 1);
    h[MAX_LINE_LEN - 1] = '\0';
    strncpy(n, needle, MAX_LINE_LEN - 1);
    n[MAX_LINE_LEN - 1] = '\0';
    toLowerCase(h);
    toLowerCase(n);
    return (strstr(h, n) != NULL) ? 1 : 0;
}

// Find report index in memory array by ID
int findReportIndex(const char *id) {
    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].id, id) == 0) return i;
    }
    return -1;
}

// Auto-generate next numeric report ID
int getNextId(void) {
    int maxId = 0;
    for (int i = 0; i < reportCount; i++) {
        int currentId = atoi(reports[i].id);
        if (currentId > maxId) maxId = currentId;
    }
    return maxId + 1;
}

// Check if a file exists on disk
int fileExists(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        fclose(fp);
        return 1;
    }
    return 0;
}

// Write separator line to file using fputc
void drawSeparator(FILE *fp, char c, int len) {
    for (int i = 0; i < len; i++) {
        fputc(c, fp);
    }
    fputc('\n', fp);
}

// Cross-platform screen clearing
void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Pause and wait for Enter key
void pauseProgram(void) {
    if (feof(stdin)) return;
    printf("\nPress Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Print formatted table header
void printTableHeader(void) {
    printf("\n%-4s %-6s %-20s %-20s %-12s %-12s\n",
           "ID", "TYPE", "ITEM", "LOCATION", "DATE", "STATUS");
    printf("---- ------ -------------------- -------------------- ------------ ------------\n");
}

// Display one aligned report row
void displayReportRow(const Report *r) {
    char itemDisplay[21], locDisplay[21];
    strncpy(itemDisplay, r->item, 20);
    itemDisplay[20] = '\0';
    strncpy(locDisplay, r->location, 20);
    locDisplay[20] = '\0';

    printf("%-4s %-6s %-20s %-20s %-12s %-12s\n",
           r->id, r->type, itemDisplay, locDisplay, r->date, r->status);
}

// Load all reports from text file into memory array
void loadReports(void) {
    FILE *fp = fopen(DATA_FILE, "r");
    if (fp == NULL) {
        reportCount = 0;
        return;
    }
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c

    char line[MAX_LINE_LEN];
    reportCount = 0;

<<<<<<< HEAD
    while (fgets(line, sizeof(line), fp) != NULL && reportCount < MAX_REPORTS) {
        Report r;
        int fields = sscanf(line, "%9[^|]|%9[^|]|%49[^|]|%99[^|]|%49[^|]|%10[^|]|%49[^|]|%11[^|]|%49[^\n]",
            r.id, r.type, r.item, r.description, r.location, r.date,
            r.reportedBy, r.status, r.claimedBy);

        if (fields == 9) reports[reportCount++] = r;
=======
    // Parse pipe-delimited records and skip corrupted lines
    while (fgets(line, sizeof(line), fp) != NULL && reportCount < MAX_REPORTS) {
        Report r;
        int fields = sscanf(line,
            "%9[^|]|%9[^|]|%49[^|]|%99[^|]|%49[^|]|%10[^|]|%49[^|]|%11[^|]|%49[^\n]",
            r.id, r.type, r.item, r.description, r.location,
            r.date, r.reportedBy, r.status, r.claimedBy);

        if (fields == 9) {
            reports[reportCount++] = r;
        }
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    }

    fclose(fp);
}

<<<<<<< HEAD
void saveAllReports() {
=======
// Save all in-memory reports back to text file
void saveAllReports(void) {
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    FILE *fp = fopen(DATA_FILE, "w");
    if (fp == NULL) {
        printf("Error: Could not open data file for writing.\n");
        return;
    }

<<<<<<< HEAD
    for (int i = 0; i < reportCount; i++) {
        fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
            reports[i].id, reports[i].type, reports[i].item,
            reports[i].description, reports[i].location, reports[i].date,
            reports[i].reportedBy, reports[i].status, reports[i].claimedBy);
=======
    // Rewrite all active records
    for (int i = 0; i < reportCount; i++) {
        fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
                reports[i].id, reports[i].type, reports[i].item,
                reports[i].description, reports[i].location, reports[i].date,
                reports[i].reportedBy, reports[i].status, reports[i].claimedBy);
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    }

    fclose(fp);
}

<<<<<<< HEAD
int getNextId() {
    int maxId = 0;
    for (int i = 0; i < reportCount; i++) {
        int id = atoi(reports[i].id);
        if (id > maxId) maxId = id;
    }
    return maxId + 1;
}

void readLine(char *buf, int maxLen) {
    if (fgets(buf, maxLen, stdin) != NULL) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    } else {
        buf[0] = '\0';
    }
}

void sanitizePipe(char *str) {
    for (int i = 0; str[i]; i++) if (str[i] == '|') str[i] = '-';
}

int isValidDate(char *date) {
    if (strlen(date) != 10) return 0;
    if (date[2] != '/' || date[5] != '/') return 0;

    int day, month, year;
    if (sscanf(date, "%2d/%2d/%4d", &day, &month, &year) != 3) return 0;

    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;

    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (month == 2 && day == 29) {
        if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) return 1;
        return 0;
    }
    if (day > daysInMonth[month - 1]) return 0;

    return 1;
}

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) str[i] = tolower(str[i]);
}

int containsKeyword(char *haystack, char *needle) {
    char h[MAX_LINE_LEN], n[MAX_LINE_LEN];
    strncpy(h, haystack, MAX_LINE_LEN - 1); h[MAX_LINE_LEN - 1] = '\0';
    strncpy(n, needle, MAX_LINE_LEN - 1); n[MAX_LINE_LEN - 1] = '\0';
    toLowerCase(h); toLowerCase(n);
    return strstr(h, n) != NULL;
}

void addReport() {
    if (reportCount >= MAX_REPORTS) {
        printf("Error: Maximum reports reached.\n");
=======
// Add a new lost or found report and append to file
void addReport(void) {
    if (reportCount >= MAX_REPORTS) {
        printf("Error: Maximum report capacity reached (%d reports).\n", MAX_REPORTS);
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        return;
    }

    Report r;
    char input[MAX_LINE_LEN];

<<<<<<< HEAD
=======
    printf("\n--- Report an Item ---\n");

    // Get report type
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    do {
        printf("Type (LOST/FOUND): ");
        readLine(input, sizeof(input));
        toLowerCase(input);
<<<<<<< HEAD
        if (strcmp(input, "lost") == 0) strcpy(r.type, "LOST");
        else if (strcmp(input, "found") == 0) strcpy(r.type, "FOUND");
        else printf("Invalid. Enter LOST or FOUND.\n");
    } while (strcmp(r.type, "LOST") != 0 && strcmp(r.type, "FOUND") != 0);

=======
        if (strcmp(input, "lost") == 0) {
            strcpy(r.type, "LOST");
        } else if (strcmp(input, "found") == 0) {
            strcpy(r.type, "FOUND");
        } else {
            printf("Invalid type. Please enter either 'LOST' or 'FOUND'.\n");
        }
    } while (strcmp(r.type, "LOST") != 0 && strcmp(r.type, "FOUND") != 0);

    // Get item name
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    do {
        printf("Item name: ");
        readLine(r.item, sizeof(r.item));
        if (strlen(r.item) == 0) printf("Item name cannot be empty.\n");
    } while (strlen(r.item) == 0);
    sanitizePipe(r.item);

<<<<<<< HEAD
    printf("Description: ");
    readLine(r.description, sizeof(r.description));
    sanitizePipe(r.description);

    do {
        printf("Location: ");
=======
    // Get description
    printf("Description: ");
    readLine(r.description, sizeof(r.description));
    if (strlen(r.description) == 0) {
        strcpy(r.description, "No additional description provided.");
    }
    sanitizePipe(r.description);

    // Get location
    do {
        printf("Location (e.g., Library, Cafeteria, Gym): ");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        readLine(r.location, sizeof(r.location));
        if (strlen(r.location) == 0) printf("Location cannot be empty.\n");
    } while (strlen(r.location) == 0);
    sanitizePipe(r.location);

<<<<<<< HEAD
    do {
        printf("Date (DD/MM/YYYY): ");
        readLine(r.date, sizeof(r.date));
        if (!isValidDate(r.date)) printf("Invalid date format or range.\n");
    } while (!isValidDate(r.date));

    do {
        printf("Reported by: ");
        readLine(r.reportedBy, sizeof(r.reportedBy));
        if (strlen(r.reportedBy) == 0) printf("Name cannot be empty.\n");
    } while (strlen(r.reportedBy) == 0);
    sanitizePipe(r.reportedBy);

    sprintf(r.id, "%d", getNextId());
    strcpy(r.status, "Unclaimed");
    strcpy(r.claimedBy, "N/A");

    FILE *fp = fopen(DATA_FILE, "a");
    if (fp == NULL) {
        printf("Error: Could not open data file.\n");
=======
    // Get date
    do {
        printf("Date (DD/MM/YYYY): ");
        readLine(r.date, sizeof(r.date));
        if (!isValidDate(r.date)) {
            printf("Invalid date format or range. Use DD/MM/YYYY with valid calendar dates.\n");
        }
    } while (!isValidDate(r.date));

    // Get reporter name
    do {
        printf("Reported by (Full Name): ");
        readLine(r.reportedBy, sizeof(r.reportedBy));
        if (strlen(r.reportedBy) == 0) printf("Reporter name cannot be empty.\n");
    } while (strlen(r.reportedBy) == 0);
    sanitizePipe(r.reportedBy);

    // Set auto fields
    snprintf(r.id, sizeof(r.id), "%u", (unsigned int)getNextId());
    strcpy(r.status, "Unclaimed");
    strcpy(r.claimedBy, "N/A");

    // Append to file
    FILE *fp = fopen(DATA_FILE, "a");
    if (fp == NULL) {
        printf("Error: Could not open data file for saving.\n");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        return;
    }

    fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
<<<<<<< HEAD
        r.id, r.type, r.item, r.description, r.location, r.date,
        r.reportedBy, r.status, r.claimedBy);

    fclose(fp);
    reports[reportCount++] = r;

    printf("Report added successfully! ID: %s\n", r.id);
}

void displayReportRow(Report *r) {
    printf("%-4s %-6s %-20s %-20s %-12s %-12s\n",
        r->id, r->type, r->item, r->location, r->date, r->status);
}

void printTableHeader() {
    printf("\n%-4s %-6s %-20s %-20s %-12s %-12s\n",
        "ID", "TYPE", "ITEM", "LOCATION", "DATE", "STATUS");
    printf("---- ------ -------------------- -------------------- ------------ ------------\n");
}

void viewAllReports() {
    if (reportCount == 0) {
        printf("No reports found.\n");
=======
            r.id, r.type, r.item, r.description, r.location,
            r.date, r.reportedBy, r.status, r.claimedBy);

    fclose(fp);

    // Add to memory
    reports[reportCount++] = r;
    printf("\n[SUCCESS] Report added successfully! Assigned Report ID: %s\n", r.id);

    // Instant smart match check against opposite type
    int matchCount = 0;
    const char *oppType = (strcmp(r.type, "LOST") == 0) ? "FOUND" : "LOST";

    for (int i = 0; i < reportCount - 1; i++) {
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

// Display all reports in aligned table format
void viewAllReports(void) {
    if (reportCount == 0) {
        printf("\nNo reports found in the system.\n");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
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

<<<<<<< HEAD
    printf("\nTotal: %d | Unclaimed: %d | Claimed: %d | Returned: %d\n",
        reportCount, unclaimed, claimed, returned);
}

void viewByStatus() {
    if (reportCount == 0) {
        printf("No reports found.\n");
        return;
    }

    char input[MAX_LINE_LEN];
    char status[MAX_STATUS_LEN];

    printf("Filter by status (Unclaimed/Claimed/Returned): ");
    readLine(input, sizeof(input));
    toLowerCase(input);

    if (strcmp(input, "unclaimed") == 0) strcpy(status, "Unclaimed");
    else if (strcmp(input, "claimed") == 0) strcpy(status, "Claimed");
    else if (strcmp(input, "returned") == 0) strcpy(status, "Returned");
    else {
        printf("Invalid status.\n");
=======
    printf("\nTotal Reports: %d | Unclaimed: %d | Claimed: %d | Returned: %d\n",
           reportCount, unclaimed, claimed, returned);
}

// Filter and view reports by status
void viewByStatus(void) {
    if (reportCount == 0) {
        printf("\nNo reports found in the system.\n");
        return;
    }

    char input[MAX_LINE_LEN], targetStatus[MAX_STATUS_LEN];
    printf("\nFilter by Status (1: Unclaimed, 2: Claimed, 3: Returned): ");
    readLine(input, sizeof(input));

    if (strcmp(input, "1") == 0 || containsKeyword(input, "unclaim")) {
        strcpy(targetStatus, "Unclaimed");
    } else if (strcmp(input, "2") == 0 || containsKeyword(input, "claim")) {
        strcpy(targetStatus, "Claimed");
    } else if (strcmp(input, "3") == 0 || containsKeyword(input, "return")) {
        strcpy(targetStatus, "Returned");
    } else {
        printf("Invalid status selection.\n");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        return;
    }

    printTableHeader();
    int count = 0;
<<<<<<< HEAD
    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].status, status) == 0) {
=======

    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].status, targetStatus) == 0) {
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
            displayReportRow(&reports[i]);
            count++;
        }
    }

<<<<<<< HEAD
    if (count == 0) printf("No reports with status: %s\n", status);
    else printf("\nTotal matching: %d\n", count);
}

void searchByKeyword() {
    if (reportCount == 0) {
        printf("No reports to search.\n");
=======
    if (count == 0) {
        printf("No reports currently marked as '%s'.\n", targetStatus);
    } else {
        printf("\nTotal matching '%s' reports: %d\n", targetStatus, count);
    }
}

// Search reports by keyword across item, description, and location
void searchByKeyword(void) {
    if (reportCount == 0) {
        printf("\nNo reports to search.\n");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        return;
    }

    char keyword[MAX_LINE_LEN];
<<<<<<< HEAD
    printf("Enter keyword: ");
=======
    printf("\nEnter search keyword: ");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    readLine(keyword, sizeof(keyword));

    if (strlen(keyword) == 0) {
        printf("Keyword cannot be empty.\n");
        return;
    }

    printTableHeader();
    int count = 0;
<<<<<<< HEAD
=======

>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    for (int i = 0; i < reportCount; i++) {
        if (containsKeyword(reports[i].item, keyword) ||
            containsKeyword(reports[i].description, keyword) ||
            containsKeyword(reports[i].location, keyword)) {
            displayReportRow(&reports[i]);
            count++;
        }
    }

<<<<<<< HEAD
    if (count == 0) printf("No matches found for: %s\n", keyword);
    else printf("\nTotal matches: %d\n", count);
}

int findReportIndex(char *id) {
    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].id, id) == 0) return i;
    }
    return -1;
}

void updateStatus() {
    if (reportCount == 0) {
        printf("No reports to update.\n");
=======
    if (count == 0) {
        printf("No reports found matching '%s'.\n", keyword);
    } else {
        printf("\nTotal matching reports: %d\n", count);
    }
}

// Automatically match lost items with found items based on keyword and location
void findPotentialMatches(void) {
    if (reportCount == 0) {
        printf("\nNo reports available for match detection.\n");
        return;
    }

    printf("\n====================================================================\n");
    printf("                SMART MATCH FINDER: LOST vs. FOUND\n");
    printf("====================================================================\n");

    int matchFound = 0;

    // Compare unclaimed LOST reports with unclaimed FOUND reports
    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].type, "LOST") != 0 || strcmp(reports[i].status, "Unclaimed") != 0) {
            continue;
        }

        for (int j = 0; j < reportCount; j++) {
            if (strcmp(reports[j].type, "FOUND") != 0 || strcmp(reports[j].status, "Unclaimed") != 0) {
                continue;
            }

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

// Display summary statistics and recovery metrics
void viewStatistics(void) {
    if (reportCount == 0) {
        printf("\nNo data available to generate statistics.\n");
        return;
    }

    int lostCount = 0, foundCount = 0;
    int unclaimedCount = 0, claimedCount = 0, returnedCount = 0;

    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].type, "LOST") == 0) lostCount++;
        else if (strcmp(reports[i].type, "FOUND") == 0) foundCount++;

        if (strcmp(reports[i].status, "Unclaimed") == 0) unclaimedCount++;
        else if (strcmp(reports[i].status, "Claimed") == 0) claimedCount++;
        else if (strcmp(reports[i].status, "Returned") == 0) returnedCount++;
    }

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

// Display all details of a single report by ID
void viewReportDetails(void) {
    if (reportCount == 0) {
        printf("\nNo reports available to view.\n");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        return;
    }

    char id[MAX_ID_LEN];
<<<<<<< HEAD
    printf("Enter Report ID: ");
=======
    printf("\nEnter Report ID to inspect: ");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    readLine(id, sizeof(id));

    int idx = findReportIndex(id);
    if (idx == -1) {
<<<<<<< HEAD
        printf("Report ID not found.\n");
        return;
    }

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
        toLowerCase(input);
        if (strcmp(input, "claimed") == 0) strcpy(newStatus, "Claimed");
        else if (strcmp(input, "returned") == 0) strcpy(newStatus, "Returned");
        else printf("Invalid. Enter Claimed or Returned.\n");
=======
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

// Filter reports by LOST or FOUND category
void filterByCategory(void) {
    if (reportCount == 0) {
        printf("\nNo reports available to filter.\n");
        return;
    }

    char input[MAX_LINE_LEN], targetCategory[MAX_TYPE_LEN];
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

// Update report status to Claimed or Returned and log claimant name
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

    if (strcmp(reports[idx].status, "Returned") == 0) {
        printf("This report has already been marked as Returned and closed.\n");
        return;
    }

    char input[MAX_LINE_LEN], newStatus[MAX_STATUS_LEN];

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
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    } while (strcmp(newStatus, "Claimed") != 0 && strcmp(newStatus, "Returned") != 0);

    char claimant[MAX_NAME_LEN];
    do {
<<<<<<< HEAD
        printf("Claimed by (name): ");
        readLine(claimant, sizeof(claimant));
        if (strlen(claimant) == 0) printf("Name cannot be empty.\n");
=======
        printf("Enter Claimant / Recipient Full Name: ");
        readLine(claimant, sizeof(claimant));
        if (strlen(claimant) == 0) printf("Claimant name cannot be empty.\n");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    } while (strlen(claimant) == 0);
    sanitizePipe(claimant);

    strcpy(reports[idx].status, newStatus);
    strcpy(reports[idx].claimedBy, claimant);
    saveAllReports();

<<<<<<< HEAD
    printf("Status updated to %s. Claimed by: %s\n", newStatus, claimant);
}

void editReport() {
    if (reportCount == 0) {
        printf("No reports to edit.\n");
=======
    printf("\n[SUCCESS] Report ID %s status updated to '%s' (Claimed by: %s).\n",
           id, newStatus, claimant);
}

// Edit details of an existing report
void editReport(void) {
    if (reportCount == 0) {
        printf("\nNo reports to edit.\n");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        return;
    }

    char id[MAX_ID_LEN];
<<<<<<< HEAD
    printf("Enter Report ID: ");
=======
    printf("\nEnter Report ID to edit: ");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    readLine(id, sizeof(id));

    int idx = findReportIndex(id);
    if (idx == -1) {
<<<<<<< HEAD
        printf("Report ID not found.\n");
=======
        printf("Report ID '%s' not found.\n", id);
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        return;
    }

    Report *r = &reports[idx];
    char input[MAX_LINE_LEN];

<<<<<<< HEAD
    printf("Editing report %s (press Enter to keep current value)\n", id);

=======
    printf("\nEditing Report ID %s (Press Enter to keep current value)\n", id);

    // Edit Type
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    printf("Type [%s]: ", r->type);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        toLowerCase(input);
        if (strcmp(input, "lost") == 0) strcpy(r->type, "LOST");
        else if (strcmp(input, "found") == 0) strcpy(r->type, "FOUND");
<<<<<<< HEAD
        else printf("Invalid type, keeping current.\n");
    }

    printf("Item [%s]: ", r->item);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->item, input, MAX_ITEM_LEN - 1);
        r->item[MAX_ITEM_LEN - 1] = '\0';
        sanitizePipe(r->item);
    }

    printf("Description [%s]: ", r->description);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->description, input, MAX_DESC_LEN - 1);
        r->description[MAX_DESC_LEN - 1] = '\0';
        sanitizePipe(r->description);
    }

    printf("Location [%s]: ", r->location);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->location, input, MAX_LOC_LEN - 1);
        r->location[MAX_LOC_LEN - 1] = '\0';
        sanitizePipe(r->location);
    }

=======
        else printf("Invalid type, keeping current value.\n");
    }

    // Edit Item Name
    printf("Item [%s]: ", r->item);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        snprintf(r->item, sizeof(r->item), "%.*s", (int)sizeof(r->item) - 1, input);
        sanitizePipe(r->item);
    }

    // Edit Description
    printf("Description [%s]: ", r->description);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        snprintf(r->description, sizeof(r->description), "%.*s", (int)sizeof(r->description) - 1, input);
        sanitizePipe(r->description);
    }

    // Edit Location
    printf("Location [%s]: ", r->location);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        snprintf(r->location, sizeof(r->location), "%.*s", (int)sizeof(r->location) - 1, input);
        sanitizePipe(r->location);
    }

    // Edit Date
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    printf("Date [%s]: ", r->date);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        if (isValidDate(input)) {
<<<<<<< HEAD
            strncpy(r->date, input, MAX_DATE_LEN - 1);
            r->date[MAX_DATE_LEN - 1] = '\0';
        } else {
            printf("Invalid date, keeping current.\n");
        }
    }

    printf("Reported by [%s]: ", r->reportedBy);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->reportedBy, input, MAX_NAME_LEN - 1);
        r->reportedBy[MAX_NAME_LEN - 1] = '\0';
=======
            snprintf(r->date, sizeof(r->date), "%.*s", (int)sizeof(r->date) - 1, input);
        } else {
            printf("Invalid date format, keeping current value.\n");
        }
    }

    // Edit Reported By
    printf("Reported By [%s]: ", r->reportedBy);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        snprintf(r->reportedBy, sizeof(r->reportedBy), "%.*s", (int)sizeof(r->reportedBy) - 1, input);
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        sanitizePipe(r->reportedBy);
    }

    saveAllReports();
<<<<<<< HEAD
    printf("Report updated successfully.\n");
}

void deleteReport() {
    if (reportCount == 0) {
        printf("No reports to delete.\n");
=======
    printf("\n[SUCCESS] Report ID %s updated successfully.\n", id);
}

// Delete report from array and update text file
void deleteReport(void) {
    if (reportCount == 0) {
        printf("\nNo reports to delete.\n");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        return;
    }

    char id[MAX_ID_LEN];
<<<<<<< HEAD
    printf("Enter Report ID: ");
=======
    printf("\nEnter Report ID to delete: ");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    readLine(id, sizeof(id));

    int idx = findReportIndex(id);
    if (idx == -1) {
<<<<<<< HEAD
        printf("Report ID not found.\n");
=======
        printf("Report ID '%s' not found.\n", id);
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        return;
    }

    printTableHeader();
    displayReportRow(&reports[idx]);

    char confirm[MAX_LINE_LEN];
<<<<<<< HEAD
    printf("\nDelete this report? (yes/no): ");
=======
    printf("\nAre you sure you want to permanently delete this report? (yes/no): ");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    readLine(confirm, sizeof(confirm));
    toLowerCase(confirm);

    if (strcmp(confirm, "yes") != 0 && strcmp(confirm, "y") != 0) {
<<<<<<< HEAD
        printf("Deletion cancelled.\n");
        return;
    }

    for (int i = idx; i < reportCount - 1; i++) reports[i] = reports[i + 1];
    reportCount--;

    saveAllReports();
    printf("Report deleted.\n");
}

int fileExists(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        fclose(fp);
        return 1;
    }
    return 0;
}

void drawSeparator(FILE *fp, char c, int len) {
    for (int i = 0; i < len; i++) fputc(c, fp);
    fputc('\n', fp);
}

void exportToTxt() {
    if (reportCount == 0) {
        printf("No reports to export.\n");
=======
        printf("Deletion cancelled. Report remains intact.\n");
        return;
    }

    // Shift elements left
    for (int i = idx; i < reportCount - 1; i++) {
        reports[i] = reports[i + 1];
    }
    reportCount--;

    saveAllReports();
    printf("\n[SUCCESS] Report ID %s deleted successfully.\n", id);
}

// Export formatted reports to a text file
void exportToTxt(void) {
    if (reportCount == 0) {
        printf("\nNo reports to export.\n");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        return;
    }

    char filename[MAX_LINE_LEN];
<<<<<<< HEAD
    printf("Export filename (without .txt): ");
=======
    printf("\nEnter export filename (e.g. campus_summary): ");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    readLine(filename, sizeof(filename));

    if (strlen(filename) == 0) {
        printf("Filename cannot be empty.\n");
        return;
    }

<<<<<<< HEAD
    if (strstr(filename, ".txt") == NULL) strcat(filename, ".txt");

    char input[MAX_LINE_LEN];
    int exportUnclaimedOnly = 0;
    printf("Export (1) All reports or (2) Unclaimed only? ");
    readLine(input, sizeof(input));
    if (strcmp(input, "2") == 0) exportUnclaimedOnly = 1;

    int matchCount = 0;
    for (int i = 0; i < reportCount; i++) {
        if (!exportUnclaimedOnly || strcmp(reports[i].status, "Unclaimed") == 0) matchCount++;
    }

    if (matchCount == 0) {
        printf("No reports match the selected filter.\n");
=======
    if (strstr(filename, ".txt") == NULL) {
        strcat(filename, ".txt");
    }

    char input[MAX_LINE_LEN];
    int exportFilter = 1;

    printf("Export options:\n");
    printf("  1. All Reports\n");
    printf("  2. Bulletin Board (Unclaimed Items Only)\n");
    printf("  3. Resolved Items (Claimed / Returned Only)\n");
    printf("Select option (1-3): ");
    readLine(input, sizeof(input));

    if (strcmp(input, "2") == 0) exportFilter = 2;
    else if (strcmp(input, "3") == 0) exportFilter = 3;

    int matchCount = 0;
    for (int i = 0; i < reportCount; i++) {
        if (exportFilter == 1) matchCount++;
        else if (exportFilter == 2 && strcmp(reports[i].status, "Unclaimed") == 0) matchCount++;
        else if (exportFilter == 3 && strcmp(reports[i].status, "Unclaimed") != 0) matchCount++;
    }

    if (matchCount == 0) {
        printf("No reports match the selected export criteria. File was not created.\n");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        return;
    }

    char mode[3] = "w";
    if (fileExists(filename)) {
<<<<<<< HEAD
        printf("File '%s' already exists. (o)verwrite or (a)ppend? ", filename);
        readLine(input, sizeof(input));
        toLowerCase(input);
        if (strcmp(input, "a") == 0 || strcmp(input, "append") == 0) strcpy(mode, "a");
=======
        printf("File '%s' already exists. Choose: (o)verwrite or (a)ppend: ", filename);
        readLine(input, sizeof(input));
        toLowerCase(input);
        if (strcmp(input, "a") == 0 || strcmp(input, "append") == 0) {
            strcpy(mode, "a");
        }
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    }

    FILE *fp = fopen(filename, mode);
    if (fp == NULL) {
<<<<<<< HEAD
        printf("Error: Could not create export file.\n");
        return;
    }

    fprintf(fp, "CAMPUS LOST & FOUND REPORT\n");
    drawSeparator(fp, '=', 60);

    if (exportUnclaimedOnly) {
        fprintf(fp, "BULLETIN BOARD - UNCLAIMED ITEMS\n");
        drawSeparator(fp, '-', 60);
    } else {
        fprintf(fp, "ALL REPORTS\n");
        drawSeparator(fp, '-', 60);
    }

    fprintf(fp, "%-4s %-6s %-25s %-25s %-12s %-12s %-15s\n",
        "ID", "TYPE", "ITEM", "LOCATION", "DATE", "STATUS", "CLAIMED BY");
    drawSeparator(fp, '-', 100);

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
    fclose(fp);

    printf("Exported %d report(s) to %s\n", matchCount, filename);

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
=======
        printf("Error: Could not open or create export file '%s'.\n", filename);
        return;
    }

    fprintf(fp, "CAMPUS LOST & FOUND EXPORT REPORT\n");
    drawSeparator(fp, '=', 80);

    if (exportFilter == 1) {
        fprintf(fp, "SCOPE: ALL REGISTERED REPORTS\n");
    } else if (exportFilter == 2) {
        fprintf(fp, "SCOPE: ACTIVE BULLETIN BOARD (UNCLAIMED ITEMS ONLY)\n");
    } else {
        fprintf(fp, "SCOPE: RESOLVED ITEMS (CLAIMED / RETURNED)\n");
    }
    drawSeparator(fp, '-', 80);

    fprintf(fp, "%-4s %-6s %-20s %-18s %-12s %-12s %-15s\n",
            "ID", "TYPE", "ITEM", "LOCATION", "DATE", "STATUS", "CLAIMED BY");
    drawSeparator(fp, '-', 95);

    for (int i = 0; i < reportCount; i++) {
        int include = 0;
        if (exportFilter == 1) include = 1;
        else if (exportFilter == 2 && strcmp(reports[i].status, "Unclaimed") == 0) include = 1;
        else if (exportFilter == 3 && strcmp(reports[i].status, "Unclaimed") != 0) include = 1;

        if (include) {
            fprintf(fp, "%-4s %-6s %-20s %-18s %-12s %-12s %-15s\n",
                    reports[i].id, reports[i].type, reports[i].item,
                    reports[i].location, reports[i].date, reports[i].status,
                    reports[i].claimedBy);
        }
    }

    drawSeparator(fp, '=', 80);
    fprintf(fp, "Total Reports Exported: %d\n", matchCount);
    fclose(fp);

    printf("\n[SUCCESS] Successfully exported %d report(s) to '%s'.\n", matchCount, filename);

    // Optional preview using fgetc
    printf("Would you like to preview the exported file content? (yes/no): ");
    readLine(input, sizeof(input));
    toLowerCase(input);

    if (strcmp(input, "yes") == 0 || strcmp(input, "y") == 0) {
        FILE *viewFp = fopen(filename, "r");
        if (viewFp != NULL) {
            int ch;
            printf("\n--- Start of File Content (%s) ---\n", filename);
            while ((ch = fgetc(viewFp)) != EOF) {
                putchar(ch);
            }
            printf("--- End of File Content ---\n");
            fclose(viewFp);
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
        }
    }
}

<<<<<<< HEAD
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

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
=======
// Display main menu
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
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    printf("----------------------------------------\n");
    printf("Enter choice: ");
}

<<<<<<< HEAD
void pause() {
    printf("\nPress Enter to continue...");
    char dummy[MAX_LINE_LEN];
    readLine(dummy, sizeof(dummy));
}

int main() {
    loadReports();

    int choice;
=======
// Program entry point
int main(void) {
    loadReports();

    int choice = 0;
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
    char input[MAX_LINE_LEN];

    do {
        displayMenu();
<<<<<<< HEAD
        readLine(input, sizeof(input));

        if (strlen(input) == 0) {
            printf("\nGoodbye!\n");
=======
        if (!readLine(input, sizeof(input))) {
            printf("\nExiting program...\n");
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
            break;
        }

        if (sscanf(input, "%d", &choice) != 1) {
<<<<<<< HEAD
            printf("Invalid input. Enter a number 1-9.\n");
            pause();
=======
            printf("\nInvalid input. Please enter a number corresponding to the menu (1-13).\n");
            pauseProgram();
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
            continue;
        }

        switch (choice) {
            case 1: addReport(); break;
            case 2: viewAllReports(); break;
            case 3: viewByStatus(); break;
            case 4: searchByKeyword(); break;
<<<<<<< HEAD
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
=======
            case 5: findPotentialMatches(); break;
            case 6: viewStatistics(); break;
            case 7: viewReportDetails(); break;
            case 8: filterByCategory(); break;
            case 9: updateStatus(); break;
            case 10: editReport(); break;
            case 11: deleteReport(); break;
            case 12: exportToTxt(); break;
            case 13: printf("\nThank you for using Campus Lost & Found Log. Goodbye!\n"); break;
            default: printf("\nInvalid choice. Please select an option between 1 and 13.\n"); break;
        }

        if (choice != 13) {
            pauseProgram();
        }

    } while (choice != 13);

    return 0;
}
>>>>>>> 8ca7b3d90c4663ff10c36175b864d2b4ab325c9c
