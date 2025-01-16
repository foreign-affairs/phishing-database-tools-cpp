#include <iostream>
#include <string>
#include <dns_sd.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <arpa/inet.h>
#include "version.h" // Include the version header file

// Function to print help text
void printHelp() {
    std::cout << "Usage: PhishingDatabaseTools [options] domain test_value\n"
              << "Options:\n"
              << "  -h, --help          Show this help message\n"
              << "  -v, --version       Show the program version\n"
              << "  -p, --prefix PREFIX Specify a custom prefix for the domain (default: _phishingdb.)\n";
}

// Function to print version
void printVersion() {
    std::cout << "Phishing Database Tools version " << VERSION << "\n";
}

// Function to set text color
void setColor(const std::string& color) {
    if (color == "green") {
        std::cout << "\033[1;32m"; // Green
    } else if (color == "orange") {
        std::cout << "\033[1;33m"; // Orange
    }
}

// Function to reset text color
void resetColor() {
    std::cout << "\033[0m"; // Reset
}

// Function to perform DNS TXT query
bool queryDnsTxtRecord(const std::string& domain, const std::string& testValue, const std::string& prefix) {
    std::string fullDomain = prefix + domain;
    res_init();
    _res.nscount = 2;
    _res.nsaddr_list[0].sin_addr.s_addr = inet_addr("9.9.9.10");
    _res.nsaddr_list[1].sin_addr.s_addr = inet_addr("149.112.112.10");

    unsigned char response[NS_PACKETSZ];
    int responseSize = res_query(fullDomain.c_str(), C_IN, ns_t_txt, response, sizeof(response));
    if (responseSize < 0) {
        std::cerr << "Failed to query DNS TXT record for " << fullDomain << "\n";
        return false;
    }

    ns_msg msg;
    ns_initparse(response, responseSize, &msg);
    int count = ns_msg_count(msg, ns_s_an);

    for (int i = 0; i < count; ++i) {
        ns_rr rr;
        ns_parserr(&msg, ns_s_an, i, &rr);
        std::string txtRecord(reinterpret_cast<const char*>(ns_rr_rdata(rr)) + 1, (size_t)ns_rr_rdata(rr)[0]);
        if (txtRecord == testValue) {
            setColor("green");
            std::cout << "The test value matches the DNS TXT record.\n";
            resetColor();
            return true;
        }
    }

    setColor("orange");
    std::cout << "The test value does not match the DNS TXT record.\n";
    resetColor();
    return false;
}

auto main(int argc, char* argv[]) -> int
{
    std::string prefix = "_phishingdb.";

    // Check for help or version arguments
    if (argc < 2 || (argc == 2 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help"))) {
        printHelp();
        return 0;
    }

    // Check for version argument
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-v" || std::string(argv[i]) == "--version") {
            printVersion();
            return 0;
        }
    }

    // Parse command line arguments
    for (int i = 1; i < argc - 2; ++i) {
        if (std::string(argv[i]) == "-p" || std::string(argv[i]) == "--prefix") {
            prefix = argv[++i];
        }
    }

    std::string domain = argv[argc - 2];
    std::string testValue = argv[argc - 1];

    // Perform DNS TXT query and compare the value
    queryDnsTxtRecord(domain, testValue, prefix);

    std::cout << "Thanks for using my tools. Please consider sponsoring me at https://www.mypdns.org/donate\n";
    return 0;
}
