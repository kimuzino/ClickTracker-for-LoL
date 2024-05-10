#include <iostream>
#include <fstream>
#include <windows.h>
#include <tlhelp32.h>
#include <chrono>
#include <filesystem>
#include <thread>

#include "KeyCodes.cpp"

// Key things
int ExitKey;
int StatsKey;
int ResetKey;
std::string PrintExit;
std::string PrintStats;
std::string PrintReset;

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

void printGameDuration(std::chrono::seconds seconds) 
{
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(seconds);
    seconds -= std::chrono::duration_cast<std::chrono::seconds>(minutes);

    std::cout << "Duration: " << minutes.count() << " minutes, " << seconds.count() << " seconds." << "\n";
}

void AddStats(int rightClickCounter, const char* Stats, int Clicked, int TopScore) 
{
    std::ofstream stats;
    stats.open(Stats);
    std::string line;

    Clicked = Clicked + rightClickCounter;

    if (TopScore < rightClickCounter)
    {
        TopScore = rightClickCounter;
        std::cout << "New highscore achieved!" << "\n";
    }

    if (!stats.is_open())
    {
        MessageBox(NULL, "Error while opening file", "File error!", MB_ICONERROR);
    }

    stats << "[HIGHSCORE] :" << TopScore << "\n" << "[CLICKS] :" << Clicked << "\n";
    stats.close();
}

int SetVariables(const char* Stats, int SWITCH) 
{ 
    int Value;
    // Takes the highscore and clicks value
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
    }

    FromFile.close();

    return Value;
}

void SetKeyState(const char* Settings)
{
    std::string FindReset;
    std::string FindExit;
    std::string FindStats;

    std::fstream FromSettings(Settings, std::ios::in);

    while (std::getline(FromSettings, FindReset))
    {
        if (FindReset.find("[RESET_KEY] :") != std::string::npos)
        {
            std::streampos startPos = FromSettings.tellg();

            std::string resetStr = FindReset.substr(FindReset.find(":") + 1);
            ResetKey = keyMap[resetStr];
            PrintReset = KeyToText[resetStr];
        }
    }
    FromSettings.close();
    std::fstream FroSettings(Settings, std::ios::in);

    while (std::getline(FroSettings, FindExit))
    {
        if (FindExit.find("[EXIT_KEY] :") != std::string::npos)
        {
            std::streampos startPos = FroSettings.tellg();

            std::string exitStr = FindExit.substr(FindExit.find(":") + 1);
            ExitKey = keyMap[exitStr];
            PrintExit = KeyToText[exitStr];
        }
    }
    FroSettings.close();

    std::fstream FrSettings(Settings, std::ios::in);

    while (std::getline(FrSettings, FindStats))
    {
        if (FindStats.find("[STATS_KEY] :") != std::string::npos)
        {
            std::streampos startPos = FrSettings.tellg();

            std::string statsStr = FindStats.substr(FindStats.find(":") + 1);
            StatsKey = keyMap[statsStr];
            PrintStats = KeyToText[statsStr];
        }
    }
    FrSettings.close();
}

void StatReset(const char* Stats)
{
    std::ofstream stats;
    stats.open(Stats);
    std::string line;

    if (!stats.is_open())
    {
        MessageBox(NULL, "Error while opening file", "File error!", MB_ICONERROR);
    }

    stats << "[HIGHSCORE] :" << "0" << "\n" << "[CLICKS] :" << "0" << "\n";
    stats.close();
}

void CreateFiles(const char* Stats, const char* Settings, const char* HowToUseFile)
{
    // All needed to know how the code calculator works
    if (!std::filesystem::exists(HowToUseFile))
    {
        std::ofstream CreateHowToUseFile(HowToUseFile);
        if (CreateHowToUseFile.is_open()) {
            CreateHowToUseFile << "All key codes here: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes" << "\n";
            CreateHowToUseFile << "Remember to only use the hex value for mapping keys." << "\n";
            CreateHowToUseFile << "Shift, Ctrl, Alt, Caps Lock, Windows Key and few other keys isn't supported." << "\n";
            CreateHowToUseFile << "You can map more keys by modifying the KeyCodes.cpp file." << "\n";
            CreateHowToUseFile.close();
        }
    }

    // Setups settings and creates stats file if needed
    if (!std::filesystem::exists(Settings))
    {
        std::ofstream outFile(Settings);
        if (outFile.is_open())
        {
            outFile << "[EXIT_KEY] :0x2E" << "\n";
            outFile << "[STATS_KEY] :0x21" << "\n";
            outFile << "[RESET_KEY] :0x22";
            outFile.close();
        }

        if (!std::filesystem::exists(Stats))
        {
            std::ofstream outFile(Stats);
            if (outFile.is_open())
            {
                outFile << "[HIGHSCORE] :" << "0" << "\n" << "[CLICKS] :" << "0";
                outFile.close();
            }
        }
    }

    // Checks if the txt file exists
    if (!std::filesystem::exists(Stats))
    {
        int txtFileError = MessageBox(NULL, "File not found! Press YES to create new", "File error!", MB_ICONERROR | MB_YESNO);

        if (txtFileError == IDYES)
        {
            std::ifstream inFile(Stats);
            while (true)
            {
                while (!inFile)
                {
                    std::ofstream outFile(Stats);
                    if (outFile.is_open())
                    {
                        outFile << "[HIGHSCORE] :" << "0" << "\n" << "[CLICKS] :" << "0";
                        outFile.close();
                        std::cout << "File created succesfully" << "\n";
                        break;
                    }
                    else 
                    {
                        std::cerr << "Error while creating file" << "\n";
                        Sleep(50);
                    }
                }
                break;
            }
        }
        else if (txtFileError == IDNO)
        {
            std::cout << "No" << "\n";
        }
    }
}

void ClicksPerMinute(int rightClickCounter, std::chrono::seconds seconds)
{
    if (seconds.count() < 60)
    {
        std::cout << "Game was too short" << "\n";
    }

    else
    {
        double minutes = seconds.count() / 60.0;
        double ClicksPerMinute = rightClickCounter / minutes;
        std::cout << "Clicks per minute: " << std::fixed << std::setprecision(2) << ClicksPerMinute << "\n";
    }
}

int main() 
{
    #define IDI_ICON 101
    #define IDI_SMALL 102
    // Icon
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    HICON hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

    // Game and client id
    DWORD GameId = 0;
    DWORD ClientId = 0;

    // Game
    const char* exeName = "League of Legends.exe";
    // Client
    const char* clientName = "LeagueClient.exe";

    // Useless
    auto startTime = std::chrono::steady_clock::now();
    int rightClickCounter = 0;
    int rounds = 1;

    // States
    bool previousState = false;
    bool wasRightClickPressed = false;
    bool WasKeyPressed = false;

    // Files
    const char* Stats = "Stats.txt";
    const char* Settings = "Settings.txt";
    const char* HowToUseFile = "HowToUse.txt";

    // Stats
    int TopScore = SetVariables(Stats, 1);
    int Clicked = SetVariables(Stats, 2);

    // Checks / creates files
    CreateFiles(Stats, Settings, HowToUseFile);

    // Sets keys and stats
    // SetVariables(Stats, TopScore, Clicked);
    SetKeyState(Settings);

    // Binds
    std::cout << "Press " << PrintStats << " to read stats" << "\n";
    std::cout << "Press " << PrintReset << " to reset stats" << "\n";
    std::cout << "Press " << PrintExit << " to exit" << "\n";

    while (isProcessRunning(clientName, ClientId) == true) {
        bool currentState = isProcessRunning(exeName, GameId);

        // Exit function
        if (GetAsyncKeyState(ExitKey) & 1)
        {
            exit(0);
        }
    
        // ReadStats function
        bool IsKeyPressed = (GetAsyncKeyState(StatsKey) & 0x8001) != 0;
        if (WasKeyPressed && !IsKeyPressed) 
        {
            std::cout << "\n" << "Highscore: " << TopScore << "\n" << "Total clicks: " << Clicked << "\n";
        }
        WasKeyPressed = IsKeyPressed;

        // ResetStats function
        if (GetAsyncKeyState(ResetKey) & 1)
        {
            int ResetMessage = MessageBox(NULL, "Are you sure you want to reset stats?", "Confirm!", MB_ICONWARNING | MB_YESNO);

            if (ResetMessage == IDYES)
            {
                StatReset(Stats);
                TopScore = 0;
                Clicked = 0;
                std::cout << "\n" << "Stats reseted successfully!" << "\n";
            }
            else if (ResetMessage == IDNO)
            {
                continue;
            }
        }

        // Main code
        if (currentState != previousState)
        {
            if (currentState)
            {
                std::cout << "\n" << "Game (" << rounds << ") has started." << "\n";
                startTime = std::chrono::steady_clock::now();
                rightClickCounter = 0;
            } 
            else
            {
                auto endTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
                std::cout << "Game has ended." << "\n";
                printGameDuration(duration);
                std::cout << "Total right clicks: " << rightClickCounter << "\n";
                ClicksPerMinute(rightClickCounter, duration);
                rounds++;
                AddStats(rightClickCounter, Stats, Clicked, TopScore);

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