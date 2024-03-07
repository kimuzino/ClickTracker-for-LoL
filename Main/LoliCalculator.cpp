#include "Headers/include.hpp"
#include "Headers/SettingsReadChange.hpp"
const char* File = "Stats.txt";

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
    stats.open("stats.dll");
    std::string line;

    Clicked = Clicked + rightClickCounter;

    if (TopScore < rightClickCounter)
    {
        TopScore = rightClickCounter;
    }

    if (!stats.is_open())
    {
        std::cout << "Error: stats.dll not found" << std::endl;
    }

    stats << "[HIGHSCORE] :" << TopScore << std::endl << "[CLICKS] :" << Clicked << std::endl;
    stats.close();
}

void RecreateDLL() {
    std::ifstream inFile(File);
    if (!inFile) {
        std::cerr << "Error opening the file." << std::endl << "Do you want to create a new file? (y/n)" << std::endl;
        while (true){
            if (GetAsyncKeyState(0x59) & 1) {
                while (!inFile){
                    std::ofstream outFile(File);
                    if (outFile.is_open()) {
                        outFile << std::endl <<"[HIGHSCORE] :" << "0" << std::endl << "[CLICKS] :" << "0";
                        outFile.close();
                        std::cout << "File created succesfully" << std::endl;
                        break;
                    } else {
                        std::cerr << "Error opening the file." << std::endl;
                    }
                }
                break;
            }

            if (GetAsyncKeyState(0x4E) & 1) {
                std::cout << "File not created" << std::endl << "";
                break;
            }
        }

    }
    else {
        std::cout << "File found" << std::endl;
    }
}
// Works
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

// Works
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

//onexit()

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
        int txtFileError = MessageBox(NULL, "File not found (Stats.txt)", "Title!", MB_ICONERROR | MB_YESNO);

        if (txtFileError == IDYES) {
            RecreateDLL();
        }
        else if (txtFileError == IDNO) {
            std::cout << "No" << std::endl;
        }
    }

    SetVariables();

    return 0;

    while (true) {
        bool currentState = isProcessRunning(exeName, processId);

        // add a hotkey to check if dll is still there


        if (GetAsyncKeyState(0x4c) & 1) {
            std::cout << "Test";
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

