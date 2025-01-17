#include "GenesisEditor/GenesisWidgets.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_node_editor.h"
#include <string>

namespace ed = ax::NodeEditor;

namespace genesis::widgets
{

    GenesisComboBoxPopupWorkaround::GenesisComboBoxPopupWorkaround() : m_ItemName(""), m_TriggerPopup(false), m_HasChanged(false), m_ItemsIndex(), m_StartAtOffset()
    {
    }

    bool GenesisComboBoxPopupWorkaround::RenderButton()
    {
        if (ImGui::Button(m_Items[*m_ItemsIndex].data(), {ImGui::CalcItemWidth(), 0}))
        {
            m_TriggerPopup = true;
        }

        return true;
    }

    bool GenesisComboBoxPopupWorkaround::RenderPost()
    {
        ed::Suspend();

        std::string popupKey = "Choose ##PopupWorkaroundItem" + m_ItemName;

        if (m_TriggerPopup)
        {
            ImGui::OpenPopup(popupKey.data());
            m_TriggerPopup = false;
        }

        if (ImGui::BeginPopup(popupKey.data(), ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Please choose an option");
            ImGui::Separator();

            for (size_t currentIndex = m_StartAtOffset; currentIndex < m_Items.size(); currentIndex++)
            {
                if (ImGui::Selectable(m_Items[currentIndex].data()))
                {
                    *m_ItemsIndex = currentIndex;
                    ImGui::CloseCurrentPopup();
                    m_HasChanged = true;
                    break;
                }
            }

            if(ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ed::Resume();

        return true;
    }

    bool GenesisComboBoxPopupWorkaround::HasChanged()
    {
        if (m_HasChanged)
        {
            m_HasChanged = false;
            return true;
        }

        return false;
    }

    bool GenesisGenericWidgets::sfRenderInputTextStlString(std::string Key, std::string* String, ImVec2 Size)
    {
        bool res = ImGui::InputTextEx(
            Key.data(), NULL, String->data(), String->capacity() + 1, Size, ImGuiInputTextFlags_CallbackResize,
            [](ImGuiInputTextCallbackData* data) -> int
            {
                if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                {
                    std::string* b = reinterpret_cast<std::string*>(data->UserData);
                    b->resize(data->BufTextLen);
                    data->Buf = b->data();
                }

                return 0;
            },
            String);
        
        return res;
    }

} // namespace genesis::widgets