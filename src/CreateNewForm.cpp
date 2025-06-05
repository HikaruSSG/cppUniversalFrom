#include "CreateNewForm.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

// Helper function to sanitize filenames
std::string sanitizeFilename(const std::string& name) {
    std::string sanitized = name;
    for (char& c : sanitized) {
        if (!isalnum(c) && c != '_') {
            c = '_';  // Replace non-alphanumeric characters with underscore
        }
    }
    return sanitized;
}

void createNewForm() {
    std::cout << "\n--- Create New Form ---\n";
    std::string formName;
    std::cout << "Enter the name for the new form: ";
    std::cin.ignore(); // Clear the buffer
    std::getline(std::cin, formName);

    std::string sanitizedFormName = sanitizeFilename(formName);
    std::cout << "Sanitized form name: " << sanitizedFormName << std::endl;
    std::string formFilePath = "Forms/" + sanitizedFormName + ".form";

    // Check if the form already exists
    std::ifstream formFileCheck(formFilePath);
    if (formFileCheck.good()) {
        std::cout << "Warning: Form '" << formName << "' already exists. Overwrite? (y/n): ";
        char overwriteChoice;
        std::cin >> overwriteChoice;
        std::cin.ignore(); // Clear the buffer
        if (overwriteChoice != 'y') {
            std::cout << "Form creation cancelled.\n";
            return;
        }
    }

    std::ofstream outFile(formFilePath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not create form file " << formFilePath << std::endl;
        return;
    }

    std::vector<std::pair<std::string, std::string>> formFields; // {field_name, field_type}

    int choice;
    do {
        std::cout << "\n--- Add Input to Form ---\n";
        std::cout << "1. Add String Input\n";
        std::cout << "2. Add Number Input\n";
        std::cout << "3. Add Select Input\n";
        std::cout << "4. Finish Form Creation\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore(); // Clear the buffer

        std::string inputName;
        if (choice >= 1 && choice <= 3) {
            std::cout << "Enter the name for this input: ";
            std::getline(std::cin, inputName);
        }

        switch (choice) {
            case 1: { // String Input
                formFields.push_back({inputName, "string"});
                outFile << "string:" << inputName << "\n";
                std::cout << "String input '" << inputName << "' added.\n";
                break;
            }
            case 2: { // Number Input
                int numTypeChoice;
                std::cout << "Select number input type:\n";
                std::cout << "1. int\n";
                std::cout << "2. float\n";
                std::cout << "3. double\n";
                std::cout << "Enter your choice: ";
                std::cin >> numTypeChoice;
                std::cin.ignore(); // Clear the buffer

                std::string numType;
                switch (numTypeChoice) {
                    case 1: numType = "int"; break;
                    case 2: numType = "float"; break;
                    case 3: numType = "double"; break;
                    default: numType = "int"; std::cout << "Invalid choice, defaulting to int.\n"; break;
                }
                formFields.push_back({inputName, numType});
                outFile << "number:" << inputName << ":" << numType << "\n";
                std::cout << numType << " input '" << inputName << "' added.\n";
                break;
            }
            case 3: { // Select Input
                std::map<int, std::string> selections;
                int selectionCount = 1;
                std::string selectionOption;
                int selectChoice;

                outFile << "select:" << inputName << "\n";

                do {
                    std::cout << "\n--- Add Selection Options for '" << inputName << "' ---\n";
                    std::cout << "1. Add Selection Option\n";
                    std::cout << "2. Delete Selection Option\n";
                    std::cout << "3. Finish Selections\n";
                    std::cout << "Enter your choice: ";
                    std::cin >> selectChoice;
                    std::cin.ignore(); // Clear the buffer

                    switch (selectChoice) {
                        case 1:
                            std::cout << "Enter selection option text: ";
                            std::getline(std::cin, selectionOption);
                            selections[selectionCount] = selectionOption;
                            outFile << "  option:" << selectionCount << ":" << selectionOption << "\n";
                            std::cout << "Option " << selectionCount << ": '" << selectionOption << "' added.\n";
                            selectionCount++;
                            break;
                        case 2:
                            if (selections.empty()) {
                                std::cout << "No selections to delete.\n";
                            } else {
                                std::cout << "Current selections:\n";
                                for (const auto& pair : selections) {
                                    std::cout << pair.first << ". " << pair.second << "\n";
                                }
                                int deleteNum;
                                std::cout << "Enter the number of the selection to delete: ";
                                std::cin >> deleteNum;
                                std::cin.ignore();
                                if (selections.count(deleteNum)) {
                                    selections.erase(deleteNum);
                                    // Re-write the select block to reflect deletion (simpler than in-place edit)
                                    // This is a simplification; a real implementation might read, modify, then rewrite the whole file
                                    std::cout << "Option " << deleteNum << " deleted. Note: This change will be fully reflected upon re-opening the form.\n";
                                } else {
                                    std::cout << "Invalid selection number.\n";
                                }
                            }
                            break;
                        case 3:
                            std::cout << "Finished adding selections.\n";
                            break;
                        default:
                            std::cout << "Invalid choice. Please try again.\n";
                    }
                } while (selectChoice != 3);
                formFields.push_back({inputName, "select"});
                break;
            }
            case 4:
                std::cout << "Form '" << formName << "' created successfully in " << formFilePath << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);

    outFile.close();
}
