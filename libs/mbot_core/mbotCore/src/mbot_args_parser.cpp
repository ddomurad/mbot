#include "inc/mbot_args_parser.h"
#include "string.h"

using namespace mbot;


char * __find_value(char **argv, int argc, const char * name)
{
    for(int i=0;i<argc;i++)
    {
        char * arg = argv[i];
        int name_len = strlen(name);
        int arg_len = strlen(arg);

        if(name_len + 2 > arg_len)
            continue;

        if(arg[name_len] != ':')
            continue;

        arg[name_len] = 0;

        char arg_name[64];

        strcpy(arg_name, arg);
        arg[name_len] = ':';

        if(strcmp(arg_name, name) != 0)
            continue;

        return arg + (name_len+1);
    }

    return nullptr;
}

ArgsParser::ArgsParser(int argc, char **argv)
    : argc(argc), argv(argv)
{}

void ArgsParser::addParam(const char *name, const char *desc, bool mandatory)
{
    ArgsParam param = {
        name, desc, mandatory
    };

    params.push_back(param);
}

result ArgsParser::parse()
{
    for(ArgsParam &param : params)
    {
        char * value = __find_value(argv, argc, param.name);
        if(value == nullptr && param.mandatory == true)
            return Error;

        if(value != nullptr)
            param.value = std::string(value);
    }

    return Success;
}

void ArgsParser::printHelp()
{
    printf("Usage:\n");
    printf("--------------------------------------\n");
    for(ArgsParam &param : params)
    {
        printf("[%s] %s\t%s\n",
               param.mandatory == true ? "MAN" : "OPT",
               param.name,
               param.desc);
    }
    printf("--------------------------------------\n");
    printf("Generic example:\n");
    printf("program.exe param1:value1 param2:value2\n");
    printf("--------------------------------------\n");
}

int ArgsParser::readInt(const char *name)
{
    for(ArgsParam &p : params)
    {
        if(p.name == name)
        {
            return atoi(p.value.c_str());
        }
    }

    return 0;
}

std::string ArgsParser::readString(const char *name)
{
    for(ArgsParam &p : params)
    {
        if(p.name == name)
        {
            return p.value;
        }
    }

    return "";
}
