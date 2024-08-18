#include <MainApplication.hpp>
#include <FileManager.hpp>
#include <PrismaConfiguration.hpp>


MainApplication mainApplication;


void MainApplication::Run(int argc, char** argv){
    // install signals and redirect prints to a protocol file
    std::set_terminate(MainApplication::TerminateHandler);
    std::signal(SIGINT, &SignalHandler);
    std::signal(SIGTERM, &SignalHandler);
    std::signal(SIGSEGV, &SignalHandler);
    RedirectPrintsToFile();

    // load configuration file
    std::string alternativeConfigurationFile = "";
    if(2 == argc){
        alternativeConfigurationFile = argv[1];
    }
    if(!prismaConfiguration.ReadFromFile(alternativeConfigurationFile)){
        return;
    }

    // print information
    PrintSystemInfo();
    PrintArguments(argc, argv);
    //TODO(robertdamerius): print raw configuration file data
    Print("\n");

    // start network manager, create window and run event loop
    networkManager.Start();
    glWindow.Run(&engine);
    networkManager.Stop();
}

void MainApplication::CloseWindow(void){
    glWindow.Close();
}

void MainApplication::SendWindowEventMessage(WindowEventMessage& msg){
    networkManager.SendWindowEventMessage(msg);
}

void MainApplication::PrintSystemInfo(void){
    // title
    Print("PRISMA\n\n");

    // operating system
    Print("operating system:         %s\n", strOS.c_str());
    #if __linux__
    struct utsname info;
    (void) uname(&info);
    Print("sysname:                  %s\n", info.sysname);
    Print("nodename:                 %s\n", info.nodename);
    Print("release:                  %s\n", info.release);
    Print("version:                  %s\n", info.version);
    Print("machine:                  %s\n", info.machine);
    #ifdef __USE_GNU
    Print("domainname:               %s\n", info.domainname);
    #endif
    #elif _WIN32
    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    (void) GetVersionEx(&osvi);
    Print("dwBuildNumber:            %ld\n", osvi.dwBuildNumber);
    Print("dwMajorVersion:           %ld\n", osvi.dwMajorVersion);
    Print("dwMinorVersion:           %ld\n", osvi.dwMinorVersion);
    Print("dwPlatformId:             %ld\n", osvi.dwPlatformId);
    Print("szCSDVersion:             %s\n", osvi.szCSDVersion);
    #endif

    // network interfaces
    Print("network interfaces:       ");
    #if __linux__
    struct if_nameindex *if_ni, *i;
    if_ni = if_nameindex();
    if(if_ni){
        for(i = if_ni; !(i->if_index == 0 && i->if_name == nullptr); i++){
            Print("[%s]", i->if_name);
        }
        if_freenameindex(if_ni);
    }
    #else
    Print("unknown");
    #endif
    Print("\n");

    // version and date
    auto timePoint = std::chrono::system_clock::now();
    std::time_t systemTime = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* gmTime = std::gmtime(&systemTime);
    Print("PRISMA version:           %s\n", strVersion.c_str());
    Print("compiler version:         %s\n", strCompilerVersion.c_str());
    Print("built (local):            %s\n", strBuilt.c_str());
    Print("current time (UTC):       %04u-%02u-%02u %02u:%02u:%02u\n", 1900 + gmTime->tm_year, 1 + gmTime->tm_mon, gmTime->tm_mday, gmTime->tm_hour, gmTime->tm_min, gmTime->tm_sec);
    #ifdef DEBUG
    Print("DEBUG:                    1\n");
    #else
    Print("DEBUG:                    0\n");
    #endif

    // application path
    Print("path:                     %s\n", FileManager::GetApplicationPath().string().c_str()); // use string().c_str() instead of .c_str(), otherwise file separators result in escape characters under windows
}

void MainApplication::PrintArguments(int argc, char** argv){
    Print("arguments:                ");
    for(int i = 0; i < argc; ++i){
        Print("[%s]",argv[i]);
    }
    Print("\n");
}

void MainApplication::RedirectPrintsToFile(void){
    if(MakeProtocolDirectory()){
        constexpr uint32_t numberOfOldProtocolFiles = 10;
        KeepNLatestProtocolFiles(numberOfOldProtocolFiles);
        std::string filename = FileName(FILENAME_PRISMA_PROTOCOL);
        Print("\nConsole output is redirected to \"%s\"\n",filename.c_str());
        FILE* unused = std::freopen(filename.c_str(), "a", stdout);
        unused = std::freopen(filename.c_str(), "a", stderr);
        (void)unused;
    }
}

bool MainApplication::MakeProtocolDirectory(void){
    std::filesystem::path directory = FileName(FILENAME_DIRECTORY_PROTOCOL);
    try{
        std::filesystem::create_directory(directory);
    }
    catch(...){
        PrintE("Could not create protocol directory \"%s\"!\n", directory.string().c_str());
        return false;
    }
    return true;
}

void MainApplication::KeepNLatestProtocolFiles(uint32_t N){
    std::filesystem::path directory = FileName(FILENAME_DIRECTORY_PROTOCOL);
    std::vector<std::filesystem::path> files;
    for(const auto& entry : std::filesystem::directory_iterator(directory)){
        if(entry.is_regular_file()){
            files.push_back(entry.path());
        }
    }
    if(static_cast<uint32_t>(files.size()) > N){
        std::sort(files.rbegin(), files.rend());
        for(uint32_t n = N; n < static_cast<uint32_t>(files.size()); ++n){
            std::error_code ec;
            if(!std::filesystem::remove(files[n], ec)){
                PrintW("Could not remove protocol file \"%s\": %s\n", files[n].string().c_str(), ec.message().c_str());
            }
        }
    }
}

void MainApplication::SignalHandler(int signal){
    switch(signal){
        case SIGSEGV:
            throw std::runtime_error("SIGSEGV");
            break;
        case SIGINT:
        case SIGTERM:
            mainApplication.CloseWindow();
            break;
    }
}

void MainApplication::TerminateHandler(void){
    static bool triedRethrow = false;
    try {
        if(!triedRethrow){
            triedRethrow = true;
            throw;
        }
    }
    catch(const std::exception &e){ std::cerr << "\nERROR: Caught unhandled exception: " << e.what() << std::endl; }
    catch(...){ std::cerr << "\nERROR: Caught unknown/unhandled exception.\n"; }
    #ifndef _WIN32
    const int backtraceSize = 64;
    void* array[backtraceSize];
    int size(backtrace(array, backtraceSize));
    char** messages(backtrace_symbols(array, size));
    std::cerr << "\nBacktrace:\n";
    for(int i = 0; (i < size) && (messages != nullptr); ++i){
        std::cerr << "[" << i << "]: " << messages[i] << std::endl;
    }
    std::cerr << std::endl;
    free(messages);
    #endif
    std::abort();
}

