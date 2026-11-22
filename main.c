#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    char type[MAX_TYPE_LEN];
    char item[MAX_ITEM_LEN];
    char description[MAX_DESC_LEN];
    char location[MAX_LOC_LEN];
    char date[MAX_DATE_LEN];
    char reportedBy[MAX_NAME_LEN];
    char status[MAX_STATUS_LEN];
    char claimedBy[MAX_NAME_LEN];
} Report;

Report reports[MAX_REPORTS];
int reportCount = 0;

void loadReports() {
    FILE *fp = fopen(DATA_FILE, "r");
    if (fp == NULL) return;

    char line[MAX_LINE_LEN];
    reportCount = 0;

    while (fgets(line, sizeof(line), fp) != NULL && reportCount < MAX_REPORTS) {
        Report r;
        int fields = sscanf(line, "%9[^|]|%9[^|]|%49[^|]|%99[^|]|%49[^|]|%10[^|]|%49[^|]|%11[^|]|%49[^\n]",
            r.id, r.type, r.item, r.description, r.location, r.date,
            r.reportedBy, r.status, r.claimedBy);

        if (fields == 9) reports[reportCount++] = r;
    }

    fclose(fp);
}

void saveAllReports() {
    FILE *fp = fopen(DATA_FILE, "w");
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

    fclose(fp);
}

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
        return;
    }

    Report r;
    char input[MAX_LINE_LEN];

    do {
        printf("Type (LOST/FOUND): ");
        readLine(input, sizeof(input));
        toLowerCase(input);
        if (strcmp(input, "lost") == 0) strcpy(r.type, "LOST");
        else if (strcmp(input, "found") == 0) strcpy(r.type, "FOUND");
        else printf("Invalid. Enter LOST or FOUND.\n");
    } while (strcmp(r.type, "LOST") != 0 && strcmp(r.type, "FOUND") != 0);

    do {
        printf("Item name: ");
        readLine(r.item, sizeof(r.item));
        if (strlen(r.item) == 0) printf("Item name cannot be empty.\n");
    } while (strlen(r.item) == 0);
    sanitizePipe(r.item);

    printf("Description: ");
    readLine(r.description, sizeof(r.description));
    sanitizePipe(r.description);

    do {
        printf("Location: ");
        readLine(r.location, sizeof(r.location));
        if (strlen(r.location) == 0) printf("Location cannot be empty.\n");
    } while (strlen(r.location) == 0);
    sanitizePipe(r.location);

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
        return;
    }

    fprintf(fp, "%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
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

int findReportIndex(char *id) {
    for (int i = 0; i < reportCount; i++) {
        if (strcmp(reports[i].id, id) == 0) return i;
    }
    return -1;
}

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
    } while (strcmp(newStatus, "Claimed") != 0 && strcmp(newStatus, "Returned") != 0);

    char claimant[MAX_NAME_LEN];
    do {
        printf("Claimed by (name): ");
        readLine(claimant, sizeof(claimant));
        if (strlen(claimant) == 0) printf("Name cannot be empty.\n");
    } while (strlen(claimant) == 0);
    sanitizePipe(claimant);

    strcpy(reports[idx].status, newStatus);
    strcpy(reports[idx].claimedBy, claimant);
    saveAllReports();

    printf("Status updated to %s. Claimed by: %s\n", newStatus, claimant);
}

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

    printf("Editing report %s (press Enter to keep current value)\n", id);

    printf("Type [%s]: ", r->type);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        toLowerCase(input);
        if (strcmp(input, "lost") == 0) strcpy(r->type, "LOST");
        else if (strcmp(input, "found") == 0) strcpy(r->type, "FOUND");
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

    printf("Reported by [%s]: ", r->reportedBy);
    readLine(input, sizeof(input));
    if (strlen(input) > 0) {
        strncpy(r->reportedBy, input, MAX_NAME_LEN - 1);
        r->reportedBy[MAX_NAME_LEN - 1] = '\0';
        sanitizePipe(r->reportedBy);
    }

    saveAllReports();
    printf("Report updated successfully.\n");
}

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

    printTableHeader();
    displayReportRow(&reports[idx]);

    char confirm[MAX_LINE_LEN];
    printf("\nDelete this report? (yes/no): ");
    readLine(confirm, sizeof(confirm));
    toLowerCase(confirm);

    if (strcmp(confirm, "yes") != 0 && strcmp(confirm, "y") != 0) {
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
        return;
    }

    char filename[MAX_LINE_LEN];
    printf("Export filename (without .txt): ");
    readLine(filename, sizeof(filename));

    if (strlen(filename) == 0) {
        printf("Filename cannot be empty.\n");
        return;
    }

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
        return;
    }

    char mode[3] = "w";
    if (fileExists(filename)) {
        printf("File '%s' already exists. (o)verwrite or (a)ppend? ", filename);
        readLine(input, sizeof(input));
        toLowerCase(input);
        if (strcmp(input, "a") == 0 || strcmp(input, "append") == 0) strcpy(mode, "a");
    }

    FILE *fp = fopen(filename, mode);
    if (fp == NULL) {
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
        }
    }
}

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
    printf("----------------------------------------\n");
    printf("Enter choice: ");
}

void pause() {
    printf("\nPress Enter to continue...");
    char dummy[MAX_LINE_LEN];
    readLine(dummy, sizeof(dummy));
}

int main() {
    loadReports();

    int choice;
    char input[MAX_LINE_LEN];

    do {
        displayMenu();
        readLine(input, sizeof(input));

        if (strlen(input) == 0) {
            printf("\nGoodbye!\n");
            break;
        }

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