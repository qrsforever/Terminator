#ifndef __PermissionManager__H_
#define __PermissionManager__H_

#include <string>

namespace IBox {

class PermissionManager {
public:	
	virtual bool checkAccess(std::string user, std::string path) = 0;
	virtual bool checkDelete(std::string user, std::string path) = 0;
};

} // namespace IBox

#endif