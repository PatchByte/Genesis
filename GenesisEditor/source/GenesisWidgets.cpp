#include "GenesisEditor/GenesisWidgets.hpp"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace ed = ax::NodeEditor;

namespace genesis::widgets
{

    bool GenesisComboBoxPopupWorkaround::sfRenderPopUpButton(std::string Key, std::string Text, std::vector<std::string> Items, int* ItemsIndex, int StartAtOffset)
    {
        int copiedItemsIndex = *ItemsIndex;
        
        Key = "##PopupWorkaroundItem" + Key;

        if(ImGui::Button(Text.data(), { ImGui::CalcItemWidth(), 0 }))
        {
            ed::Suspend();
            ImGui::OpenPopup(Key.data());
            ed::Resume();
        }

        bool res = false;

        ed::Suspend();
		if (ImGui::BeginPopup(Key.data()))
		{
            for(size_t currentIndex = StartAtOffset; currentIndex < Items.size(); currentIndex++)
            {
                if(ImGui::MenuItem(Items[currentIndex].data()))
                {
                    *ItemsIndex = currentIndex;
                    ImGui::CloseCurrentPopup();
                    res = true;
                    break;
                }
            }

            ImGui::EndPopup();
        }
        ed::Resume();

        return res;
    }

}