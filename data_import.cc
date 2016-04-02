#include <cassert>
#include <cstdlib>
#include <cstdio>
#include "leveldb/db.h"

const char* DBPATH = "/home/abcdabcd987/test.db";
const int KEYSIZE = 4 * 1024;

leveldb::DB* db;
leveldb::Status ok;
leveldb::WriteOptions write_options;
FILE* fp;
char key[KEYSIZE], *value;
bool eof, valid;
int cnt_line, cnt_valid, cnt_success;

inline bool is_valid_domain_char(const char c) {
    return ('a' <= c && c <= 'z')
        || ('A' <= c && c <= 'Z')
        || ('0' <= c && c <= '9')
        || (c == '.');
}

inline bool is_valid_name_char(const char c) {
    return ('a' <= c && c <= 'z')
        || ('A' <= c && c <= 'Z')
        || ('0' <= c && c <= '9')
        || (c == '+')
        || (c == '-')
        || (c == '.')
        || (c == '_');
}

void getrecord() {
    valid = false;
    size_t len = 0;
    ssize_t read;
    char *pkey = key;
    read = getline(&value, &len, fp);
    if ((eof = read == -1)) return;
    ++cnt_line;

    char *pline = value;
    while (*pline && !is_valid_name_char(*pline)) ++pline;
    while (*pline && is_valid_name_char(*pline)) *pkey++ = *pline++;
    if (!(*pline) || *pline != '@') return;
    *pkey++ = *pline++;
    while (*pline && is_valid_domain_char(*pline)) *pkey++ = *pline++;
    *pkey++ = '\0';

    valid = true;
    ++cnt_valid;
}

void insert() {
    getrecord();
    if (!valid) return;
    ok = db->Put(write_options, key, value);
    if (ok.ok()) ++cnt_success;
}

int main(int argc, char** argv) {
    if (argc != 2) return 1;
    fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "fail to open file\n");
        return 1;
    }

    leveldb::Options options;
    options.create_if_missing = true;
    ok = leveldb::DB::Open(options, DBPATH, &db);
    if (!ok.ok()) {
        fprintf(stderr, "%s\n", ok.ToString().c_str());
        return 1;
    }

    while (!eof) insert();

    printf("%s\t\tline: %d\tvalid: %d\tsuccess: %d\t\n", argv[1], cnt_line, cnt_valid, cnt_success);

    delete db;
    fclose(fp);
    free(value);
}
