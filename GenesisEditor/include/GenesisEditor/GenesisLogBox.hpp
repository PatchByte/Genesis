#ifndef _GENESISLOGBOX_HPP
#define _GENESISLOGBOX_HPP

#include "AshLogger/AshLoggerPassage.h"
#include "AshLogger/AshLoggerTag.h"
#include "imgui.h"
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace genesis::utils
{

    class GenesisLogBox
    {
    public:
        GenesisLogBox();
        ~GenesisLogBox();
        
        void Initialize();
        void Render();
        void Clear();

        ash::AshLoggerPassage* CreatePassage();
    private:
        std::map<ash::AshLoggerShortTag, std::pair<ash::AshLoggerTag, ImColor>> m_TagColors;
        std::vector<std::pair<ash::AshLoggerTag, std::string>> m_Logs;
    };

}

#endif // !_GENESISLOGBOX_HPP