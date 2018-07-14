#ifndef ROVER_ARGS_PARSER_H
#define ROVER_ARGS_PARSER_H

#include "mbot_defs.h"
#include <vector>
#include <string>

namespace mbot {

struct ArgsParam
{
    const char * name;
    const char * desc;
    bool mandatory;
    std::string value;
};

class ArgsParser
{
private:
    std::vector<ArgsParam> params;
    int argc;
    char ** argv;

public:
    ArgsParser(int argc, char ** argv);
    void addParam(const char *name, const char * desc, bool mandatory);

    result parse();
    void printHelp();

    int readInt(const char *name);
    std::string readString(const char *name);
};

}

#endif //ROVER_ARGS_PARSER_H
