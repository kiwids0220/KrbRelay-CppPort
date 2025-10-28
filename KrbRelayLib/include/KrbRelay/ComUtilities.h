#pragma once

#include "Types.h"
#include <string>
#include <vector>
#include <memory>

namespace KrbRelay {

/**
 * @brief COM utilities for OBJREF handling and marshalling
 */
class KRBRELAY_API COMUtilities {
public:
    /**
     * @brief Initialize COM utilities
     */
    COMUtilities();
    ~COMUtilities();

    /**
     * @brief Marshal COM object to OBJREF
     * @param pUnknown IUnknown pointer to marshal
     * @param objrefData Output OBJREF data
     * @return Operation result
     */
    OperationResult MarshalInterface(
        void* pUnknown,
        std::vector<uint8_t>& objrefData
    );

    /**
     * @brief Unmarshal OBJREF to COM object
     * @param objrefData OBJREF data
     * @param ppUnknown Output IUnknown pointer
     * @return Operation result
     */
    OperationResult UnmarshalInterface(
        const std::vector<uint8_t>& objrefData,
        void** ppUnknown
    );

    /**
     * @brief Create moniker string from OBJREF
     * @param objrefData OBJREF data
     * @return Moniker string
     */
    std::string CreateMonikerString(const std::vector<uint8_t>& objrefData);

    /**
     * @brief Parse OBJREF data
     * @param objrefData OBJREF data
     * @param ipid Output IPID
     * @param oxid Output OXID
     * @param oid Output OID
     * @return Operation result
     */
    OperationResult ParseOBJREF(
        const std::vector<uint8_t>& objrefData,
        std::string& ipid,
        uint64_t& oxid,
        uint64_t& oid
    );

    /**
     * @brief Modify OBJREF string bindings
     * @param objrefData OBJREF data (in/out)
     * @param ipAddress New IP address
     * @param port New port
     * @return Operation result
     */
    OperationResult ModifyOBJREFBindings(
        std::vector<uint8_t>& objrefData,
        const std::string& ipAddress,
        uint16_t port
    );

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace KrbRelay
