#ifndef __LoginTable__H_
#define __LoginTable__H_

#include "DBTable.h"
#include <list>

namespace IBox {

typedef struct {
    int         id;
    std::string name;
    std::string datetime;
    std::string jID;
	int			errcode;
} LoginInfo;

class LoginTable : public DBTable {
public:
	LoginTable(SQLiteDatabase* db);
	~LoginTable();
   
    bool insert(LoginInfo info);
};

} // namespace IBox

#endif
