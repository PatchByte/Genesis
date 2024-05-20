#include "GenesisEditor/Main.hpp"
#include "GenesisEditor/GenesisEditor.hpp"

int main(int argc, const char** argv)
{
    genesis::editor::GenesisEditor* editor = new genesis::editor::GenesisEditor();

    editor->Run(argc, argv);

    delete editor;
    return 0;
}