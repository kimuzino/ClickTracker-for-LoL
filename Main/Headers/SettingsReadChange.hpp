void command() {
    std::ifstream inputFile("Commands.dll"); // Change "input.txt" to your file name

    if (!inputFile.is_open()) {
        std::cerr << "Commands.dll missing" << std::endl;
        // Return an error code
    }

    std::unordered_map<std::string, int> variables;
    std::string variable;
    int value;

    // Assuming the file format is "variableName = value"
    while (inputFile >> variable >> value) {
        variables[variable] = value;
    }

    inputFile.close();

    // Print current variable values
    std::cout << "Current Variable Values:" << std::endl;
    for (const auto& pair : variables) {
        std::cout << "Variable: " << pair.first << ", Value: " << pair.second << std::endl;
    }

    // Change the value of a specific variable
    std::cout << "\nEnter the variable name to change its value: ";
    std::cin >> variable;

    if (variables.find(variable) != variables.end()) {
        std::cout << "Enter the new value for " << variable << ": ";
        std::cin >> value;

        // Update the value in the map
        variables[variable] = value;

        // Update the file with the new values
        std::ofstream outputFile("Commands.dll");
        if (outputFile.is_open()) {
            for (const auto& pair : variables) {
                outputFile << pair.first << " " << pair.second << std::endl;
            }
            outputFile.close();
            std::cout << "Value for variable " << variable << " changed successfully." << std::endl;
        } else {
            std::cerr << "Error opening output file." << std::endl;
        }
    } else {
        std::cout << "Variable not found." << std::endl;
    }
}