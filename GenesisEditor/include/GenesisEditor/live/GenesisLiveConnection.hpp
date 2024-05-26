#ifndef _GENESISLIVECONNECTION_HPP
#define _GENESISLIVECONNECTION_HPP

#include "GenesisLiveShared/GenesisLiveRelayConnection.hpp"

namespace genesis::live
{

    class GenesisLiveConnection : public GenesisLiveRelayConnection
    {
    public:
        GenesisLiveConnection();

        inline bool IsAuthed()
        {
            return m_Authed;
        }

        void SetAuthed(bool Authed)
        {
            m_Authed = Authed;
        }

    protected:
        bool m_Authed;
    };

} // namespace genesis::live

#endif // !_GENESISLIVECONNECTION_HPP