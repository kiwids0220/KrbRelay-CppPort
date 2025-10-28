#pragma once

#include "Types.h"
#include <vector>
#include <memory>

#define SECURITY_WIN32
#include <security.h>
#include <sspi.h>

namespace KrbRelay {

/**
 * @brief Manages security buffers for Kerberos authentication
 * 
 * This class provides a C++ wrapper around Windows security buffers,
 * making it easier to work with Kerberos tickets and authentication data.
 */
class KRBRELAY_API SecurityBufferManager {
public:
    SecurityBufferManager();
    ~SecurityBufferManager();

    /**
     * @brief Create a security buffer from raw data
     * @param data Raw buffer data
     * @param size Size of the buffer
     * @param type Type of security buffer
     * @return True if successful
     */
    bool CreateBuffer(const void* data, size_t size, BufferType type);

    /**
     * @brief Create a security buffer from a byte vector
     * @param data Vector containing buffer data
     * @param type Type of security buffer
     * @return True if successful
     */
    bool CreateBuffer(const std::vector<uint8_t>& data, BufferType type);

    /**
     * @brief Get the internal SecBuffer structure
     * @return Pointer to SecBuffer
     */
    SecBuffer* GetSecBuffer();

    /**
     * @brief Get the buffer data as a byte vector
     * @return Vector containing buffer data
     */
    std::vector<uint8_t> GetData() const;

    /**
     * @brief Get buffer size
     * @return Size in bytes
     */
    size_t GetSize() const;

    /**
     * @brief Get buffer type
     * @return Buffer type
     */
    BufferType GetType() const;

    /**
     * @brief Clear the buffer
     */
    void Clear();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Manages security buffer descriptor for multiple buffers
 */
class KRBRELAY_API SecurityBufferDescriptor {
public:
    SecurityBufferDescriptor();
    ~SecurityBufferDescriptor();

    /**
     * @brief Add a buffer to the descriptor
     * @param buffer Security buffer to add
     * @return True if successful
     */
    bool AddBuffer(const SecurityBufferManager& buffer);

    /**
     * @brief Get the internal SecBufferDesc structure
     * @return Pointer to SecBufferDesc
     */
    SecBufferDesc* GetDescriptor();

    /**
     * @brief Get number of buffers
     * @return Number of buffers in descriptor
     */
    size_t GetBufferCount() const;

    /**
     * @brief Clear all buffers
     */
    void Clear();

    /**
     * @brief Convert descriptor to byte array
     * @return Vector containing all buffer data
     */
    std::vector<uint8_t> ToByteArray() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace KrbRelay
