#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <vector>
#include <cctype>

// 抽象语法树节点的基类
class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual std::string generateCode() const = 0;
};

// 整数表达式节点
class IntExprNode : public ASTNode {
public:
    IntExprNode(int value) : value(value) {}

    std::string generateCode() const override {
        return std::to_string(value);
    }

private:
    int value;
};

// 二元操作符表达式节点
class BinaryOpExprNode : public ASTNode {
public:
    BinaryOpExprNode(char op, std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}

    std::string generateCode() const override {
        std::string code = left->generateCode() + " " + right->generateCode() + " ";
        code.push_back(op);
        return code;
    }

private:
    char op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};

// 赋值语句节点
class AssignmentExprNode : public ASTNode {
public:
    AssignmentExprNode(std::string variable, std::unique_ptr<ASTNode> expression)
        : variable(variable), expression(std::move(expression)) {}

    std::string generateCode() const override {
        return expression->generateCode() + " " + variable + " =";
    }

private:
    std::string variable;
    std::unique_ptr<ASTNode> expression;
};

// 语义分析器类
class SemanticAnalyzer {
public:
    SemanticAnalyzer(std::unique_ptr<ASTNode> root) : root(std::move(root)) {}

    std::vector<std::string> generateCode() {
        std::vector<std::string> code;
        if (root) {
            code.push_back(root->generateCode());
        }
        return code;
    }

private:
    std::unique_ptr<ASTNode> root;
};

bool isOperator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

std::unique_ptr<ASTNode> parseExpression(std::stringstream& ss);

std::unique_ptr<ASTNode> parseTerm(std::stringstream& ss) {
    std::string token;
    ss >> token;

    if (isdigit(token[0])) {
        int value = std::stoi(token);
        return std::make_unique<IntExprNode>(value);
    }
    else if (token == "(") {
        auto expr = parseExpression(ss);
        if (!expr) {
            std::cerr << "Invalid expression" << std::endl;
            return nullptr;
        }

        std::string closingParenthesis;
        ss >> closingParenthesis;
        if (closingParenthesis != ")") {
            std::cerr << "Expected closing parenthesis" << std::endl;
            return nullptr;
        }

        return expr;
    }
    else {
        std::cerr << "Invalid token: " << token << std::endl;
        return nullptr;
    }
}

std::unique_ptr<ASTNode> parseFactor(std::stringstream& ss) {
    std::string token;
    ss >> token;

    if (isdigit(token[0])) {
        int value = std::stoi(token);
        return std::make_unique<IntExprNode>(value);
    }
    else if (token == "(") {
        auto expr = parseExpression(ss);
        if (!expr) {
            std::cerr << "Invalid expression" << std::endl;
            return nullptr;
        }

        std::string closingParenthesis;
        ss >> closingParenthesis;
        if (closingParenthesis != ")") {
            std::cerr << "Expected closing parenthesis" << std::endl;
            return nullptr;
        }

        return expr;
    }
    else if (isalpha(token[0])) {
        std::string nextToken;
        ss >> nextToken;
        if (nextToken == "=") {
            auto expr = parseExpression(ss);
            if (!expr) {
                std::cerr << "Invalid expression" << std::endl;
                return nullptr;
            }

            std::string semicolon;
            ss >> semicolon;

            return std::make_unique<AssignmentExprNode>(token, std::move(expr));
        }
        else {
            ss.putback(nextToken[0]);
            std::stringstream remainingInput;
            remainingInput << nextToken << " " << ss.rdbuf();
            return parseTerm(remainingInput);
        }
    }
    else {
        std::cerr << "Invalid token: " << token << std::endl;
        return nullptr;
    }
}

std::unique_ptr<ASTNode> parseExpression(std::stringstream& ss) {
    std::unique_ptr<ASTNode> left = parseTerm(ss);

    std::string token;
    while (ss >> token && isOperator(token[0])) {
        char op = token[0];

        std::unique_ptr<ASTNode> right = parseTerm(ss);
        if (!right) {
            std::cerr << "Invalid expression" << std::endl;
            return nullptr;
        }

        left = std::make_unique<BinaryOpExprNode>(op, std::move(left), std::move(right));
    }

    return left;
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeToFile(const std::string& filename, const std::vector<std::string>& code) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    for (const std::string& instruction : code) {
        file << instruction << " ";
    }
}

int main() {
    // 从文件中读取抽象语法树
    std::string inputFilename = "D:/output_ABT.txt";
    std::string treeString = readFile(inputFilename);

    if (treeString.empty()) {
        std::cerr << "Failed to read input file: " << inputFilename << std::endl;
        return 1;
    }

    // 解析抽象语法树字符串
    std::stringstream ss(treeString);
    std::unique_ptr<ASTNode> ast = parseExpression(ss);

    if (!ast) {
        std::cerr << "Failed to parse expression" << std::endl;
        return 1;
    }

    // 进行语义分析和生成中间代码
    SemanticAnalyzer analyzer(std::move(ast));
    std::vector<std::string> code = analyzer.generateCode();

    // 打印中间代码（逆波兰式）
    for (const std::string& instruction : code) {
        std::cout << instruction << " ";
    }
    std::cout << std::endl;

    // 将中间代码写入文件
    std::string outputFilename = "D:/output_TRP.txt";
    writeToFile(outputFilename, code);

    return 0;
}
