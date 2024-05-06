#ifndef _GENESISFLOWEDITOR_HPP
#define _GENESISFLOWEDITOR_HPP

#include "AshLogger/AshLogger.h"
#include "GenesisShared/GenesisFlow.hpp"
#include "GenesisShared/GenesisOperations.hpp"
#include "imgui.h"

namespace genesis::editor
{

    class GenesisFlowEditor : public GenesisFlow
    {
    public:
        GenesisFlowEditor();
        ~GenesisFlowEditor();

        void Initialize();
        void Render();
        void RenderNodeOperation(operations::GenesisBaseOperation* Operation);
    
        static bool sfGetColorForOperationInformation(const operations::GenesisOperationInformation& Information, ImColor* OutputNormalColor, ImColor* OutputBrightColor);
    private:
        ash::AshLogger m_Logger;
    };

}

#endif // !_GENESISFLOWEDITOR_HPP