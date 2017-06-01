#ifndef __DiskTable__H_
#define __DiskTable__H_

#include "DBTable.h"

namespace IBox {

typedef struct {
	int 		id;
    std::string fsname;
    std::string mntdir;
    std::string type;
    size_t      total;
    size_t      free;
	std::string alias;
	std::string bustype;
	int			backupId;
} DiskInfo;

class DiskTable : public DBTable {
public:
    DiskTable(SQLiteDatabase* db);
    ~DiskTable();

    bool insert(DiskInfo disk);
    bool update(DiskInfo disk);
    bool update(int id, std::string key, int value);
    bool update(int id, std::string key, std::string value);
    bool remove(int id);
    bool diskExists(std::string fsname);
    DiskInfo get(int id);
	int nextId();
};

} // namespace IBox

#endif

