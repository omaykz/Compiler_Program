#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "Token.h"


class Lexer {
public:
    Lexer(const std::string& input) : input_(input), pos_(0), line_(1), column_(1) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;

        while (pos_ < input_.size()) {
            skipWhitespace();

            if (pos_ >= input_.size()) {
                // 输入结束
                break;
            }

            char currentChar = input_[pos_];

            if (isLetter(currentChar)) {
                // 标识符或关键字
                std::string identifier = readIdentifier();

                if (currentChar == '+' || currentChar == '-') {
                    // 双字符运算符++
                    if (input_[pos_ + 1] == currentChar) {
                        std::string operatorSymbol = identifier + currentChar + currentChar;
                        tokens.push_back({ operatorMap[operatorSymbol], operatorSymbol, line_, column_ });
                        pos_ += 2;
                        // 读取并忽略下一个字符，因为已经处理过了
                        currentChar = input_[++pos_];
                    }
                    else {
                        // 普通的标识符
                        tokens.push_back({ TokenCode::Identifier, identifier, line_, column_ });
                    }
                }
                else {
                    // 普通的标识符
                    tokens.push_back({ TokenCode::Identifier, identifier, line_, column_ });
                }
            }



            else if (isDigit(currentChar)) {
                // 整数或数组
                std::string number = readNumber();
                if (input_[pos_] == '[' && input_[pos_ + 1] == ']') {
                    // 数组
                    pos_ += 2; // 跳过 '[' 和 ']'
                    int arraySize = std::stoi(number);
                    registerArrayIdentifier(number, arraySize);
                    tokens.push_back({ TokenCode::Identifier, number, line_, column_ });
                    tokens.push_back({ TokenCode::Delimiter, "[", line_, column_ });
                    tokens.push_back({ TokenCode::Delimiter, "]", line_, column_ });
                }
                else {
                    // 整数
                    tokens.push_back({ TokenCode::Integer, number, line_, column_ });
                }
            }
            
          
            else if (operatorMap.count(std::string(1, currentChar)) > 0) {
                // 操作符
                std::string op = std::string(1, currentChar);
                tokens.push_back({ operatorMap[op], op, line_, column_ });
                pos_++;
            }
            else if (currentChar == '(') {
                tokens.push_back({ TokenCode::Delimiter, "(", line_, column_ });
                pos_++;
            }
            else if (currentChar == ')') {
                tokens.push_back({ TokenCode::Delimiter, ")", line_, column_ });
                pos_++;
            }
            else if (currentChar == ';') {
                tokens.push_back({ TokenCode::Delimiter, ";", line_, column_ });
                pos_++;
            }
            else if (currentChar == '{') {
                tokens.push_back({ TokenCode::Delimiter, "{", line_, column_ });
                pos_++;
            }
            else if (currentChar == '}') {
                tokens.push_back({ TokenCode::Delimiter, "}", line_, column_ });
                pos_++;
            }
            else if (currentChar == 'i' && input_[pos_+1] == 'f') {
                // if关键字
                tokens.push_back({ TokenCode::Keyword, "if", line_, column_ });
                pos_ += 2; // 跳过'i'和'f'
            }
            else if (currentChar == 'e' && input_[pos_ + 1] == 'l' && input_[pos_ + 2] == 's' && input_[pos_ +3] == 'e') {
                // else关键字
                tokens.push_back({ TokenCode::Keyword, "else", line_, column_ });
                pos_ += 4; // 跳过'e'、'l'、's'和'e'
            }


            else {
                // 错误字符
                tokens.push_back({ TokenCode::Error, std::string(1, currentChar), line_, column_ });
                pos_++;
            }
        }

        return tokens;
    }

    void printSymbolTable() const {
        std::cout << "Symbol Table:" << std::endl;
        for (const auto& entry : symbolTable_) {
            std::cout << entry.first << std::endl;
        }
    }

private:
    std::string input_;
    size_t pos_;
    int line_;
    int column_;
    std::unordered_map<std::string, int> symbolTable_;

    void skipWhitespace() {
        while (pos_ < input_.size() && isWhitespace(input_[pos_])) {
            if (input_[pos_] == '\n') {
                line_++;
                column_ = 1;
            }
            else {
                column_++;
            }
            pos_++;
        }
    }

    bool isLetter(char c) {
        return std::isalpha(c);
    }

    bool isDigit(char c) {
        return std::isdigit(c);
    }
   

    bool isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\n';
    }

    std::string readIdentifier() {
        std::string identifier;
        while (pos_ < input_.size() && (isLetter(input_[pos_]) || isDigit(input_[pos_]))) {
            identifier += input_[pos_++];
        }
        return identifier;
    }

    std::string readNumber() {
        std::string number;
        while (pos_ < input_.size() && isDigit(input_[pos_])) {
            number += input_[pos_++];
        }
        return number;
    }

    std::string readStringLiteral() {
        std::string result;

        // 跳过起始引号
        ++pos_;

        while (pos_ < input_.size()) {
            char currentChar = input_[pos_];

            if (currentChar == '\\') {
                // 处理转义字符
                if (pos_ + 1 < input_.size()) {
                    ++pos_;
                    char escapedChar = input_[pos_];

                    switch (escapedChar) {
                    case 'n':
                        result += '\n';
                        break;
                    case 't':
                        result += '\t';
                        break;
                        // 处理其他转义字符...
                    default:
                        result += escapedChar;
                        break;
                    }
                }
            }
            else if (currentChar == '\"') {
                // 遇到结束引号，停止读取
                ++pos_;
                break;
            }
            else {
                result += currentChar;
            }

            ++pos_;
        }

        return result;
    }


    void registerArrayIdentifier(const std::string& identifier, int arraySize) {
        symbolTable_[identifier] = arraySize;
    }
};

int main() {
    std::string filename = "d:/source_code.txt";  // 输入文件名

    // 读取文件内容
    std::ifstream inputFile(filename);
    std::string sourceCode;
    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            sourceCode += line + "\n";
        }
        inputFile.close();
    }
    else {
        std::cerr << "无法打开文件" << std::endl;
        return 1;
    }

    // 词法分析
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    // 输出词法分析结果
    for (const auto& token : tokens) {
        std::cout << "单词: " << token.value << " 二元序列: " << static_cast<int>(token.code)
            << " 类型: " << static_cast<int>(token.code) << " 位置: (" << token.line << ", " << token.column << ")"
            << std::endl;
    }
    std::string filename1 = "d:/tokens.txt";  // 指定输出文件名

    // 打开输出文件
    std::ofstream outputFile(filename1);
    if (!outputFile.is_open()) {
        std::cerr << "无法打开输出文件" << std::endl;
        return 1;
    }

    // 词法分析
    // ...

    // 输出词法分析结果到文件
    for (const auto& token : tokens) {
        /*   outputFile << << "单词: " << token.value << " 二元序列: " << static_cast<int>(token.code)
            << " 类型: " << static_cast<int>(token.code) << " 位置: (" << token.line << ", " << token.column << ")"
            << std::endl;
       }
       */

        outputFile << " TokenType::";
            switch (token.code) {
           
        case TokenCode::Identifier:
            outputFile << "Identifier";
            break;
        case TokenCode::Integer:
            outputFile << "Integer";
            break;
        case TokenCode::Operator:
            outputFile << "Operator";
            break;
        case TokenCode::Delimiter:
            outputFile << "Delimiter";
            break;
        case TokenCode::Keyword:
            outputFile << "Keyword";
            break;
        case TokenCode::Error:
            outputFile << "Error";
            break;
        case TokenCode::Print:
            outputFile << "Print";
            break;
        case TokenCode::StringLiteral:
            outputFile << "StringLiteral";
            break;
        default:
            outputFile << "Unknown";
            break;
        }
        
            outputFile << " ,\"" << token.value<<"\" "<< std::endl;;
        }
        // 关闭输出文件
        outputFile.close();

        return 0;
    }
