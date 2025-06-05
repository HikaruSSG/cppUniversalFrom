#include "gtest/gtest.h"
#include "CreateNewForm.h"
#include <fstream>
#include <string>
#include <sstream>

void createNewFormWithInput(const std::string& formName) {
    // Redirect input to provide the form name
    std::stringstream input;
    input << formName << std::endl; // Form name
    input << "4" << std::endl;      // Finish form creation

    std::cin.rdbuf(input.rdbuf()); // Redirect std::cin to the stringstream

    createNewForm();

    // Restore std::cin
    std::cin.rdbuf(std::cin.tie()->rdbuf());
}

TEST(CreateNewFormTest, CreateNewForm) {
    // Test case: Create a new form and check if the file is created.
    std::string formName = "test_form";
    std::string formFilePath = "Forms/" + formName + ".form";

    // Remove the form file if it exists.
    std::remove(formFilePath.c_str());

    createNewFormWithInput(formName);

    // Check if the form file exists.
    std::ifstream formFile(formFilePath);
    ASSERT_TRUE(formFile.good());

    // Clean up: Remove the form file.
    std::remove(formFilePath.c_str());
}
