#ifndef _GENESISWIDGETS_HPP
#define _GENESISWIDGETS_HPP

#include <string>
#include <vector>

namespace genesis::widgets
{

    class GenesisComboBoxPopupWorkaround
    {
    public:
        static bool sfRenderPopUpButton(std::string Key, std::string Text, std::vector<std::string> Items, int* ItemsIndex, int StartAtOffset = 0);
    };

} // namespace genesis::widgets

#endif // !_GENESISWIDGETS_HPP