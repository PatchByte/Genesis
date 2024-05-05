#include "GenesisEditor/GenesisFlowEditor.hpp"
#include "GenesisEditor/GenesisOperationsEditor.hpp"
#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "imgui.h"
#include "imnodes.h"

namespace genesis::editor
{

    GenesisFlowEditor::GenesisFlowEditor(GenesisFlow* Flow):
        m_Flow(Flow),
        m_Links()
    {
        m_Flow->AddOperationToFlow(new operations::GenesisFindPatternOperation("E8 ? ? ? ? 90"));
        m_Flow->AddOperationToFlow(new operations::GenesisMathOperation(operations::GenesisMathOperation::Type::ADDITION, 6));
    }

    GenesisFlowEditor::~GenesisFlowEditor()
    {
        if(m_Flow)
        {
            delete m_Flow;
            m_Flow = nullptr;
        }
    }

    void GenesisFlowEditor::Render()
    {

        ImNodes::BeginNodeEditor();

        m_Flow->ForeachObject([this] (GenesisFlowIndex Index, operations::GenesisBaseOperation* Operation) -> bool {
            ImNodes::BeginNode(Index);
            RenderNodeOperation(Index, Operation);
            ImNodes::EndNode();
            
            return true;
        });

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

    void GenesisFlowEditor::RenderNodeOperation(GenesisFlowIndex Index, operations::GenesisBaseOperation* Operation)
    {
        ImNodes::BeginNodeTitleBar();
        ImGui::Text("%s", Operation->GetOperationName().data());
        ImNodes::EndNodeTitleBar();

        GenesisOperationEditorForNodes::sfRenderOperation(Operation);
    }

}