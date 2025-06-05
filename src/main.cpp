#include <iostream>
#include <string>
#include <vector>
#include <memory> // For std::unique_ptr

#include "CreateNewForm.h"
#include "DeleteForm.h"
#include "SelectAndUseForm.h"
#include "AddEntry.h"
#include "EditEntry.h"
#include "ViewEntry.h"
#include "DeleteEntry.h"
#include "SaveAsCSV.h"
#include "SaveAsJSON.h"
#include "SaveAsSQL.h"
#include "FormDefinition.h" // For currentSelectedForm
#include "Entry.h"          // For EntryManager

// Global variable to hold the currently selected form (declared in FormDefinition.h)
// std::shared_ptr<FormDefinition> currentSelectedForm; // Already declared in FormDefinition.h

// Global EntryManager instance, initialized when a form is selected (defined in Entry.cpp)
extern std::unique_ptr<EntryManager> currentEntryManager;

void saveAs(); // Declare saveAs function prototype

int main() {
    int choice;
    do {
        std::cout << "\n--- Todo App Menu ---\n";
        std::cout << "1. Create New Form\n";
        std::cout << "2. Delete Form\n";
        std::cout << "3. Select and Use Form\n";
        std::cout << "4. Add New Entry\n";
        std::cout << "5. Edit Entry\n";
        std::cout << "6. View Entry\n";
        std::cout << "7. Delete Entry\n";
        std::cout << "8. Save As\n";
        std::cout << "9. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                createNewForm();
                break;
            case 2:
                deleteForm();
                break;
            case 3:
                selectAndUseForm();
                break;
            case 4:
                addEntry();
                break;
            case 5:
                editEntry();
                break;
            case 6:
                viewEntry();
                break;
            case 7:
                deleteEntry();
                break;
            case 8:
                saveAs();
                break;
            case 9:
                std::cout << "Exiting Todo App. Goodbye!\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 9);

    return 0;
}

void saveAs() {
    if (!currentSelectedForm) {
        std::cout << "No form is currently selected. Please select a form first (Option 3).\n";
        return;
    }

    // If the form changes, or if it's the first time, create/recreate the EntryManager
    if (!currentEntryManager || currentEntryManager->getFormName() != currentSelectedForm->name) {
        currentEntryManager = std::make_unique<EntryManager>(currentSelectedForm->name);
    }

    if (currentEntryManager->getEntries().empty()) {
        std::cout << "No entries to save for the current form.\n";
        return;
    }

    int saveChoice;
    std::cout << "\n--- Save Entries As ---\n";
    std::cout << "1. Save as CSV\n";
    std::cout << "2. Save as JSON\n";
    std::cout << "3. Save as SQL\n";
    std::cout << "Enter your choice: ";
    std::cin >> saveChoice;
    std::cin.ignore(); // Clear the buffer

    std::string outputFilename = "Forms/" + currentSelectedForm->name + "_entries";

    switch (saveChoice) {
        case 1:
            saveAsCSV(outputFilename + ".csv", currentSelectedForm, currentEntryManager->getEntries());
            break;
        case 2:
            saveAsJSON(outputFilename + ".json", currentSelectedForm, currentEntryManager->getEntries());
            break;
        case 3:
            saveAsSQL(outputFilename + ".sql", currentSelectedForm, currentEntryManager->getEntries());
            break;
        default:
            std::cout << "Invalid choice. No entries saved.\n";
            break;
    }
}
