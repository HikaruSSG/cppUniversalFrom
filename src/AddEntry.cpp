#include "AddEntry.h"
#include "FormDefinition.h" // For currentSelectedForm
#include "Entry.h"          // For EntryManager
#include <iostream>

extern std::unique_ptr<EntryManager> currentEntryManager; // Declare extern

void addEntry() {
    if (!currentSelectedForm) {
        std::cout << "No form is currently selected. Please select a form first (Option 3).\n";
        return;
    }

    // If the form changes, or if it's the first time, create/recreate the EntryManager
    if (!currentEntryManager || currentEntryManager->getFormName() != currentSelectedForm->name) {
        currentEntryManager = std::make_unique<EntryManager>(currentSelectedForm->name);
    }

    currentEntryManager->addEntry(currentSelectedForm);
}
