#include "GenesisEditor/Main.hpp"
#include "GenesisEditor/GenesisEditor.hpp"

int main(int argc, char** argv)
{
    genesis::editor::GenesisEditor* editor = new genesis::editor::GenesisEditor();

    editor->Run();

    delete editor;
    return 0;
}