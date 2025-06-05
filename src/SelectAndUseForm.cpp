#include "SelectAndUseForm.h"
#include "FormDefinition.h" // To use FormDefinition and currentSelectedForm
#include <iostream>
#include <string>
#include <vector>
#include <filesystem> // For listing files

namespace fs = std::filesystem;

void selectAndUseForm() {
    std::cout << "\n--- Select and Use Form ---\n";
    std::string formsDir = "Forms/";

    // Check if Forms directory exists
    if (!fs::exists(formsDir) || !fs::is_directory(formsDir)) {
        std::cout << "No forms directory found. Please create a form first.\n";
        return;
    }

    std::vector<std::string> formFiles;
    std::cout << "Available Forms:\n";
    int i = 1;
    for (const auto& entry : fs::directory_iterator(formsDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".form") {
            formFiles.push_back(entry.path().filename().string());
            std::cout << i++ << ". " << entry.path().filename().string() << "\n";
        }
    }

    if (formFiles.empty()) {
        std::cout << "No forms found to select.\n";
        return;
    }

    int choice;
    std::cout << "Enter the number of the form to use (0 to cancel): ";
    std::cin >> choice;

    if (choice > 0 && choice <= formFiles.size()) {
        std::string formPath = formsDir + formFiles[choice - 1];
        currentSelectedForm = FormDefinition::loadFromFile(formPath);
        if (currentSelectedForm) {
            std::cout << "Form '" << currentSelectedForm->name << "' selected and loaded successfully.\n";
            std::cout << "Form fields:\n";
            for (const auto& field : currentSelectedForm->fields) {
                std::cout << "- " << field->name << " (" << field->type << ")\n";
                if (field->type == "select") {
                    auto selectField = std::static_pointer_cast<SelectField>(field);
                    for (const auto& option : selectField->options) {
                        std::cout << "    " << option.first << ": " << option.second << "\n";
                    }
                }
            }
        } else {
            std::cout << "Failed to load form '" << formFiles[choice - 1] << "'.\n";
        }
    } else if (choice == 0) {
        std::cout << "Form selection cancelled.\n";
    } else {
        std::cout << "Invalid choice.\n";
    }
}
