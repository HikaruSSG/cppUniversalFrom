#include "ViewEntry.h"
#include "FormDefinition.h" // For currentSelectedForm
#include "Entry.h"          // For currentEntryManager
#include <iostream>
#include <limits> // For numeric_limits

extern std::unique_ptr<EntryManager> currentEntryManager; // Declare extern

void viewEntry() {
    if (!currentSelectedForm) {
        std::cout << "No form is currently selected. Please select a form first (Option 3).\n";
        return;
    }

    if (!currentEntryManager || currentEntryManager->getFormName() != currentSelectedForm->name) {
        currentEntryManager = std::make_unique<EntryManager>(currentSelectedForm->name);
    }

    if (currentEntryManager->getEntries().empty()) {
        std::cout << "No entries to display for the current form.\n";
        return;
    }

    int currentPage = 1;
    int entriesPerPage = 5;
    int totalEntries = currentEntryManager->getEntries().size();
    int totalPages = (totalEntries + entriesPerPage - 1) / entriesPerPage;

    std::string navChoice;
    do {
        currentEntryManager->viewEntries(currentPage, entriesPerPage);

        if (totalPages > 1) {
            std::cout << "Enter 'n' for next page, 'p' for previous page, or 'q' to quit viewing: ";
            std::cin >> navChoice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

            if (navChoice == "n" || navChoice == "N") {
                if (currentPage < totalPages) {
                    currentPage++;
                } else {
                    std::cout << "Already on the last page.\n";
                }
            } else if (navChoice == "p" || navChoice == "P") {
                if (currentPage > 1) {
                    currentPage--;
                } else {
                    std::cout << "Already on the first page.\n";
                }
            } else if (navChoice != "q" && navChoice != "Q") {
                std::cout << "Invalid navigation choice.\n";
            }
        } else {
            navChoice = "q"; // No navigation needed if only one page
        }

    } while (navChoice != "q" && navChoice != "Q");
}
