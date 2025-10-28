#pragma once

#include "Types.h"
#include "SecurityBuffer.h"
#include <string>
#include <vector>
#include <functional>

namespace KrbRelay {

/**
 * @brief Callback function for handling intercepted authentication tokens
 * @param ticket Authentication ticket/token data
 * @param isKerberos True if Kerberos, false if NTLM
 * @return True to continue, false to abort
 */
using AuthenticationCallback = std::function<bool(const std::vector<uint8_t>& ticket, bool isKerberos)>;

/**
 * @brief Main class for Kerberos relay functionality
 * 
 * This class provides the core functionality for relaying Kerberos
 * authentication tokens. It can be used to build custom relay tools.
 */
class KRBRELAY_API KerberosRelay {
public:
    KerberosRelay();
    ~KerberosRelay();

    /**
     * @brief Configure relay parameters
     * @param spn Service Principal Name for target service
     * @param useSSL Whether to use SSL/TLS
     * @return Operation result
     */
    OperationResult Configure(const std::string& spn, bool useSSL = false);

    /**
     * @brief Set authentication callback
     * @param callback Callback function to handle intercepted tickets
     */
    void SetAuthenticationCallback(AuthenticationCallback callback);

    /**
     * @brief Initialize COM server for relay
     * @param clsid CLSID to use for COM server
     * @param port Port for RPC server
     * @return Operation result
     */
    OperationResult InitializeComServer(const std::string& clsid, uint16_t port);

    /**
     * @brief Hook AcceptSecurityContext function
     * @return Operation result
     */
    OperationResult HookAcceptSecurityContext();

    /**
     * @brief Unhook AcceptSecurityContext function
     * @return Operation result
     */
    OperationResult UnhookAcceptSecurityContext();

    /**
     * @brief Extract Kerberos ticket from security buffer
     * @param inputBuffer Input security buffer
     * @param ticket Output ticket data
     * @return Operation result
     */
    static OperationResult ExtractKerberosTicket(
        SecBufferDesc* inputBuffer,
        std::vector<uint8_t>& ticket
    );

    /**
     * @brief Validate Kerberos AP-REQ ticket
     * @param ticket Ticket data
     * @return True if valid AP-REQ
     */
    static bool ValidateApReq(const std::vector<uint8_t>& ticket);

    /**
     * @brief Convert ticket format for relay
     * @param ticket Input ticket (in/out)
     * @return Operation result
     */
    static OperationResult ConvertTicketFormat(std::vector<uint8_t>& ticket);

    /**
     * @brief Get SPN from configuration
     * @return Configured SPN
     */
    std::string GetSPN() const;

    /**
     * @brief Get SSL configuration
     * @return True if SSL is enabled
     */
    bool IsSSLEnabled() const;

    /**
     * @brief Set user context for relay
     * @param domain User domain
     * @param username User name
     */
    void SetUserContext(const std::string& domain, const std::string& username);

    /**
     * @brief Get user domain
     * @return User domain
     */
    std::string GetUserDomain() const;

    /**
     * @brief Get username
     * @return Username
     */
    std::string GetUsername() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Helper functions for Kerberos relay
 */
namespace RelayHelpers {
    /**
     * @brief Convert byte array to hex string
     * @param data Byte array
     * @return Hex string
     */
    KRBRELAY_API std::string BytesToHexString(const std::vector<uint8_t>& data);

    /**
     * @brief Convert hex string to byte array
     * @param hexString Hex string
     * @return Byte array
     */
    KRBRELAY_API std::vector<uint8_t> HexStringToBytes(const std::string& hexString);

    /**
     * @brief Find pattern in byte array
     * @param data Data to search
     * @param pattern Pattern to find
     * @return Offset of pattern, or -1 if not found
     */
    KRBRELAY_API int FindPattern(const std::vector<uint8_t>& data, const std::vector<uint8_t>& pattern);

    /**
     * @brief Parse SPN to extract service and hostname
     * @param spn Service Principal Name
     * @param service Output service name
     * @param hostname Output hostname
     * @return True if successful
     */
    KRBRELAY_API bool ParseSPN(const std::string& spn, std::string& service, std::string& hostname);
}

} // namespace KrbRelay
