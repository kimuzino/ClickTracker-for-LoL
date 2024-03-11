#include <iostream>
#include <fstream>
#include <windows.h>
#include <tlhelp32.h>
#include <chrono>
#include <filesystem>

#include "KeyCodes.cpp"

const char* File = "Stats.txt";
const char* Settings = "Settings.txt";
const char* ReadmeFile = "Readme.txt";

int TopScore;
int Clicked;

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

void printDuration(std::chrono::seconds seconds) 
{
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(seconds);
    seconds -= std::chrono::duration_cast<std::chrono::seconds>(minutes);

    std::cout << "Duration: " << minutes.count() << " minutes, " << seconds.count() << " seconds." << "\n";
}

void AddStats(int rightClickCounter) 
{
    std::ofstream stats;
    stats.open(File);
    std::string line;

    Clicked = Clicked + rightClickCounter;

    if (TopScore < rightClickCounter)
    {
        TopScore = rightClickCounter;
    }

    if (!stats.is_open())
    {
        MessageBox(NULL, "Error while opening file", "File error!", MB_ICONERROR);
    }

    stats << "[HIGHSCORE] :" << TopScore << "\n" << "[CLICKS] :" << Clicked << "\n";
    stats.close();
}

void RecreateTXT()
{
    std::ifstream inFile(File);
    while (true)
    {
        while (!inFile)
        {
            std::ofstream outFile(File);
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

bool CheckTXT()
{
    std::ifstream TestFile(File);
    if (!TestFile)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void SetVariables() 
{ 
    // Takes the highscore and clicks value
    std::fstream FromFile(File, std::ios::in);

    std::string line;
    while (std::getline(FromFile, line))
    {
        if (line.find("[HIGHSCORE] :") != std::string::npos)
        {
            std::streampos startPos = FromFile.tellg();

            std::string highscoreStr = line.substr(line.find(":") + 1);
            TopScore = std::stoi(highscoreStr);
        }

        if (line.find("[CLICKS] :") != std::string::npos)
        {
            std::streampos startPos = FromFile.tellg();

            std::string clicksStr = line.substr(line.find(":") + 1);
            Clicked = std::stoi(clicksStr);
        }
    }
    FromFile.close();
}

void SetKeyState()
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

void ReadStats()
{
    std::cout << "\n" << "Highscore: " << TopScore << "\n" << "Total clicks: " << Clicked << "\n";
}

void Help()
{
    std::cout << "Press " << PrintStats << " to read stats" << "\n";
    std::cout << "Press " << PrintReset << " to reset stats" << "\n";
    std::cout << "Press " << PrintExit << " to exit" << "\n";
}

void StatReset()
{
    std::ofstream stats;
    stats.open(File);
    std::string line;

    if (!stats.is_open())
    {
        MessageBox(NULL, "Error while opening file", "File error!", MB_ICONERROR);
    }

    stats << "[HIGHSCORE] :" << "0" << "\n" << "[CLICKS] :" << "0" << "\n";
    stats.close();

    SetVariables();
}

int main() 
{
    const char* exeName = "League of Legends.exe";

    DWORD processId = 0;
    bool previousState = false;
    auto startTime = std::chrono::steady_clock::now();
    int rightClickCounter = 0;
    std::string userInput;
    int rounds = 1;
    std::string FileCheck;

    bool wasRightClickPressed = false;
    bool WasKeyPressed = false;

    // All needed to know how the code calculator works
    if (!std::filesystem::exists(ReadmeFile))
    {
        std::ofstream CreateReadmeFile(ReadmeFile);
        if (CreateReadmeFile.is_open()) {
            CreateReadmeFile << "All key codes here: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes" << "\n";
            CreateReadmeFile << "Remember to only use the hex value for mapping keys." << "\n";
            CreateReadmeFile << "Shift, Ctrl, Alt, Caps Lock, Windows Key and few other keys isn't supported." << "\n";
            CreateReadmeFile << "You can map more keys by modifying the KeyCodes.cpp file." << "\n";
            CreateReadmeFile.close();
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

        if (!std::filesystem::exists(File))
        {
            std::ofstream outFile(File);
            if (outFile.is_open())
            {
                outFile << "[HIGHSCORE] :" << "0" << "\n" << "[CLICKS] :" << "0";
                outFile.close();
            }
        }
    }

    // Checks if the txt file exists
    if (!CheckTXT())
    {
        int txtFileError = MessageBox(NULL, "File not found! Press YES to create new", "File error!", MB_ICONERROR | MB_YESNO);

        if (txtFileError == IDYES)
        {
            RecreateTXT();
        }
        else if (txtFileError == IDNO)
        {
            std::cout << "No" << "\n";
        }
    }

    // Sets important values
    SetVariables();
    SetKeyState();

    Help();

    while (true) {
        bool currentState = isProcessRunning(exeName, processId);

        if (GetAsyncKeyState(ExitKey) & 1)
        {
            exit(0);
        }
    
        bool IsKeyPressed = (GetAsyncKeyState(StatsKey) & 0x8001) != 0;
        if (WasKeyPressed && !IsKeyPressed) 
        {
            ReadStats();
        }
        WasKeyPressed = IsKeyPressed;

        if (GetAsyncKeyState(ResetKey) & 1)
        {
            int ResetMessage = MessageBox(NULL, "Are you sure you want to reset stats?", "Confirm!", MB_ICONWARNING | MB_YESNO);

            if (ResetMessage == IDYES) {
                StatReset();
                std::cout << "\n" << "Stats reseted successfully!" << "\n";
            }
            else if (ResetMessage == IDNO) {
                continue;
            }
        }

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
                printDuration(duration);
                std::cout << "Total right clicks: " << rightClickCounter << "\n";
                rounds++;
                AddStats(rightClickCounter);
            }
            previousState = currentState;
        }

        bool isRightClickPressed = (GetAsyncKeyState(VK_RBUTTON) & 0x8001) != 0;
        if (wasRightClickPressed && !isRightClickPressed)
        {
            rightClickCounter++;
        }

        wasRightClickPressed = isRightClickPressed;
    }
    return 0;
}

// average Rightclicks per minute