#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sqlite3.h>

#define DB_FILE "foodrec.db"
#define EXPIRE_WINDOW_DAYS 3

static sqlite3 *db = NULL;

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s add <name> <YYYY-MM-DD>\n"
        "  %s scan <image> <name>\n"
        "  %s list\n"
        "  %s check\n"
        "  %s suggest\n",
        prog, prog, prog, prog, prog);
}

static int init_db(void) {
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        fprintf(stderr, "Failed to open DB: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    const char *sql =
        "CREATE TABLE IF NOT EXISTS items ("\
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
        "name TEXT,"\
        "best_by INTEGER);";
    char *err = NULL;
    if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) {
        fprintf(stderr, "DB init error: %s\n", err);
        sqlite3_free(err);
        return 1;
    }
    return 0;
}

static time_t parse_date(const char *s) {
    struct tm tm = {0};
    char *ret = strptime(s, "%Y-%m-%d", &tm);
    if (!ret) ret = strptime(s, "%m/%d/%Y", &tm);
    if (!ret) ret = strptime(s, "%d/%m/%Y", &tm);
    if (!ret) return -1;
    tm.tm_isdst = -1;
    return mktime(&tm);
}

static int add_item(const char *name, const char *date_str) {
    time_t t = parse_date(date_str);
    if (t == (time_t)-1) {
        fprintf(stderr, "Could not parse date: %s\n", date_str);
        return 1;
    }
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO items (name, best_by) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "DB prepare error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, (sqlite3_int64)t);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "DB insert error: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    printf("Added %s expiring on %s\n", name, date_str);
    return 0;
}

static void list_items(void) {
    const char *sql = "SELECT id, name, datetime(best_by, 'unixepoch') FROM items ORDER BY best_by;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Query error: %s\n", sqlite3_errmsg(db));
        return;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        const unsigned char *date = sqlite3_column_text(stmt, 2);
        printf("%d\t%s\t%s\n", id, name, date);
    }
    sqlite3_finalize(stmt);
}

static void check_expiring(void) {
    time_t now = time(NULL);
    time_t window = now + EXPIRE_WINDOW_DAYS * 86400;
    const char *sql = "SELECT name, datetime(best_by, 'unixepoch') FROM items WHERE best_by <= ? ORDER BY best_by;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Query error: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int64(stmt, 1, (sqlite3_int64)window);
    printf("Items expiring within %d days:\n", EXPIRE_WINDOW_DAYS);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *name = sqlite3_column_text(stmt, 0);
        const unsigned char *date = sqlite3_column_text(stmt, 1);
        printf("- %s (by %s)\n", name, date);
    }
    sqlite3_finalize(stmt);
}

static int extract_date_from_text(const char *text, char *out, size_t n) {
    for (const char *p = text; *p; ++p) {
        int y,m,d;
        if (sscanf(p, "%4d-%2d-%2d", &y,&m,&d) == 3) {
            snprintf(out, n, "%04d-%02d-%02d", y,m,d);
            return 0;
        }
        if (sscanf(p, "%2d/%2d/%4d", &m,&d,&y) == 3) {
            snprintf(out, n, "%04d-%02d-%02d", y,m,d);
            return 0;
        }
        if (sscanf(p, "%2d-%2d-%4d", &m,&d,&y) == 3) {
            snprintf(out, n, "%04d-%02d-%02d", y,m,d);
            return 0;
        }
    }
    return 1;
}

static int scan_image(const char *file, char *date_out, size_t n) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "tesseract \"%s\" stdout --psm 6 2>/dev/null", file);
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        perror("popen");
        return 1;
    }
    char buf[2048];
    size_t len = fread(buf, 1, sizeof(buf)-1, fp);
    buf[len] = '\0';
    pclose(fp);
    if (extract_date_from_text(buf, date_out, n) != 0) {
        fprintf(stderr, "Could not find date in OCR text\n");
        return 1;
    }
    return 0;
}

static void suggest_recipes(void) {
    printf("Recipe suggestion feature not implemented.\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }
    if (init_db() != 0) return 1;

    const char *cmd = argv[1];
    int rc = 0;
    if (strcmp(cmd, "add") == 0) {
        if (argc != 4) { usage(argv[0]); rc = 1; }
        else rc = add_item(argv[2], argv[3]);
    } else if (strcmp(cmd, "scan") == 0) {
        if (argc != 4) { usage(argv[0]); rc = 1; }
        else {
            char date[32];
            if (scan_image(argv[2], date, sizeof(date)) == 0)
                rc = add_item(argv[3], date);
            else rc = 1;
        }
    } else if (strcmp(cmd, "list") == 0) {
        list_items();
    } else if (strcmp(cmd, "check") == 0) {
        check_expiring();
    } else if (strcmp(cmd, "suggest") == 0) {
        suggest_recipes();
    } else {
        usage(argv[0]);
        rc = 1;
    }

    if (db) sqlite3_close(db);
    return rc;
}

