#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include "testi.cpp"

int PreviousClicked = 0;
int PreviousTopScore = 0;

void Scores(int &Clicked, int &NewTopScore)
{
    std::ofstream stats;
    stats.open("stats.dll");

    PreviousClicked = PreviousClicked + Clicked;

    if (PreviousTopScore <= NewTopScore)
    {
        PreviousTopScore = NewTopScore;
    }

    if (!stats.is_open())
    {
        std::cout << "Error: stats.dll not found" << std::endl;
    }

    else
    {
        stats << std::endl <<"[HIGHSCORE] :" << NewTopScore << std::endl << "[CLICKS] :" << PreviousClicked;
        stats.close();
    }
}

int main()
{
    int Clicked = 0;
    int TopScore = 0;
    while(true)
    {    
        Sleep(50);
        while(!GetAsyncKeyState(VK_SPACE))
        {
            Sleep(50);
            if(GetAsyncKeyState(VK_LBUTTON))
            {

                Clicked++;
                std::cout << Clicked << std::endl;
                std::cout << "Test: " << TopScore << std::endl;
            }
        }

        std::cout << "Reset" << std::endl;
        
        if (Clicked > TopScore)
        {
            TopScore = Clicked;
        }

        Scores(Clicked, TopScore);

        Clicked = 0;
    }
}