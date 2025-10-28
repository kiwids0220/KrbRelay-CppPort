#include "KrbRelay/SecurityBuffer.h"
#include <cstring>
#include <stdexcept>

namespace KrbRelay {

// SecurityBufferManager Implementation
class SecurityBufferManager::Impl {
public:
    SecBuffer secBuffer;
    std::vector<uint8_t> data;

    Impl() {
        std::memset(&secBuffer, 0, sizeof(SecBuffer));
    }

    ~Impl() {
        if (secBuffer.pvBuffer && secBuffer.pvBuffer != data.data()) {
            delete[] static_cast<uint8_t*>(secBuffer.pvBuffer);
        }
    }
};

SecurityBufferManager::SecurityBufferManager() 
    : pImpl(std::make_unique<Impl>()) {
}

SecurityBufferManager::~SecurityBufferManager() = default;

bool SecurityBufferManager::CreateBuffer(const void* data, size_t size, BufferType type) {
    if (!data || size == 0) {
        return false;
    }

    pImpl->data.resize(size);
    std::memcpy(pImpl->data.data(), data, size);

    pImpl->secBuffer.cbBuffer = static_cast<unsigned long>(size);
    pImpl->secBuffer.BufferType = static_cast<unsigned long>(type);
    pImpl->secBuffer.pvBuffer = pImpl->data.data();

    return true;
}

bool SecurityBufferManager::CreateBuffer(const std::vector<uint8_t>& data, BufferType type) {
    return CreateBuffer(data.data(), data.size(), type);
}

SecBuffer* SecurityBufferManager::GetSecBuffer() {
    return &pImpl->secBuffer;
}

std::vector<uint8_t> SecurityBufferManager::GetData() const {
    return pImpl->data;
}

size_t SecurityBufferManager::GetSize() const {
    return pImpl->data.size();
}

BufferType SecurityBufferManager::GetType() const {
    return static_cast<BufferType>(pImpl->secBuffer.BufferType);
}

void SecurityBufferManager::Clear() {
    pImpl->data.clear();
    std::memset(&pImpl->secBuffer, 0, sizeof(SecBuffer));
}

// SecurityBufferDescriptor Implementation
class SecurityBufferDescriptor::Impl {
public:
    SecBufferDesc descriptor;
    std::vector<SecBuffer> buffers;
    std::vector<std::vector<uint8_t>> bufferData;

    Impl() {
        std::memset(&descriptor, 0, sizeof(SecBufferDesc));
        descriptor.ulVersion = SECBUFFER_VERSION;
    }

    ~Impl() = default;
};

SecurityBufferDescriptor::SecurityBufferDescriptor() 
    : pImpl(std::make_unique<Impl>()) {
}

SecurityBufferDescriptor::~SecurityBufferDescriptor() = default;

bool SecurityBufferDescriptor::AddBuffer(const SecurityBufferManager& buffer) {
    auto data = buffer.GetData();
    pImpl->bufferData.push_back(data);

    SecBuffer secBuf;
    secBuf.cbBuffer = static_cast<unsigned long>(data.size());
    secBuf.BufferType = static_cast<unsigned long>(buffer.GetType());
    secBuf.pvBuffer = pImpl->bufferData.back().data();

    pImpl->buffers.push_back(secBuf);
    pImpl->descriptor.cBuffers = static_cast<unsigned long>(pImpl->buffers.size());
    pImpl->descriptor.pBuffers = pImpl->buffers.data();

    return true;
}

SecBufferDesc* SecurityBufferDescriptor::GetDescriptor() {
    return &pImpl->descriptor;
}

size_t SecurityBufferDescriptor::GetBufferCount() const {
    return pImpl->buffers.size();
}

void SecurityBufferDescriptor::Clear() {
    pImpl->buffers.clear();
    pImpl->bufferData.clear();
    std::memset(&pImpl->descriptor, 0, sizeof(SecBufferDesc));
    pImpl->descriptor.ulVersion = SECBUFFER_VERSION;
}

std::vector<uint8_t> SecurityBufferDescriptor::ToByteArray() const {
    std::vector<uint8_t> result;
    
    for (const auto& bufData : pImpl->bufferData) {
        result.insert(result.end(), bufData.begin(), bufData.end());
    }
    
    return result;
}

} // namespace KrbRelay
