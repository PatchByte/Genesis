#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "imgui.h"
#include "imnodes.h"

namespace genesis::editor
{

    GenesisFlowEditor::GenesisFlowEditor()
    {
        AddOperationToFlow(new operations::GenesisFindPatternOperation("E8 ? ? ? ? 90"));
        AddOperationToFlow(new operations::GenesisMathOperation(operations::GenesisMathOperation::Type::ADDITION, 6));
        AddOperationToFlow(new operations::GenesisMathOperation(operations::GenesisMathOperation::Type::ADDITION, 2));
    }

    GenesisFlowEditor::~GenesisFlowEditor()
    {
    }

    void GenesisFlowEditor::Render()
    {

        ImNodes::BeginNodeEditor();

        for(auto currentIterator : m_Operations)
        {
            ImNodes::BeginNode(currentIterator.first);
            RenderNodeOperation(currentIterator.second);
            ImNodes::EndNode();
        }

        for (int i = 0; i < m_Links.size(); ++i)
        {
            const std::pair<int, int> p = m_Links[i];
            // in this case, we just use the array index of the link
            // as the unique identifier
            ImNodes::Link(i, p.first, p.second);
        }

        ImNodes::EndNodeEditor();

        {
            int start_attr, end_attr;
            if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
            {
                m_Links.push_back(std::make_pair(start_attr, end_attr));
            }
        }
    }

    void GenesisFlowEditor::RenderNodeOperation(operations::GenesisBaseOperation* Operation)
    {
        ImNodes::BeginNodeTitleBar();
        ImGui::Text("%s", Operation->GetOperationName().data());
        ImNodes::EndNodeTitleBar();

        GenesisOperationEditorForNodes::sfRenderOperation(Operation);
    }

}