#include <cassert>
#include <cstdlib>
#include <cstdio>
#include "leveldb/db.h"
#include "leveldb/write_batch.h"

const char* DBPATH = "/home/abcdabcd987/test.db";
const int KEYSIZE = 4 * 1024;
const int BATCHSIZE = 1 * 1024 * 1024;

leveldb::DB* db;
leveldb::Status ok;
leveldb::WriteBatch *batch;
leveldb::WriteOptions write_options;
FILE* fp;
char key[KEYSIZE], *value;
bool eof, valid;
int cnt_batch;
int cnt_line, cnt_valid, cnt_write, cnt_fail;

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

void batch_write() {
    ++cnt_write;
    ok = db->Write(write_options, batch);
    if (!ok.ok()) ++cnt_fail;
    delete batch;
    cnt_batch = 0;
}

void insert() {
    getrecord();
    if (!valid) return;
    batch->Put(key, value);
    if (++cnt_batch < BATCHSIZE) return;
    batch_write();
    batch = new leveldb::WriteBatch();
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
    batch = new leveldb::WriteBatch();

    while (!eof) insert();
    batch_write();

    printf("%s\tline: %d\tvalid: %d\twrite: %d\tfail: %d\n", argv[1], cnt_line, cnt_valid, cnt_write, cnt_fail);

    delete db;
    fclose(fp);
    free(value);
}
