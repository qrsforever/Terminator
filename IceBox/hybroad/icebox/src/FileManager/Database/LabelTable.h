#ifndef __LabelTable__H_
#define __LabelTable__H_

#include "DBTable.h"
#include <list>

namespace IBox {

typedef enum {
	SYSTEM_LABLE = 10,
	USER_LABLE = 11
} LabelType;

typedef struct {
	int			id;
    std::string name;
	std::string owners;
	int			type;
} Label;

class LabelTable : public DBTable {
public:
	LabelTable(SQLiteDatabase* db);
	~LabelTable();
    
    bool insert(Label info);
    bool update(std::string name, std::string owners);
    bool remove(std::string name);
	bool labelExists(std::string name); 
	Label get(std::string name);
	std::vector<Label> getLabels(std::string owner);
};

} // namespace IBox

#endif

