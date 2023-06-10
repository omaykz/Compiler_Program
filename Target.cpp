#include <iostream>
#include <fstream>
#include <stack>
#include <string>

std::string convertToAssembly(const std::string& expression) {
    std::stack<std::string> stack;
    std::string assemblyCode;

    for (std::size_t i = 0; i < expression.size(); ++i) {
        char c = expression[i];
        if (c == '=') {
            std::string assignmentValue = stack.top();
            stack.pop();
            std::string variableName = stack.top();
            stack.pop();
            assemblyCode += "mov " + variableName + ", " + assignmentValue + "\n";
        }
        else if (isdigit(c)) {
            std::string numberValue(1, c);
            stack.push(numberValue);
        }
        else if (isalpha(c)) {
            std::string variableName(1, c);
            while (i + 1 < expression.size() && isalnum(expression[i + 1])) {
                variableName += expression[i + 1];
                ++i;
            }
            stack.push(variableName);
        }
        else if (c == '+') {
            std::string operand1 = stack.top();
            stack.pop();
            std::string operand2 = stack.top();
            stack.pop();
            assemblyCode += "push " + operand2 + "\n";
            assemblyCode += "push " + operand1 + "\n";
            assemblyCode += "add\n";
            stack.push("result");
        }
        else if (c == '-') {
            std::string operand1 = stack.top();
            stack.pop();
            std::string operand2 = stack.top();
            stack.pop();
            assemblyCode += "push " + operand2 + "\n";
            assemblyCode += "push " + operand1 + "\n";
            assemblyCode += "sub\n";
            stack.push("result");
        }
        else if (c == '*') {
            std::string operand1 = stack.top();
            stack.pop();
            std::string operand2 = stack.top();
            stack.pop();
            assemblyCode += "push " + operand2 + "\n";
            assemblyCode += "push " + operand1 + "\n";
            assemblyCode += "mul\n";
            stack.push("result");
        }
        else if (c == '/') {
            std::string operand1 = stack.top();
            stack.pop();
            std::string operand2 = stack.top();
            stack.pop();
            assemblyCode += "push " + operand2 + "\n";
            assemblyCode += "push " + operand1 + "\n";
            assemblyCode += "div\n";
            stack.push("result");
        }
    }

    while (!stack.empty()) {
        std::string value = stack.top();
        stack.pop();
        if (value != "result") {
            assemblyCode += "push " + value + "\n";
        }
    }

    return assemblyCode;
}


int main() {
    std::ifstream inputFile("d:/output_TRP.txt");
    if (!inputFile) {
        std::cout << "Error opening input file." << std::endl;
        return 1;
    }

    std::string expression;
    std::getline(inputFile, expression);

    std::string assemblyCode = convertToAssembly(expression);

    std::cout << assemblyCode << std::endl;
    std::ofstream outputFile("d:/output.asm");
    if (!outputFile) {
        std::cout << "Error creating output file." << std::endl;
        inputFile.close();
        return 1;
    }

    outputFile << assemblyCode << std::endl;

    std::cout << "Assembly code has been written to output.asm." << std::endl;

    inputFile.close();
    outputFile.close();
    
    return 0;
}
