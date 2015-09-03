#ifndef GCL_OPENMESHALGORITHMS_FAIRING_H
#define GCL_OPENMESHALGORITHMS_FAIRING_H
namespace GCL {
class OMesh;

namespace Utilities{


class FairingBase
{
public:
    FairingBase() {}
    virtual ~FairingBase() {}
    virtual bool fairing(OMesh& ) {return true;}

};
}}
#endif // FAIRING_H
