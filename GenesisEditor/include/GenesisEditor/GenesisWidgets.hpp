#ifndef _GENESISWIDGETS_HPP
#define _GENESISWIDGETS_HPP

#include "imgui.h"
#include <string>
#include <vector>

namespace genesis::widgets
{

    class GenesisComboBoxPopupWorkaround
    {
    public:
        GenesisComboBoxPopupWorkaround();

        inline void SetData(std::string ItemName, std::vector<std::string> Items, int* ItemsIndex, int StartAtOffset = 0)
        {
            m_ItemName = ItemName;
            m_Items = Items;
            m_ItemsIndex = ItemsIndex;
            m_StartAtOffset = StartAtOffset;
        }

        bool RenderButton();
        bool RenderPost();
        bool HasChanged();

    private:
        std::string m_ItemName;
        bool m_TriggerPopup;
        bool m_HasChanged;
        std::vector<std::string> m_Items;
        int* m_ItemsIndex;
        int m_StartAtOffset;
    };

    class GenesisGenericWidgets
    {
    public:
        static bool sfRenderInputTextStlString(std::string Key, std::string* String, ImVec2 Size = { 0, 0 });
    };

} // namespace genesis::widgets

#endif // !_GENESISWIDGETS_HPP