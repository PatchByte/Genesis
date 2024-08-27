#ifndef _GENESISGIT_HPP
#define _GENESISGIT_HPP

namespace genesis::merge
{

    enum class GenesisGitExitCodes : int
    {
        SUCCESS = 0,
        FAILED_TO_RESOLVE = 1,
        ABORTED = 2
    };

} // namespace genesis::merge

#endif // !_GENESISGIT_HPP
