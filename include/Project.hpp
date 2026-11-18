#pragma once


#include <tbaricault/cli.hpp>

#include "entrypoint.hpp"


class Project
{

    public:

        static tbaricault::cli::Command command();

        static ENTRYPOINT(describe);
        static ENTRYPOINT(create);

};
