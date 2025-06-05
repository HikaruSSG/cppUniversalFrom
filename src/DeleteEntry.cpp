#include "DeleteEntry.h"
#include "FormDefinition.h" // For currentSelectedForm
#include "Entry.h"          // For currentEntryManager
#include <iostream>
#include <limits> // For numeric_limits

extern std::unique_ptr<EntryManager> currentEntryManager; // Declare extern

void deleteEntry() {
    if (!currentSelectedForm) {
        std::cout << "No form is currently selected. Please select a form first (Option 3).\n";
        return;
    }

    if (!currentEntryManager || currentEntryManager->getFormName() != currentSelectedForm->name) {
        currentEntryManager = std::make_unique<EntryManager>(currentSelectedForm->name);
    }

    if (currentEntryManager->getEntries().empty()) {
        std::cout << "No entries to delete for the current form.\n";
        return;
    }

    int keyToDelete;
    std::cout << "Enter the key of the entry to delete: ";
    std::cin >> keyToDelete;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    currentEntryManager->deleteEntry(keyToDelete);
}
