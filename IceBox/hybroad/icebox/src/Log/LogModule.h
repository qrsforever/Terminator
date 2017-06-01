#ifndef _LogModule_H_
#define _LogModule_H_

#define SET_LOGLEVEL_FROM_FILE 1

#ifdef __cplusplus

struct LogModule {
	LogModule(const char*, int&, int&);
	~LogModule();

	const char* m_name;
	int&        m_flag;
	int&        m_level;
	LogModule*  m_next;
};

#if defined(SET_LOGLEVEL_FROM_FILE)
void SetLogLevelFromFile(const char* filename);
#endif

extern "C" {

#endif // __cplusplus

int setModuleFlag(const char*, int);
int clearModuleFlag(const char*, unsigned int);

int setModuleLevel(const char*, int);
int getModuleLevel(const char*);

void setModulesLevel(int, int);
void clearModulesLevel();
char* getModulesNames(char* names, int len);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _LogModule_H_
