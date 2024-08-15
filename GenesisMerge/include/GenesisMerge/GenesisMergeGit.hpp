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

    enum class GenesisMergeStatus : int
    {
        INVALID = 0,
        UNRESOLVED = 1, // Needs to be resolved
        RESOLVED = 2    // Is resolved
    };

} // namespace genesis::merge

#endif // !_GENESISGIT_HPP
