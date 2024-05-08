#ifndef _GENESISNODEBUILDER_HPP
#define _GENESISNODEBUILDER_HPP

// Copied from:
// https://github.com/thedmd/imgui-node-editor/blob/master/examples/blueprints-example/utilities/builders.h
#include "imgui.h"
#include "imgui_node_editor.h"
#include <string>

namespace genesis::utils
{
    class GenesisNodeBuilder
    {
    public:
        GenesisNodeBuilder(ImTextureID texture = nullptr, int textureWidth = 0, int textureHeight = 0);

        void Begin(ax::NodeEditor::NodeId id);
        void End();

        void Header(std::string Title, const ImVec4& color = ImVec4(1, 1, 1, 1));
        void EndHeader();

        void Input(ax::NodeEditor::PinId id);
        void EndInput();

        void Middle();

        void Output(ax::NodeEditor::PinId id);
        void EndOutput();

    private:
        enum class Stage
        {
            Invalid,
            Begin,
            Header,
            Content,
            Input,
            Output,
            Middle,
            End
        };

        bool SetStage(Stage stage);

        void Pin(ax::NodeEditor::PinId id, ax::NodeEditor::PinKind kind);
        void EndPin();

        ImTextureID m_HeaderTextureId;
        int m_HeaderTextureWidth;
        int m_HeaderTextureHeight;
        ax::NodeEditor::NodeId m_CurrentNodeId;
        Stage m_CurrentStage;
        std::string m_HeaderTitle;
        ImU32 m_HeaderColor;
        ImVec2 m_NodeMin;
        ImVec2 m_NodeMax;
        ImVec2 m_HeaderMin;
        ImVec2 m_HeaderMax;
        ImVec2 m_ContentMin;
        ImVec2 m_ContentMax;
        bool m_HasHeader;
    };
} // namespace genesis::utils

#endif // !_GENESISNODEBUILDER_HPP