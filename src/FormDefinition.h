#ifndef FORM_DEFINITION_H
#define FORM_DEFINITION_H

#include <string>
#include <vector>
#include <map>
#include <memory> // For std::shared_ptr

// Base class for form fields
struct FormField {
    std::string name;
    std::string type; // "string", "number", "select"

    FormField(const std::string& n, const std::string& t) : name(n), type(t) {}
    virtual ~FormField() = default;
};

// Derived class for String fields
struct StringField : public FormField {
    StringField(const std::string& n) : FormField(n, "string") {}
};

// Derived class for Number fields
struct NumberField : public FormField {
    std::string numberType; // "int", "float", "double"
    NumberField(const std::string& n, const std::string& nt) : FormField(n, "number"), numberType(nt) {}
};

// Derived class for Select fields
struct SelectField : public FormField {
    std::map<int, std::string> options;
    SelectField(const std::string& n) : FormField(n, "select") {}
};

// Structure to hold the definition of a form
struct FormDefinition {
    std::string name;
    std::vector<std::shared_ptr<FormField>> fields;

    // Function to load a form definition from a file
    static std::shared_ptr<FormDefinition> loadFromFile(const std::string& filename);
};

// Global variable to hold the currently selected form
extern std::shared_ptr<FormDefinition> currentSelectedForm;

#endif // FORM_DEFINITION_H
