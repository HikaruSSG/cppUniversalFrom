#include "DeleteForm.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem> // For listing and deleting files

namespace fs = std::filesystem;

void deleteForm() {
    std::cout << "\n--- Delete Form ---\n";
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
        std::cout << "No forms found to delete.\n";
        return;
    }

    int choice;
    std::cout << "Enter the number of the form to delete (0 to cancel): ";
    std::cin >> choice;

    if (choice > 0 && choice <= formFiles.size()) {
        std::string formToDelete = formsDir + formFiles[choice - 1];
        try {
            fs::remove(formToDelete);
            std::cout << "Form '" << formFiles[choice - 1] << "' deleted successfully.\n";
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error deleting form: " << e.what() << std::endl;
        }
    } else if (choice == 0) {
        std::cout << "Form deletion cancelled.\n";
    } else {
        std::cout << "Invalid choice.\n";
    }
}
