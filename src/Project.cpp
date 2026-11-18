#include <string>

#include "Project.hpp"


tbaricault::cli::Command Project::command()
{
    return tbaricault::cli::Command()
        .setName("project")
        .setShort('p')
        .setDescription("Subcommand to manage projects")
        .setCallback(Project::describe)
        .add(tbaricault::cli::Command()
            .setName("describe")
            .setShort('d')
            .setDescription("Gives informations about the current project")
            .setCallback(Project::describe))
        .add(tbaricault::cli::Command()
            .setName("create")
            .setShort('c')
            .setDescription("Create a new project")
            .setCallback(Project::create)
            .add(tbaricault::cli::Positional<std::string>()
                .setName("name")
                .setDescription("project name")));
}

ENTRYPOINT(Project::describe)
{
    (void)command;
    auto& out = context.getOut();
    out << "This command is under development and does nothing for now." << std::endl;
    return 0;
}

ENTRYPOINT(Project::create)
{
    (void)command;
    auto& out = context.getOut();
    out << "This command is under development and does nothing for now." << std::endl;
    return 0;
}
