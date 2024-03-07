#include <iostream>
#include <fstream>
#include <windows.h>
#include <tlhelp32.h>
#include <chrono>
#include <filesystem>

const char* File = "Stats.txt";

int TopScore;
int Clicked;

bool isProcessRunning(const char* processName, DWORD& processId) 
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);

    if (Process32First(snapshot, &entry)) {
        do {
            if (_stricmp(entry.szExeFile, processName) == 0) {
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

    std::cout << "Duration: " << minutes.count() << " minutes, " << seconds.count() << " seconds." << std::endl;
}

void addToTextFile(int rightClickCounter) 
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

    stats << "[HIGHSCORE] :" << TopScore << std::endl << "[CLICKS] :" << Clicked << std::endl;
    stats.close();
}

void RecreateTXT() {
    std::ifstream inFile(File);
    while (true){
        while (!inFile){
            std::ofstream outFile(File);
            if (outFile.is_open()) {
                outFile << "[HIGHSCORE] :" << "0" << std::endl << "[CLICKS] :" << "0";
                outFile.close();
                std::cout << "File created succesfully" << std::endl;
                break;
            } else {
                std::cerr << "Error while creating file" << std::endl;
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
    if (!std::filesystem::exists(File))
    {
        std::cerr << "Error: " << File << " not found" << std::endl;
    }

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

void ReadStats()
{
    std::cout << "Highscore: " << TopScore << std::endl << "Total clicks: " << Clicked << std::endl;
}

void Help()
{
    std::cout << "Press PgUP to read stats" << std::endl;
    std::cout << "Press PgDN to reset stats" << std::endl;
    std::cout << "Press DELETE to exit" << std::endl;
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

    stats << "[HIGHSCORE] :" << "0" << std::endl << "[CLICKS] :" << "0" << std::endl;
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

    bool wasRightClickPressed = false;

    // Checks if the txt file exists
    if (!CheckTXT())
    {
        int txtFileError = MessageBox(NULL, "File not found! Press YES to create new", "File error!", MB_ICONERROR | MB_YESNO);

        if (txtFileError == IDYES) {
            RecreateTXT();
        }
        else if (txtFileError == IDNO) {
            std::cout << "No" << std::endl;
        }
    }

    // Set the values for the variables
    SetVariables();

    Help();

    while (true) {
        bool currentState = isProcessRunning(exeName, processId);

        // DEL key to exit
        if (GetAsyncKeyState(VK_DELETE) & 1)
        {
            exit(0);
        }
        
        // PgUP key to read stats
        if (GetAsyncKeyState(VK_PRIOR) & 1)
        {
            ReadStats();
            Sleep(50);
        }

        // PgDN to reset stats
        if (GetAsyncKeyState(VK_NEXT) & 1)
        {
            int ResetMessage = MessageBox(NULL, "Are you sure you want to reset stats?", " ", MB_ICONWARNING | MB_YESNO);

            if (ResetMessage == IDYES) {
                StatReset();
                std::cout << "Stats reseted successfully!" << std::endl;
            }
            else if (ResetMessage == IDNO) {
                continue;
            }
        }

        if (currentState != previousState) {
            if (currentState) {
                std::cout << "Game (" << rounds << ") has started." << std::endl;
                startTime = std::chrono::steady_clock::now();
                rightClickCounter = 0;
            } else {
                auto endTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
                std::cout << "Game has ended." << std::endl;
                printDuration(duration);
                std::cout << "Total right clicks: " << rightClickCounter << std::endl << std::endl;
                rounds++;
                addToTextFile(rightClickCounter);
            }
            previousState = currentState;
        }

        bool isRightClickPressed = (GetAsyncKeyState(VK_RBUTTON) & 0x8001) != 0;
        if (wasRightClickPressed && !isRightClickPressed) {
            rightClickCounter++;
        }

        wasRightClickPressed = isRightClickPressed;
    }
    return 0;
}

// average Rightclicks per minute