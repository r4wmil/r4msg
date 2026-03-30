#ifndef BS_H_
#define BS_H_

typedef struct DBArray {
	int fd;
	size_t block_size;
} DBArray;

typedef struct Database {
	DBArray a_users;
} Database;

extern Database db;

DBArray DBAInit(char* fname, size_t block_size) {
	DBArray dba = {0};
	assert(!mkdir("db", 0755) || errno == EEXIST);
	dba.fd = open(fname, O_WRONLY | O_CREAT | O_APPEND, 0644);
	dba.block_size = block_size;
	off_t offset = lseek(dba.fd, 0, SEEK_END);
	printf("size=%ld\n", offset);
	assert(dba.fd >= 0);
	return dba;
}

void DBAAppend(DBArray* dba, uint8_t* buf) {
	lseek(dba->fd, 0, SEEK_END);
	ssize_t wn = write(dba->fd, buf, dba->block_size);
	assert(wn == dba->block_size);
}

void DBADeinit(DBArray* dba) {
	close(dba->fd);
}

typedef struct Database {
	DBArray a_users;
} Database;

Database db;

void DBInit() {
	db.a_users = DBAInit("db/users.bin", 1024);
}

void DBUserAdd(struct mg_str username, struct mg_str password) {
	// id(8b)
	// username(24b)
	// password_hash(16b)
	// created_timestamp(8b)
	// display_name(128b)
	uint8_t buf[1024] = {0};
	size_t i = 0;
	// TODO: id index
	i += 8;
	memcpy(buf + i, username.buf, username.len);
	i += 24;
	// TODO: other fields
	DBAAppend(&db.a_users, buf);
}

void DBDeinit() {
	DBADeinit(&db.a_users);
}

#define BS_READ(sl_, amount_, out_, err_) \
  do { \
		if (*(err_)) { break; } \
		const size_t amount = (amount_); \
    if (amount > (sl_)->len) { *(err_) = 1; break; } \
    memcpy((out_), (sl_)->buf, amount); \
    (sl_)->buf += amount; \
    (sl_)->len -= amount; \
  } while(0);

#define BS_READ_SLICE(sl_, amount_, out_, err_) \
  do { \
		if (*(err_)) { break; } \
		const size_t amount = (amount_); \
    if (amount > (sl_)->len) { *(err_) = 1; break; } \
		*(out_) = (sl_)->buf; \
    (sl_)->buf += amount; \
    (sl_)->len -= amount; \
  } while(0);

void DBInit();
void DBDeinit();
void DBUserAdd(struct mg_str username, struct mg_str password);

DBArray DBAInit(char* fname, size_t block_size);
void DBADeinit(DBArray* dba);
void DBAAppend(DBArray* dba, uint8_t* buf);

#endif /* BS_H_ */

#ifdef BS_IMPLEMENTATION

Database db;

DBArray DBAInit(char* fname, size_t block_size) {
	DBArray dba = {0};
	assert(!mkdir("db", 0755) || errno == EEXIST);
	dba.fd = open(fname, O_WRONLY | O_CREAT | O_APPEND, 0644);
	dba.block_size = block_size;
	off_t offset = lseek(dba.fd, 0, SEEK_END);
	printf("size=%ld\n", offset);
	assert(dba.fd >= 0);
	return dba;
}

void DBAAppend(DBArray* dba, uint8_t* buf) {
	lseek(dba->fd, 0, SEEK_END);
	ssize_t wn = write(dba->fd, buf, dba->block_size);
	assert(wn == dba->block_size);
}

void DBADeinit(DBArray* dba) {
	close(dba->fd);
}

void DBInit() {
	db.a_users = DBAInit("db/users.bin", 1024);
}

void DBUserAdd(struct mg_str username, struct mg_str password) {
	// id(8b)
	// username(24b)
	// password_hash(16b)
	// created_timestamp(8b)
	// display_name(128b)
	uint8_t buf[1024] = {0};
	size_t i = 0;
	// TODO: id index
	i += 8;
	memcpy(buf + i, username.buf, username.len);
	i += 24;
	// TODO: other fields
	DBAAppend(&db.a_users, buf);
}

void DBDeinit() {
	DBADeinit(&db.a_users);
}

#endif /* BS_IMPLEMENTATION */
