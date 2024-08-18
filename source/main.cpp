#include <MainApplication.hpp>


int main(int argc, char** argv){
    // On windows, WSAStartup has to be called to allow the use of network sockets
    #ifdef _WIN32
    WSADATA wsadata;
    (void) WSAStartup(MAKEWORD(2, 2), &wsadata);
    #endif

    // Run the main application
    mainApplication.Run(argc, argv);

    // On windows, call the counter part of the corresponding startup
    #ifdef _WIN32
    WSACleanup();
    #endif
    return 0;
}

