#include <tbaricault/cli.hpp>

#include "entrypoint.hpp"
#include "Project.hpp"


ENTRYPOINT(root)
{
    (void)command;
    auto& out = context.getOut();
    out
        << "Fantastic Framework & Practical PHP" << std::endl
        << 'v' << VERSION << std::endl
        << std::endl
        << "Thanks for using F2P2!" << std::endl
        << "You can type 'f2p2 --help' to get help." << std::endl;
    return 0;
}

int main(int argc, char** argv)
{
    return (tbaricault::cli::run(
        tbaricault::cli::Command()
            .setName(argv[0])
            .setDescription("CLI for F2P2 framework")
            .setCallback(root)
            .add(Project::command()),
        std::span(argv + 1, argc - 1)
    ));
}
