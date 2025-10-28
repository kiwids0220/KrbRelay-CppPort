#include <KrbRelay/KerberosRelay.h>
#include <KrbRelay/SecurityBuffer.h>
#include <KrbRelay/NativeWrappers.h>
#include <KrbRelay/ComUtilities.h>
#include <iostream>
#include <iomanip>

using namespace KrbRelay;

// Example callback for handling intercepted tickets
bool OnAuthenticationTicket(const std::vector<uint8_t>& ticket, bool isKerberos) {
    std::cout << "[+] Authentication ticket intercepted!" << std::endl;
    std::cout << "[*] Type: " << (isKerberos ? "Kerberos" : "NTLM") << std::endl;
    std::cout << "[*] Size: " << ticket.size() << " bytes" << std::endl;
    
    // Display first 32 bytes in hex
    std::cout << "[*] Data: ";
    size_t displaySize = std::min(ticket.size(), size_t(32));
    for (size_t i = 0; i < displaySize; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(ticket[i]);
    }
    if (ticket.size() > 32) {
        std::cout << "...";
    }
    std::cout << std::dec << std::endl;
    
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "KrbRelay C++ Library - Basic Example" << std::endl;
    std::cout << "=====================================" << std::endl << std::endl;

    // Example 1: Initialize COM
    std::cout << "[*] Initializing COM..." << std::endl;
    auto result = NativeWrappers::InitializeCOM();
    if (!result.success) {
        std::cerr << "[-] Failed to initialize COM: " << result.message << std::endl;
        return 1;
    }
    std::cout << "[+] COM initialized successfully" << std::endl << std::endl;

    // Example 2: Parse SPN
    std::cout << "[*] Parsing SPN..." << std::endl;
    std::string service, hostname;
    std::string spn = "ldap/dc01.contoso.com";
    
    if (RelayHelpers::ParseSPN(spn, service, hostname)) {
        std::cout << "[+] SPN parsed successfully:" << std::endl;
        std::cout << "    Service:  " << service << std::endl;
        std::cout << "    Hostname: " << hostname << std::endl;
    }
    std::cout << std::endl;

    // Example 3: Create security buffer
    std::cout << "[*] Creating security buffer..." << std::endl;
    SecurityBufferManager bufferMgr;
    std::vector<uint8_t> testData = {0x60, 0x82, 0x01, 0x23, 0x06, 0x09};
    
    if (bufferMgr.CreateBuffer(testData, BufferType::SECBUFFER_TOKEN)) {
        std::cout << "[+] Security buffer created" << std::endl;
        std::cout << "    Size: " << bufferMgr.GetSize() << " bytes" << std::endl;
        std::cout << "    Type: SECBUFFER_TOKEN" << std::endl;
    }
    std::cout << std::endl;

    // Example 4: Configure KerberosRelay
    std::cout << "[*] Configuring Kerberos relay..." << std::endl;
    KerberosRelay relay;
    
    result = relay.Configure("ldap/dc01.contoso.com", false);
    if (result.success) {
        std::cout << "[+] Relay configured" << std::endl;
        std::cout << "    SPN: " << relay.GetSPN() << std::endl;
        std::cout << "    SSL: " << (relay.IsSSLEnabled() ? "Enabled" : "Disabled") << std::endl;
    }
    
    // Set user context
    relay.SetUserContext("CONTOSO", "Administrator");
    std::cout << "    User: " << relay.GetUserDomain() << "\\" << relay.GetUsername() << std::endl;
    std::cout << std::endl;

    // Example 5: Validate AP-REQ
    std::cout << "[*] Validating AP-REQ ticket..." << std::endl;
    std::vector<uint8_t> validApReq = {0x60, 0x82, 0x01, 0x23};  // Valid start
    std::vector<uint8_t> invalidApReq = {0x05, 0x00, 0x0b};      // Invalid
    
    std::cout << "    Valid ticket:   " << (KerberosRelay::ValidateApReq(validApReq) ? "PASS" : "FAIL") << std::endl;
    std::cout << "    Invalid ticket: " << (KerberosRelay::ValidateApReq(invalidApReq) ? "PASS" : "FAIL") << std::endl;
    std::cout << std::endl;

    // Example 6: Hex string conversion
    std::cout << "[*] Testing hex string utilities..." << std::endl;
    std::vector<uint8_t> bytes = {0xDE, 0xAD, 0xBE, 0xEF};
    std::string hexStr = RelayHelpers::BytesToHexString(bytes);
    std::cout << "    Bytes to hex: " << hexStr << std::endl;
    
    auto convertedBytes = RelayHelpers::HexStringToBytes(hexStr);
    std::cout << "    Hex to bytes: ";
    if (bytes == convertedBytes) {
        std::cout << "MATCH" << std::endl;
    } else {
        std::cout << "MISMATCH" << std::endl;
    }
    std::cout << std::endl;

    // Example 7: Pattern search
    std::cout << "[*] Testing pattern search..." << std::endl;
    std::vector<uint8_t> data = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x6E, 0x82, 0x08, 0x09};
    std::vector<uint8_t> pattern = {0x6E, 0x82};
    
    int offset = RelayHelpers::FindPattern(data, pattern);
    if (offset >= 0) {
        std::cout << "[+] Pattern found at offset: " << offset << std::endl;
    } else {
        std::cout << "[-] Pattern not found" << std::endl;
    }
    std::cout << std::endl;

    // Example 8: COM Utilities
    std::cout << "[*] Testing COM utilities..." << std::endl;
    COMUtilities comUtil;
    std::vector<uint8_t> sampleObjref = {
        0x4D, 0x45, 0x4F, 0x57  // MEOW signature
    };
    // Add more bytes for a minimal valid OBJREF
    sampleObjref.resize(70, 0x00);
    
    std::string moniker = comUtil.CreateMonikerString(sampleObjref);
    std::cout << "    Moniker: " << moniker.substr(0, 40) << "..." << std::endl;
    std::cout << std::endl;

    // Cleanup
    std::cout << "[*] Cleaning up..." << std::endl;
    NativeWrappers::UninitializeCOM();
    std::cout << "[+] Done!" << std::endl;

    return 0;
}
