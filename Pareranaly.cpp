#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
#include <fstream>
#include<string>
// 抽象语法树节点的基类
class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual void print(std::ostream& os) const = 0;
};

// 表达式节点
class ExprNode : public ASTNode {
public:
    virtual ~ExprNode() {}
};

// If-else语句节点
class IfElseExprNode : public ExprNode {
public:
    IfElseExprNode(std::unique_ptr<ExprNode> condition, std::unique_ptr<ExprNode> ifBranch, std::unique_ptr<ExprNode> elseBranch)
        : condition(std::move(condition)), ifBranch(std::move(ifBranch)), elseBranch(std::move(elseBranch)) {}

    void print(std::ostream& os) const override {
        os << "If-else" << std::endl;
        os << "Condition: ";
        condition->print(os);
        os << std::endl;
        os << "If branch: ";
        ifBranch->print(os);
        os << std::endl;
        os << "Else branch: ";
        elseBranch->print(os);
        os << std::endl;
    }

private:
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<ExprNode> ifBranch;
    std::unique_ptr<ExprNode> elseBranch;
};


// 赋值语句节点
class AssignmentStatementNode : public ExprNode {
public:
    AssignmentStatementNode(std::string identifier, std::unique_ptr<ExprNode> expression)
        : identifier(identifier), expression(std::move(expression)) {}

    void print(std::ostream& os) const override {
        os << identifier << " = ";
        expression->print(os);
    }

private:
    std::string identifier;
    std::unique_ptr<ExprNode> expression;
};

// 整数表达式节点
class IntExprNode : public ExprNode {
public:
    IntExprNode(int value) : value(value) {}

    void print(std::ostream& os) const override {
        os << value;
    }

private:
    int value;
};

// 二元操作符表达式节点
class BinaryOpExprNode : public ExprNode {
public:
    BinaryOpExprNode(char op, std::unique_ptr<ExprNode> left, std::unique_ptr<ExprNode> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}

    void print(std::ostream& os) const override {
       // os << "( ";
        left->print(os);
        os << " " << op << " ";
        right->print(os);
        //os << " )";
    }


private:
    char op;
    std::unique_ptr<ExprNode> left;
    std::unique_ptr<ExprNode> right;
};

// 词法标记类型枚举
enum class TokenType {
    Integer,
    Operator,
    Keyword,
    Identifier,
    Delimiter,
    Print,
    StringLiteral
};

// 词法标记结构体
struct Token {
    TokenType type;
    std::string value;
};

// 语法分析器类
class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), currentIndex(0) {}


    std::unique_ptr<ExprNode> parse() {
        return parseExpression();
    }



private:
    std::unique_ptr<ExprNode> parseIfStatement() {
        if (currentIndex < tokens.size() && tokens[currentIndex].type == TokenType::Keyword &&
            tokens[currentIndex].value == "if") {
            currentIndex++; // 移动到下一个标记
            //解析if分支
            auto ifBranch = parseExpression();
            if (!ifBranch) {
                std::cerr << "Syntax error: Missing if branch in if statement" << std::endl;
                return nullptr;
            }

            // 解析条件表达式
            auto condition = parseExpression();
            if (!condition) {
                std::cerr << "Syntax error: Invalid condition in if statement" << std::endl;
                return nullptr;
            }

           
            // 解析else分支
            std::unique_ptr<ExprNode> elseBranch = nullptr;
            if (currentIndex < tokens.size() && tokens[currentIndex].type == TokenType::Keyword &&
                tokens[currentIndex].value == "else") {
                currentIndex++; // 移动到下一个标记

                elseBranch = parseExpression();
                if (!elseBranch) {
                    std::cerr << "Syntax error: Missing else branch in if statement" << std::endl;
                    return nullptr;
                }
            }

            return std::make_unique<IfElseExprNode>(std::move(condition), std::move(ifBranch), std::move(elseBranch));
        }

        // 如果当前标记不是if关键字，则返回空指针
        return nullptr;
    }

    std::unique_ptr<ExprNode> parseExpression() {

        auto left = parseTerm();

        while (currentIndex < tokens.size() && tokens[currentIndex].type == TokenType::Operator) {
            std::string op = tokens[currentIndex].value;
            currentIndex++;
            auto right = parseTerm();
            left = std::make_unique<BinaryOpExprNode>(op[0], std::move(left), std::move(right));
            // 处理分号
            if (currentIndex < tokens.size() && tokens[currentIndex].type == TokenType::Delimiter) {
                currentIndex++;
                break;  // 遇到分号，结束表达式解析
            }
        }


        return left;
    }

    std::unique_ptr<ExprNode> parseTerm() {
        if (currentIndex < tokens.size() && tokens[currentIndex].type == TokenType::Integer) {
            const std::string& valueStr = tokens[currentIndex].value;


            std::string value;
            if (valueStr.size() >= 2 && valueStr.front() == '"' && valueStr.back() == '"') {
                value = valueStr.substr(1, valueStr.size() - 2);
            }
            else {
                value = valueStr;
            }

            int intValue;
            try {
                intValue = std::stoi(value);
            }
            catch (const std::exception& e) {
                std::cerr << "Syntax error: Failed to parse integer: " << e.what() << std::endl;
                return nullptr;
            }

            currentIndex++;
            return std::make_unique<IntExprNode>(intValue);
        }
        else if (currentIndex < tokens.size() && tokens[currentIndex].type == TokenType::Identifier) {
            std::string identifier = tokens[currentIndex].value;
            currentIndex++;

            if (currentIndex < tokens.size() && tokens[currentIndex].value == "=") {
                currentIndex++;
                auto expression = parseExpression();
                return std::make_unique<AssignmentStatementNode>(identifier, std::move(expression));
            }
            else {
                std::cerr << "Syntax error: Expected '=' after identifier" << std::endl;
                return nullptr;
            }
        }
        else {
            std::cerr << "Syntax error: Expected integer or identifier" << std::endl;
            return nullptr;
            return std::make_unique<IntExprNode>(0);

        }
    }

private:
    const std::vector<Token>& tokens;
    size_t currentIndex;
};

void writeToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    file << content;
    file.close();
}
std::vector<Token> readTokensFromFile(const std::string& filename) {
    std::vector<Token> tokens;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return tokens;
    }
    std::string line;
    int lineNumber = 1;  // 记录当前行号

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string typeStr, value;
        ss >> typeStr >> value;

        // 去除空格
        typeStr.erase(std::remove_if(typeStr.begin(), typeStr.end(), ::isspace), typeStr.end());

        // 解析 token 类型
        size_t start = typeStr.find("::") + 2;
        size_t end = typeStr.find_last_of('>');
        std::string tokenType = typeStr.substr(start, end - start);

        TokenType type;
        if (tokenType == "Integer") {
            type = TokenType::Integer;
        }
        else if (tokenType == "Operator") {
            type = TokenType::Operator;
        }
        else if (tokenType == "Keyword") {
            type = TokenType::Keyword;
        }
        else if (tokenType == "Identifier") {
            type = TokenType::Identifier;
        }
        else if (tokenType == "Delimiter") {
            type = TokenType::Delimiter;
        }
        else {
            std::cerr << "Invalid token type: " << tokenType << std::endl;
            continue;
        }

        // 解析 token 值
        size_t valueStart = value.find('"') + 1;
        size_t valueEnd = value.find_last_of('"');
        std::string tokenValue = value.substr(valueStart, valueEnd - valueStart);

        tokens.push_back({ type, tokenValue });
        if (type == TokenType::Delimiter) {
            lineNumber++;
        }
    }

    return tokens;
}

std::string astToString(const std::unique_ptr<ExprNode>& ast) {
    if (!ast) {
        return "";
    }

    std::stringstream ss;
    ast->print(ss);
    return ss.str();
}
void printAST(const std::unique_ptr<ExprNode>& ast) {
    if (ast) {
        ast->print(std::cout);
        std::cout  << std::endl;
        //std::cout << std::endl;
    }
}
void saveASTToFile(const std::string& filename, const std::unique_ptr<ExprNode>& ast) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    ast->print(file);

    file.close();
}
int main() {
    std::string tokensFile = "D:/tokens.txt";
    std::string outputFile = "D:/output_ABT.txt";

    // 读取 tokens
    std::vector<Token> tokens = readTokensFromFile(tokensFile);
    // 打印 tokens 的内容


    // 解析抽象语法树
    Parser parser(tokens);
    std::unique_ptr<ExprNode> ast = parser.parse();

    // 输出抽象语法树
    printAST(ast);
    // 保存抽象语法树到文件
    saveASTToFile(outputFile, ast);

    return 0;
}