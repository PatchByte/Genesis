#include "GenesisEditor/Main.hpp"
#include "GenesisEditor/GenesisEditor.hpp"
#include "rtc/global.hpp"

int main(int argc, const char** argv)
{
    rtc::InitLogger(rtc::LogLevel::Info);

    genesis::editor::GenesisEditor* editor = new genesis::editor::GenesisEditor();
    editor->Run(argc, argv);
    delete editor;

    return 0;
}
