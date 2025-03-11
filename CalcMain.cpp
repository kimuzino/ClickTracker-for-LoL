#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <windows.h>
#include <tlhelp32.h>
#include <chrono>
#include <filesystem>
#include <thread>
#include <unistd.h>
#include <tchar.h>

#include "KeyCodes.cpp"
#include "Graph.cpp"

// Key things
int ExitKey;
int ResetKey;
int ComparisonKey;
std::string PrintExit;
std::string PrintReset;
std::string PrintComparison;

#ifdef _WIN32
#else
#endif
std::string GetCurrentWorkingDirectory() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
#else
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        return std::string(buffer);
    } else {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }
#endif
}

bool isProcessRunning(const char* processName, DWORD& processId)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);

    if (Process32First(snapshot, &entry))
    {
        do
        {
            if (_stricmp(entry.szExeFile, processName) == 0)
            {
                processId = entry.th32ProcessID;
                CloseHandle(snapshot);
                return true;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return false;
}

int printGameDuration(std::chrono::seconds seconds, int SaveMe)
{
    if (SaveMe == 1)
    {
        // Kill me
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(seconds);;
        //int total = minutes.count() * 1;
        int total = minutes.count();
        return total;
    }

    else
    {
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(seconds);
        seconds -= std::chrono::duration_cast<std::chrono::seconds>(minutes);

        std::cout << "Duration: " << minutes.count() << " minutes, " << seconds.count() << " seconds.\n";
        return 0;
    }
}

void AddStats(int rightClickCounter, const char* Stats, int Clicked, int TopScore, int duration, int TopScoreTime)
{
    std::ofstream stats;
    stats.open(Stats);
    std::string line;

    Clicked = Clicked + rightClickCounter;

    if (TopScore < rightClickCounter)
    {
        TopScore = rightClickCounter;
        TopScoreTime = duration;
        std::cout << "New highscore achieved!\n";
    }

    if (!stats.is_open())
    {
        MessageBoxA(NULL, "Error while opening file", "File error!", MB_ICONERROR);
    }

    stats << "[HIGHSCORE] :" << TopScore << 
    "\n" << "[CLICKS] :" << Clicked << 
    "\n" << "[TOPSCORETIME] :" << TopScoreTime;
    stats.close();
}

int SetVariables(const char* Stats, int SWITCH)
{
    int Value;
    std::fstream FromFile(Stats, std::ios::in);
    std::string line;
    while (std::getline(FromFile, line))
    {
        if (SWITCH == 1)
        {
            if (line.find("[HIGHSCORE] :") != std::string::npos)
            {
                std::streampos startPos = FromFile.tellg();

                std::string highscoreStr = line.substr(line.find(":") + 1);
                Value = std::stoi(highscoreStr);
            }
        }

        if (SWITCH == 2)
        {
            if (line.find("[CLICKS] :") != std::string::npos)
            {
                std::streampos startPos = FromFile.tellg();

                std::string clicksStr = line.substr(line.find(":") + 1);
                Value = std::stoi(clicksStr);
            }
        }

        if (SWITCH == 3)
        {
            if (line.find("[TOPSCORETIME] :") != std::string::npos)
            {
                std::streampos startPos = FromFile.tellg();

                std::string topScoreTimeStr = line.substr(line.find(":") + 1);
                Value = std::stoi(topScoreTimeStr);
            }
        }
    }

    FromFile.close();

    return Value;
}

void SetKeyState(const char* KeyCommands)
{
    std::string FindReset;
    std::string FindExit;
    std::string FindStats;
    std::string FindComparison;

    std::fstream FromKeyCommands(KeyCommands, std::ios::in);
    while (std::getline(FromKeyCommands, FindReset))
    {
        if (FindReset.find("[RESET_KEY] :") != std::string::npos)
        {
            std::streampos startPos = FromKeyCommands.tellg();

            std::string resetStr = FindReset.substr(FindReset.find(":") + 1);
            ResetKey = keyMap[resetStr];
            PrintReset = KeyToText[resetStr];
        }
    }
    FromKeyCommands.close();

    std::fstream FroKeyCommands(KeyCommands, std::ios::in);
    while (std::getline(FroKeyCommands, FindExit))
    {
        if (FindExit.find("[EXIT_KEY] :") != std::string::npos)
        {
            std::streampos startPos = FroKeyCommands.tellg();

            std::string exitStr = FindExit.substr(FindExit.find(":") + 1);
            ExitKey = keyMap[exitStr];
            PrintExit = KeyToText[exitStr];
        }
    }
    FroKeyCommands.close();

    std::fstream FKeyCommands(KeyCommands, std::ios::in);
    while (std::getline(FKeyCommands, FindComparison))
    {
        if (FindComparison.find("[COMP_KEY] :") != std::string::npos)
        {
            std::streampos startPos = FKeyCommands.tellg();

            std::string CompStr = FindComparison.substr(FindComparison.find(":") + 1);
            ComparisonKey = keyMap[CompStr];
            PrintComparison = KeyToText[CompStr];
        }
    }
    FKeyCommands.close();
}

void StatReset(const char* Stats)
{
    std::ofstream stats;
    stats.open(Stats);
    std::string line;

    if (!stats.is_open())
    {
        MessageBoxA(NULL, "Error while opening file", "File error!", MB_ICONERROR);
    }

    stats << "[HIGHSCORE] :" << "0" << "\n" << "[CLICKS] :" << "0" << "\n" << "[TOPSCORETIME] :" << "0";
    stats.close();
}

void CreateAndCheckFiles(const char* Stats, const char* Settings, const char* ReadMeFile, const char* Temporary, const char* SettingsFold, const char* KeyCommands)
{
    if (!std::filesystem::exists(Temporary))
    {
        std::filesystem::create_directories(Temporary);
    }
    if (!std::filesystem::exists(SettingsFold))
    {
        std::filesystem::create_directories(SettingsFold);
    }
    if (!std::filesystem::exists(Settings))
    {
        std::ofstream CreateSettings(Settings);
        if (CreateSettings.is_open())
        {
            // Only one dot
            CreateSettings << "[VERSION] :1.0" << "\n";
            CreateSettings.close();
        }
    }
    if (!std::filesystem::exists(ReadMeFile))
    {
        std::ofstream CreateReadMeFile(ReadMeFile);
        if (CreateReadMeFile.is_open()) {
            CreateReadMeFile << "All key codes here: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes\n";
            CreateReadMeFile << "Remember to only use the hex value for mapping keys.\n";
            CreateReadMeFile << "Shift, Ctrl, Alt, Caps Lock, Windows Key and few other keys isn't supported.\n";
            CreateReadMeFile.close();
        }
    }
    if (!std::filesystem::exists(KeyCommands))
    {
        std::ofstream CreateKeyCommands(KeyCommands);
        if (CreateKeyCommands.is_open())
        {
            CreateKeyCommands << "[EXIT_KEY] :0x2E" << "\n";
            //CreateKeyCommands << "[STATS_KEY] :0x21" << "\n";
            CreateKeyCommands << "[RESET_KEY] :0x22" << "\n";
            CreateKeyCommands << "[COMP_KEY] :0x13";
            CreateKeyCommands.close();
        }

        if (!std::filesystem::exists(Stats))
        {
            std::ofstream CreateStats(Stats);
            if (CreateStats.is_open())
            {
                CreateStats << "[HIGHSCORE] :" << "0" << "\n" << "[CLICKS] :" << "0" << "\n" << "[TOPSCORETIME] :" << "0";
                CreateStats.close();
            }
        }
    }
    if (!std::filesystem::exists(Stats))
    {
        int txtFileError = MessageBoxA(NULL, "File not found! Press YES to create new", "File error!", MB_ICONERROR | MB_YESNO);

        if (txtFileError == IDYES)
        {
            std::ifstream inFile(Stats);
            while (true)
            {
                while (!inFile)
                {
                    std::ofstream CreateStats(Stats);
                    if (CreateStats.is_open())
                    {
                        CreateStats << "[HIGHSCORE] :" << "0" << "\n" << "[CLICKS] :" << "0" << "\n" << "[TOPSCORETIME] :" << "0";
                        CreateStats.close();
                        std::cout << "File created succesfully\n";
                        break;
                    }
                    else
                    {
                        std::cerr << "Error while creating file\n";
                    }
                }
                break;
            }
        }
        else if (txtFileError == IDNO)
        {
            std::cout << "No\n";
        }
    }
}

int ClicksPerMinute(int rightClickCounter, std::chrono::seconds seconds, int SaveMe)
{
    if (SaveMe == 1)
    {
        double minutes = seconds.count() / 60.0;
        double ClicksPerMinute = rightClickCounter / minutes;
        return ClicksPerMinute;
    }

    else
    {
        if (seconds.count() < 60)
        {
            std::cout << "Game was too short\n";
        }

        else
        {
            double minutes = seconds.count() / 60.0;
            double ClicksPerMinute = rightClickCounter / minutes;
            std::cout << "Clicks per minute: " << std::fixed << std::setprecision(2) << ClicksPerMinute << "\n";
        }
        return 0;
    }
}

void ShowStats(std::map<std::string, int>PreviousStats, int RightClicks, int TopClicks, std::chrono::seconds duration, int TopTime, std::string Path, DWORD ProcessId)
{
    int PrevClicks = PreviousStats["PreviousClicks"];
    int PrevCPM = PreviousStats["PreviousCPM"];
    int PrevTime = PreviousStats["PreviousTime"];
    
    double NowCPM = ClicksPerMinute(RightClicks, duration, 1);
    double TopCPM = TopClicks / TopTime;
    int NowTime = printGameDuration(duration, 1);

    int ImpClicks = RightClicks - PrevClicks;
    double ImpCPM = NowCPM - PrevCPM;
    int ImpTime = NowTime - PrevTime; 

    // Preview stats idea
    //
    //Game       Prev     Now    Improvement     Best
    //Clicks     1000     2000   +1000           3500
    //CPM        15.6     9.4    -6.2            16.3
    //Time(min)  32      29                     42
    //
    // implement the time from the fame where the best score was achieved

    std::cout << "Game\t\t"    << "Prev\t"   << "Now\t"            << "Improvement\t"   << "Best\n"
              << "Clicks\t\t"  << PrevClicks << "\t"<< RightClicks << "\t"<< ImpClicks  << "\t\t" << TopClicks << '\n'
            
              << "CPM\t\t"     << PrevCPM    << "\t"<< NowCPM      << "\t"<< ImpCPM     << "\t\t" << TopCPM    << '\n'
              << "Time(min)\t" << PrevTime   << "\t"<< NowTime     << "\t"<< ImpTime   << "\t\t" << TopTime   << '\n';

    std::ofstream CreateTemporaryFile("Temporary/StatsTemp.txt");
    if (CreateTemporaryFile.is_open())
    {
        /*Laita kaikki turhat hommat*/

        CreateTemporaryFile << "Close notepad to continue program";
        CreateTemporaryFile.close();
    }

    // Execute notepad
    Path += "/Temporary/StatsTemp.txt";
    const char* PathDirect = Path.c_str();
    ShellExecute(NULL, "open", PathDirect, NULL, NULL, SW_SHOWNORMAL);
    
    while (true)
    {
        if (!isProcessRunning("notepad.exe", ProcessId))
        {
            continue;
        } else
        {
            break;
        }
    }

    // Wait for notepad to close
    while(isProcessRunning("notepad.exe", ProcessId)) {/*Wakey wakey*/}
    std::remove(PathDirect);
}

void LaunchUpdater()
{
    char* App = "Updater.exe"; // const char*& AppRef;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
        App,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

bool GetLaunchCode(std::string LaunchCodeFind, std::string Path)
{
    if (!std::filesystem::exists(Path))
    {
        return false;
    }
    std::fstream FromCommandFile(Path, std::ios::in);
    std::string Command;
    std::string CommandStr;
    while (std::getline(FromCommandFile, Command))
    {
        if (Command.find(LaunchCodeFind) != std::string::npos)
        {
            std::streampos startPos = FromCommandFile.tellg();
            CommandStr = Command.substr(Command.find(":") + 1);
        }
    }

    if (CommandStr == "TRUE")
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main()
{
    // Path
    std::string Path = GetCurrentWorkingDirectory();

    // Icon
    #define IDI_ICON 101
    #define IDI_SMALL 102
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    HICON hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

    // Process id
    DWORD ProcessId = 0;

    // Game
    const char* exeName = "League of Legends.exe";
    // Client
    const char* clientName = "LeagueClient.exe";

    // Useless
    int rightClickCounter = 0;
    int rounds = 1;

    // All time shit
    auto endTime = std::chrono::steady_clock::now();
    auto startTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);

    // States
    bool previousState = false;
    bool wasRightClickPressed = false;
    bool WasCompKeyPressed = false;

    // Files
    const char* Stats = "Settings/Stats.txt";
    const char* Settings = "Settings/Settings.txt";
    const char* KeyCommands = "Settings/KeySettings.txt";
    const char* LaunchCommands = "Settings/LaunchCommands.txt";
    const char* ReadMeFile = "ReadMe.txt";
    const char* TemporaryFolder = "Temporary";
    const char* SettingsFolder = "Settings";
    std::filesystem::remove("Temporary/Logs.txt");

    // Map for stats comparison
    std::map<std::string, int> MapPreviousStats;
    std::pair<std::map<std::string, int>::iterator, bool> ret;
    int FirstGame = 0;

    // Stats
    int TopScore = SetVariables(Stats, 1);
    int Clicked = SetVariables(Stats, 2);
    int TopScoreTime = SetVariables(Stats, 3);

    // Checks / creates files
    CreateAndCheckFiles(Stats, Settings, ReadMeFile, TemporaryFolder, SettingsFolder, KeyCommands);

    // Sets keys and stats
    SetKeyState(KeyCommands);

    // Updater
    if (GetLaunchCode("[DISABLE_UPDATE] :", Path + "/Settings/LaunchCommands.txt") == false)
    {
        LaunchUpdater();
    }
    else
    {
        if (std::filesystem::exists(Path + "/Temporary/Logs.txt"))
        {
            std::filesystem::remove(Path + "/Temporary/Logs.txt");
        }
        std::ofstream WriteLog(Path + "/Temporary/Logs.txt", std::ios::app); WriteLog << "Updater Disabled\n"; WriteLog.close();
    }
    while(true)
    {
        if (GetLaunchCode("[UPDATE] :", Path + "/Temporary/UpdatePermission.txt") == true)
        {
            int UpdateMessage = MessageBoxA(NULL, "Update available! Press YES to update", "Update", MB_ICONINFORMATION | MB_YESNO);
            if (UpdateMessage == IDYES)
            {
                std::ofstream Verify(Path + "/Temporary/UpdatePremission.txt", std::ios::app); Verify << "[UPDATE_VERIFY]: TRUE\n"; Verify.close();
                while(true){}
            }
            else if (UpdateMessage == IDNO)
            {
                std::ofstream Verify(Path + "/Temporary/UpdatePremission.txt", std::ios::app); Verify << "[UPDATE_VERIFY]: FALSE\n"; Verify.close();
                break;
            }   
        }
        else if (GetLaunchCode("[UPDATE] :", Path + "/Temporary/UpdatePermission.txt") == false)
        {
            std::filesystem::remove(Path + "/Temporary/UpdatePermission.txt");
            break;
        }
    }

    // Binds
    std::cout << "Press " << PrintReset << " to reset stats\n";
    std::cout << "Press " << PrintExit << " to exit\n";
    std::cout << "Press " << PrintComparison << " to view stats comparison\n";

    while (isProcessRunning(clientName, ProcessId) == false) // Change to true
    {
        bool currentState = isProcessRunning(exeName, ProcessId);

        // Exit function
        if (GetAsyncKeyState(ExitKey) & 1)
        {
            exit(0);
        }

        // ResetStats function
        if (GetAsyncKeyState(ResetKey) & 1)
        {
            int ResetMessage = MessageBoxA(NULL, "Are you sure you want to reset stats?", "Confirm!", MB_ICONWARNING | MB_YESNO);

            if (ResetMessage == IDYES)
            {
                StatReset(Stats);
                TopScore = 0;
                Clicked = 0;
                std::cout << "\n" << "Stats reseted successfully!\n";
            }
            else if (ResetMessage == IDNO)
            {
                continue;
            }
        }

        // Stats function
        bool IsCompKeyPressed = (GetAsyncKeyState(ComparisonKey) & 1) != 0;
        if (WasCompKeyPressed && !IsCompKeyPressed)
        {
            std::cout << "HelloM";
            InsertToGraph = GraphMap.insert(std::pair<std::string, int>("Game1", 1696));
            InsertToGraph = GraphMap.insert(std::pair<std::string, int>("Game2", 480));
            InsertToGraph = GraphMap.insert(std::pair<std::string, int>("Game3", 2885));
            InsertToGraph = GraphMap.insert(std::pair<std::string, int>("Game4", 2216));
            InsertToGraph = GraphMap.insert(std::pair<std::string, int>("Game5", 5960));
            GraphGames = 5;

            CallGraph(true);
            //ShowStats(MapPreviousStats, rightClickCounter, TopScore, duration, TopScoreTime, Path, ProcessId);
        }
        WasCompKeyPressed = IsCompKeyPressed;

        // Main code
        if (currentState != previousState)
        {
            if (currentState)
            {
                if (FirstGame > 0)
                {
                    MapPreviousStats.clear();
                    ret = MapPreviousStats.insert(std::pair<std::string, int>("PreviousCPM", ClicksPerMinute(rightClickCounter, duration, 1)));
                    ret = MapPreviousStats.insert(std::pair<std::string, int>("PreviousClicks", rightClickCounter));
                    ret = MapPreviousStats.insert(std::pair<std::string, int>("PreviousTime", printGameDuration(duration, 1)));
                }

                std::cout << "\n" << "Game (" << rounds << ") has started.\n";
                startTime = std::chrono::steady_clock::now();
                rightClickCounter = 0;
                FirstGame++;
            }

            else
            {
                endTime = std::chrono::steady_clock::now();
                duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
                std::cout << "Game has ended.\n";
                printGameDuration(duration, 0);
                std::cout << "Total right clicks: " << rightClickCounter << "\n";
                ClicksPerMinute(rightClickCounter, duration, 0);
                rounds++;

                if (GraphGames > 5) {GraphGames = 1; CallGraph(false); for (int i; i < 5; i++){ClicksList[i] = 0;} GraphMap.clear(); InsertToGraph = GraphMap.insert(std::pair<std::string, int>("Game1", rightClickCounter));}
                else {InsertToGraph = GraphMap.insert(std::pair<std::string, int>("Game" + std::to_string(GraphGames), rightClickCounter));}
                GraphGames++;

                AddStats(rightClickCounter, Stats, Clicked, TopScore, printGameDuration(duration, 1), TopScoreTime);

                TopScore = SetVariables(Stats, 1);
                Clicked = SetVariables(Stats, 2);
            }
            previousState = currentState;
        }

        // Right click counter
        bool isRightClickPressed = (GetAsyncKeyState(VK_RBUTTON) & 0x8001) != 0;
        if (wasRightClickPressed && !isRightClickPressed)
        {
            rightClickCounter++;
        }
        wasRightClickPressed = isRightClickPressed;
    }
}