#include "FileOper.h"
#include "MessageTypes.h"
#include "JsonObject.h"
#include "StringData.h"
#include "NativeHandler.h"

namespace IBox {

FileOper::FileOper(int code, std::string request)
	: mOperCode(code)
	, mOperRequest(request)	
{
		
}	
	
void FileOper::start()
{
	Result result = execute();
	onFinished(result);
}
	
void FileOper::onFinished(Result result)
{
	if (mOperRequest.empty())
		return ;
	JsonObject responseObj(mOperRequest);
	responseObj.add("Action", "FunctionResponse");
	responseObj.add("Response", result == Result::OK ? "Success" : "Failed");

	StringData* strData = new StringData(responseObj.toString().c_str());
	NativeHandler& H = defNativeHandler();
	H.sendMessage(H.obtainMessage(MessageType_Database, MV_DATABASE_RESPONSE, mOperCode, strData));
	strData->safeUnref();	
}	

} /* namespace IBox */

