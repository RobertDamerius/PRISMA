#pragma once


#include <Common.hpp>
#include <NonCopyable.hpp>
#include <GLWindow.hpp>
#include <Engine.hpp>
#include <NetworkManager.hpp>
#include <WindowEventMessage.hpp>


class MainApplication: private NonCopyable {
    public:
        /**
         * @brief Run the main application.
         * @param[in] argc Number of arguments passed to the application.
         * @param[in] argv List of arguments passed to the application.
         */
        void Run(int argc, char** argv);

        /**
         * @brief Inform the internal window to close.
         */
        void CloseWindow(void);

        /**
         * @brief Send a window event message via the network manager.
         * @param[in] msg The message to be send.
         */
        void SendWindowEventMessage(WindowEventMessage& msg);

    private:
        GLWindow glWindow;               // The internal GL window object.
        Engine engine;                   // The internal PRISMA engine that handles input events and rendering (this engine also contains the PRISMA state).
        NetworkManager networkManager;   // The network manager that handles incomming UDP messages.

        /**
         * @brief Print system information to the output.
         */
        void PrintSystemInfo(void);

        /**
         * @brief Print arguments that where passed to the application.
         * @param[in] argc Number of arguments passed to the application.
         * @param[in] argv List of arguments passed to the application.
         */
        void PrintArguments(int argc, char** argv);

        /**
         * @brief Redirect console prints (stdout, stderr) to a file.
         */
        void RedirectPrintsToFile(void);

        /**
         * @brief Make a protocol directory for prints.
         * @return True if success, false otherwise.
         */
        bool MakeProtocolDirectory(void);

        /**
         * @brief Keep the N latest protocol files.
         * @param[in] N Number of protocol files to keep.
         */
        void KeepNLatestProtocolFiles(uint32_t N);

        /**
         * @brief The signal handler that handles signals from OS.
         * @param[in] signal Signal value.
         */
        static void SignalHandler(int signal);

        /**
         * @brief Global termination function that is called if exception handling fails.
         * @details On linux, a backtrace to the error is printed if debugging is enabled.
         */
        static void TerminateHandler(void);
};


extern MainApplication mainApplication;

