#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <algorithm>

using namespace std;

#ifdef _WIN32
#include <windows.h>
#define PLATFORM_WINDOWS
#else
#include <unistd.h>
#include <sys/wait.h>
#define PLATFORM_UNIX
#endif

// Function declarations
bool IsWslAvailable();
void ExecuteCommand(const string &command, const vector<string> &args);
vector<string> SplitString(const string &str);
string ToLower(const string &str);
string Trim(const string &str);

int main(int argc, char *argv[])
{
    // Check if WSL is available on Windows
#ifdef PLATFORM_WINDOWS
    if (!IsWslAvailable())
    {
        cout << "ERROR: WSL is not installed or not available." << endl;
        cout << "Please install WSL by running: wsl --install" << endl;
        cout << "(Run this in PowerShell as Administrator)" << endl;
        return 1;
    }
    cout << "Small Bash Shell running on WSL(linux subsystem for Windows)" << endl;
#else
    cout << "Running in native Unix/Linux mode" << endl;
#endif

    // Shell loop with parsing and error handling
    while (true)
    {
        cout << "Espinoza-Owens> ";
        cout.flush();

        string input;

        // Read line and handle EOF
        if (!getline(cin, input))
        {
            cout << "\nEOF detected. Exiting..." << endl;
            break;
        }

        // Trim whitespace and ignore blank lines
        input = Trim(input);
        if (input.empty())
        {
            continue;
        }

        // Parse input: split on whitespace
        vector<string> tokens = SplitString(input);

        if (tokens.empty())
        {
            continue;
        }

        // Extract command and arguments
        string command = ToLower(tokens[0]);

        // Get arguments
        vector<string> cmdArgs(tokens.begin() + 1, tokens.end());

        // Exit condition
        if (command == "exit")
        {
            cout << "Bye!" << endl;
            break;
        }

        // Execute command
        ExecuteCommand(tokens[0], cmdArgs); // Use original case for command
    }

    return 0;
}

bool IsWslAvailable()
{
#ifdef PLATFORM_WINDOWS
    STARTUPINFOA si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};

    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;

    // Create pipes to suppress output
    HANDLE hStdOutRead, hStdOutWrite;
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

    if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0))
    {
        return false;
    }

    si.hStdOutput = hStdOutWrite;
    si.hStdError = hStdOutWrite;

    char cmdLine[] = "wsl.exe --status";
    BOOL success = CreateProcessA(
        NULL,
        cmdLine,
        NULL, NULL, TRUE,
        CREATE_NO_WINDOW,
        NULL, NULL,
        &si, &pi);

    CloseHandle(hStdOutWrite);

    if (!success)
    {
        CloseHandle(hStdOutRead);
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hStdOutRead);

    return exitCode == 0;
#else
    return true; // Not needed on Unix
#endif
}

void ExecuteCommand(const string &command, const vector<string> &args)
{
#ifdef PLATFORM_UNIX
    // Native Unix/Linux/Mac - fork and exec
    pid_t pid = fork();

    if (pid == -1)
    {
        cerr << "Error: fork failed" << endl;
        return;
    }

    if (pid == 0)
    {
        // Child process
        vector<char *> argv;
        argv.push_back(const_cast<char *>(command.c_str()));

        for (const auto &arg : args)
        {
            argv.push_back(const_cast<char *>(arg.c_str()));
        }
        argv.push_back(nullptr);

        execvp(command.c_str(), argv.data());

        // If execvp returns, it failed
        cout << command << ": command not found" << endl;
        exit(1);
    }
    else
    {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
#else
    // Windows - run through WSL
    string fullCommand = "wsl.exe sh -c \"" + command;

    for (const auto &arg : args)
    {
        string escaped = arg;
        // Escape quotes in arguments
        size_t pos = 0;
        while ((pos = escaped.find("\"", pos)) != string::npos)
        {
            escaped.replace(pos, 1, "\\\"");
            pos += 2;
        }
        fullCommand += " " + escaped;
    }
    fullCommand += "\"";

    STARTUPINFOA si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};

    // Allow child to inherit console
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    char *cmdLine = const_cast<char *>(fullCommand.c_str());
    BOOL success = CreateProcessA(
        NULL,
        cmdLine,
        NULL, NULL, TRUE,
        0,
        NULL, NULL,
        &si, &pi);

    if (!success)
    {
        cout << command << ": command not found" << endl;
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
#endif
}

vector<string> SplitString(const string &str)
{
    vector<string> tokens;
    istringstream iss(str);
    string token;

    while (iss >> token)
    {
        tokens.push_back(token);
    }

    return tokens;
}

string ToLower(const string &str)
{
    string result = str;
    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c)
              { return tolower(c); });
    return result;
}

string Trim(const string &str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos)
    {
        return "";
    }

    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}