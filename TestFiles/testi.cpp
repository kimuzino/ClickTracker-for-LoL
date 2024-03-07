#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

 main()
{
    std::string filename = "test.txt";
    
    if (!std::filesystem::exists(filename)) // Check if the file exists
    {
        std::cerr << "Error: " << filename << " not found" << std::endl;
        return 1; // Add return statement to exit the program if the file is not found
    }

    std::fstream file(filename, std::ios::in | std::ios::out);

    std::string line;
    while(std::getline(file, line))
    {
        if(line.find("[HIGHSCORE] :") != std::string::npos)
        {
            std::streampos startPos = file.tellg();

            std::string highscoreStr = line.substr(line.find(":") + 1);
            int highscore = std::stoi(highscoreStr);

            std::cout << "Highscore: " << highscore << std::endl;

            break;  
        }
    }


    file.close();
    return 0;
}