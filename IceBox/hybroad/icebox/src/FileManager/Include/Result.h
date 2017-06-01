#ifndef __ResultCode__H_
#define __ResultCode__H_

#include "DeclareEnum.h"

namespace IBox {

DECLARE_ENUM(Result)
	NoHandle = -1,
	OK = 0,
	User_Unregistered = 30100,
	User_Exists,				 /* User exists */
	User_WrongPassword,
	User_NotOnline,
	Command_NotFound = 30120,
	File_NotFound = 30150,
	DB_InsertFailed = 30200,
	DB_MakeDiffFailed,
	DB_DeleteFailed,
	DB_UpdateFailed,
	File_EncryptFailed = 30250,
	File_DecryptFailed
END_ENUM()

} // namespace IBox

#endif


