#include "Assertions.h"
#include "LogModule.h"

#include <string.h>
#include <stdio.h>

#if defined(SET_LOGLEVEL_FROM_FILE)
#include "Setting.h"
#include <stdlib.h>
#endif

static struct LogModule* g_moduleListHead = 0;

LogModule::LogModule(const char* name, int& flag, int& level)
	: m_name(name)
	, m_flag(flag)
	, m_level(level)
{
    m_next = g_moduleListHead;
    g_moduleListHead = this;
}

LogModule::~LogModule()
{
}

#if defined(SET_LOGLEVEL_FROM_FILE)

class LogModuleSetting : public IBox::Setting {
public:
    LogModuleSetting(const char* filename);
    ~LogModuleSetting();
    int init();
    int load();
    int show();
};

LogModuleSetting::LogModuleSetting(const char* filename) 
    : Setting(filename)
{
    init();
    load();
    show();
}

LogModuleSetting::~LogModuleSetting()
{

}

int  
LogModuleSetting::init()
{
    struct LogModule* module = g_moduleListHead;
    while (module) {
        add(module->m_name, getModuleLevel(module->m_name));
        module = module->m_next;
    }
    return 0;
}

int 
LogModuleSetting::load()
{
    struct LogModule* module = g_moduleListHead;
    char level[8] = { 0 };
    if (!Setting::load()) {
        while (module) {
            if (!get(module->m_name, level, 8))
                module->m_level = atoi(level);
            module = module->m_next;
        }
    }
    return 0;
}

int 
LogModuleSetting::show()
{
    printf("### LogModules (1:error 2:warning 3:normal 4:verbose):\n");
    struct LogModule* module = g_moduleListHead;
    while (module) {
        printf("  %s=%d\n", module->m_name, module->m_level);
        module = module->m_next;
    }
}

void SetLogLevelFromFile(const char* filename)
{
    LogModuleSetting set(filename);
}

#endif


extern "C" 
int setModuleFlag(const char* name, int flag)
{
    struct LogModule* module = g_moduleListHead;

    while (module) {
        if (!strcmp(module->m_name, name)) {
            module->m_flag |= flag;
            return 0;
        }
        module = module->m_next;
    }

    return -1;
}

extern "C" 
int clearModuleFlag(const char* name, unsigned int flag)
{
    struct LogModule* module = g_moduleListHead;

    while (module) {
        if (!strcmp(module->m_name, name)) {
            module->m_flag &= ~flag;
            return 0;
        }
        module = module->m_next;
    }

    return -1;
}

extern "C" 
int setModuleLevel(const char* name, int level)
{
    struct LogModule* module = g_moduleListHead;

    while (module) {
        if (!strcmp(module->m_name, name)) {
            int old = module->m_level;
            module->m_level = level;
            return old;
        }
        module = module->m_next;
    }

    return -1;
}

extern "C" 
int getModuleLevel(const char* name)
{
    struct LogModule* module = g_moduleListHead;

    while (module) {
        if (!strcmp(module->m_name, name))
            return module->m_level;
        module = module->m_next;
    }

    return -1;
}

extern "C"
void setModulesLevel(int flag, int level)
{
    struct LogModule* module = g_moduleListHead;

    while (module) {
        if (module->m_flag & flag)
            module->m_level = level;
        module = module->m_next;
    }
}

extern "C" 
void clearModulesLevel( )
{
    struct LogModule* module = g_moduleListHead;
    while (module) {
        module->m_level = 0;
        module = module->m_next;
    }
}

extern "C" 
char* getModulesNames(char* names, int len)
{
    if (names && len > 0) {
        int sumlen = 0;
        struct LogModule* module = g_moduleListHead;
        while (module) {
            if (len > sumlen)
                sumlen += snprintf(names + sumlen, len - sumlen, "%s ", module->m_name);
            module = module->m_next;
        }
        names[sumlen] = 0;
    }
    return names;
}

