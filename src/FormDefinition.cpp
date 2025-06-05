#include "FormDefinition.h"
#include <fstream>
#include <iostream>
#include <sstream>

// Initialize the global variable
std::shared_ptr<FormDefinition> currentSelectedForm = nullptr;

std::shared_ptr<FormDefinition> FormDefinition::loadFromFile(const std::string& filename) {
    auto formDef = std::make_shared<FormDefinition>();
    formDef->name = filename.substr(filename.find_last_of('/') + 1); // Extract name from path
    if (formDef->name.length() > 5 && formDef->name.substr(formDef->name.length() - 5) == ".form") {
        formDef->name = formDef->name.substr(0, formDef->name.length() - 5);
    }

    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open form file " << filename << std::endl;
        return nullptr;
    }

    std::string line;
    std::shared_ptr<SelectField> currentSelectField = nullptr;

    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        std::string type;
        std::getline(ss, type, ':');

        if (type == "string") {
            std::string name;
            std::getline(ss, name);
            formDef->fields.push_back(std::make_shared<StringField>(name));
            currentSelectField = nullptr; // Reset current select field
        } else if (type == "number") {
            std::string name, numType;
            std::getline(ss, name, ':');
            std::getline(ss, numType);
            formDef->fields.push_back(std::make_shared<NumberField>(name, numType));
            currentSelectField = nullptr; // Reset current select field
        } else if (type == "select") {
            std::string name;
            std::getline(ss, name);
            currentSelectField = std::make_shared<SelectField>(name);
            formDef->fields.push_back(currentSelectField);
        } else if (type == "  option" && currentSelectField) { // Note the two spaces for indentation
            int num;
            std::string optionText;
            char colon; // To consume the colon
            ss >> num >> colon; // Read number and colon
            std::getline(ss, optionText); // Read the rest of the line as option text
            currentSelectField->options[num] = optionText;
        }
    }

    inFile.close();
    return formDef;
}
