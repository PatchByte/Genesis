#include "GenesisEditor/GenesisWidgets.hpp"
#include "imgui.h"
#include "imgui_node_editor.h"

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

        if (ImGui::BeginPopupModal(popupKey.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Please choose an option with your arrow keys and press enter.");
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

} // namespace genesis::widgets