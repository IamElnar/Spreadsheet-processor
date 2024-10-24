

/**
 * @brief Class for representing a position in a table.
 */
class CPos {
public:
    /**
     * @brief Default constructor.
     */
    CPos();

    /**
     * @brief Constructor using a string representation.
     *
     * @param str String representing the position in the format "column_row".
     * @throw std::invalid_argument If the string has an invalid format.
     */
    CPos(std::string_view str);

    /**
     * @brief Overloaded addition operator.
     *
     * @param offset Offset as a pair (column, row).
     * @return New position considering the offset.
     */
    CPos operator+(std::pair<long long, long long> &offset) const;

    /**
     * @brief Get the string representation of the column.
     *
     * @return String representation of the column.
     */
    std::string getColumnStr() const;

    /**
     * @brief Get the column number.
     *
     * @return Column number.
     */
    long long getColumn() const;

    /**
     * @brief Get the row number.
     *
     * @return Row number.
     */
    long long getRow() const;

    /**
     * @brief Overloaded equality operator.
     *
     * @param other Another position to compare.
     * @return True if positions are equal, otherwise false.
     */
    bool operator==(const CPos& other) const;

private:
    /**
     * @brief Convert a string to a number.
     *
     * @param str String to convert.
     * @return Numeric representation of the string.
     */
    long long stringToNumber(const std::string& str) const;

    /**
     * @brief Convert a number to a string.
     *
     * @return String representation of the column.
     */
    std::string numberToString() const;

    std::string m_columnStr; ///< String representation of the column.
    long long m_column;      ///< Column number.
    long long m_row;         ///< Row number.
};

/**
 * @brief Struct for comparing positions.
 */
struct CPosComparator {
    /**
     * @brief Comparison operator for positions.
     *
     * @param lhs Left position.
     * @param rhs Right position.
     * @return True if the left position is less than the right one, otherwise false.
     */
    bool operator()(const CPos& lhs, const CPos& rhs) const;
};


CPos::CPos() : m_column(0), m_row(0) {}

CPos::CPos(std::string_view str) {
    size_t splitPos = 0;

    while(splitPos < str.length() && std::isalpha(str[splitPos])) {
        m_columnStr.push_back(std::toupper(str[splitPos++]));
    }

    if(splitPos == 0 || splitPos == str.length()) {
        throw std::invalid_argument("Invalid cell identifier.");
    }

    size_t check = 0;
    m_row = std::stoi(str.substr(splitPos).data(), &check);

    if(check != str.substr(splitPos).length()) {
        throw std::invalid_argument("Invalid cell identifier.");
    }

    m_column = stringToNumber(m_columnStr);
}

CPos CPos::operator+(std::pair<long long, long long> &offset) const {
    CPos off;
    off.m_row = m_row + offset.second;
    off.m_column = m_column + offset.first;
    off.m_columnStr = numberToString();
    return off;
}

std::string CPos::getColumnStr() const {
    return numberToString();
}

long long CPos::getColumn() const {
    return m_column;
}

long long CPos::getRow() const {
    return m_row;
}

bool CPos::operator==(const CPos& other) const {
    return m_column == other.m_column && m_row == other.m_row;
}

long long CPos::stringToNumber(const std::string& str) const {
    long long result = 0;
    for (char ch : str) {
        if (ch >= 'A' && ch <= 'Z') {
            result *= 26;
            result += ch - 'A' + 1;
        }
    }
    return result;
}

std::string CPos::numberToString() const {
    auto num = m_column;
    std::string result;
    while (num > 0) {
        auto remainder = (num - 1) % 26;
        char ch = 'A' + remainder;
        result.insert(0, 1, ch);
        num = (num - 1) / 26;
    }
    return result;
}

bool CPosComparator::operator()(const CPos& lhs, const CPos& rhs) const {
    if (lhs.getColumn() != rhs.getColumn()) {
        return lhs.getColumn() < rhs.getColumn();
    }
    return lhs.getRow() < rhs.getRow();
}


/**
 * @brief Base class representing a node in the abstract syntax tree (AST).
 */
struct ASTNode {
    /**
     * @brief Constructor for ASTNode.
     *
     * @param l Pointer to the left child node.
     * @param r Pointer to the right child node.
     * @param isExp Flag indicating whether the node represents an expression.
     */
    ASTNode(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r, bool isExp = false) : m_left(l), m_right(r), isExpression(isExp){}

    /**
     * @brief Virtual destructor for ASTNode.
     */
    virtual ~ASTNode() = default;

    /**
     * @brief Evaluate the node and return the result.
     *
     * @return The result of the evaluation.
     */
    virtual CValue evaluate() = 0;

    /**
     * @brief Create a deep copy of the node and its children.
     *
     * @param map Map of positions to nodes for keeping track of copied nodes.
     * @return A pointer to the deep copy of the node.
     */
    virtual std::shared_ptr<ASTNode> deepCopy(std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator>& map) const = 0;

    /**
     * @brief Move the node and its children relative to their positions.
     *
     * @param offset Pair of offsets to move the node.
     */
    virtual void moveRelativelyBy(const std::pair<long long int, long long int>& offset) = 0;

    /**
     * @brief Print the node.
     *
     * @param os Output stream to print the node.
     */
    virtual void print(std::ostream& os) const = 0;

    /**
     * @brief Check if the node or any of its children forms a cycle.
     *
     * @param visited Map of positions to booleans to keep track of visited nodes.
     * @return True if a cycle is detected, otherwise false.
     */
    virtual bool hasCycle(std::map<CPos, bool, CPosComparator>& visited) const = 0;

    /**
     * @brief Overloaded output stream operator to print the node.
     *
     * @param os Output stream.
     * @param node Node to be printed.
     * @return Reference to the output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const ASTNode& node)
    {
        node.print(os);
        return os;
    }

    std::shared_ptr<ASTNode> m_left; ///< Pointer to the left child node.
    std::shared_ptr<ASTNode> m_right; ///< Pointer to the right child node.
    bool isExpression; ///< Flag indicating whether the node represents an expression.
};



/**
 * @brief Class representing a binary arithmetic operator node in the abstract syntax tree (AST).
 *
 * This class represents a binary arithmetic operator node in the AST.
 * It inherits from the ASTNode class and provides common functionalities for binary arithmetic operators.
 */
struct ASTNodeBinaryOperator : public ASTNode {
    /**
     * @brief Constructor for ASTNodeBinaryOperator.
     *
     * @param op The operator symbol.
     * @param l Pointer to the left child node.
     * @param r Pointer to the right child node.
     */
    ASTNodeBinaryOperator(const std::string &op, std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    /**
     * @brief Move the binary operator node and its children relative to their positions.
     *
     * @param offset Pair of offsets to move the node.
     */
    void moveRelativelyBy(const std::pair<long long int, long long int> &offset) override;

    /**
     * @brief Print the binary operator node.
     *
     * @param os Output stream to print the node.
     */
    void print(std::ostream &os) const override;

    /**
     * @brief Check if the binary operator node or any of its children forms a cycle.
     *
     * @param visited Map of positions to booleans to keep track of visited nodes.
     * @return True if a cycle is detected, otherwise false.
     */
    bool hasCycle(std::map<CPos, bool, CPosComparator> &visited) const override;

    std::string m_op; /**< The operator symbol. */
};


ASTNodeBinaryOperator::ASTNodeBinaryOperator(const std::string &op, std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNode(l, r), m_op(op){}


    void ASTNodeBinaryOperator:: moveRelativelyBy(const std::pair<long long int, long long int> &offset)
    {
        m_left->moveRelativelyBy(offset);
        m_right->moveRelativelyBy(offset);
    }

    void ASTNodeBinaryOperator::print(std::ostream &os) const
    {
        os <<"(";
        m_left->print(os);
        os << m_op;
        m_right->print(os);
        os <<")";
    }
    bool ASTNodeBinaryOperator::hasCycle(std::map<CPos, bool, CPosComparator> &visited) const
    {
        return (m_left->hasCycle(visited) || m_right->hasCycle(visited));
    }



struct ASTNodeRelationalOperator : public ASTNode {

    ASTNodeRelationalOperator(const std::string &op, std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    void moveRelativelyBy(const std::pair<long long int, long long int> &offset) override;

    void print(std::ostream &os) const override;

    bool hasCycle(std::map<CPos, bool, CPosComparator> &visited) const override;

    std::string m_op;
};


ASTNodeRelationalOperator::ASTNodeRelationalOperator(const std::string &op, std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNode(l, r), m_op(op){}


void ASTNodeRelationalOperator:: moveRelativelyBy(const std::pair<long long int, long long int> &offset)
{
    m_left->moveRelativelyBy(offset);
    m_right->moveRelativelyBy(offset);
}

void ASTNodeRelationalOperator::print(std::ostream &os) const
{
    os <<"(";
    m_left->print(os);
    os << m_op;
    m_right->print(os);
    os <<")";
}
bool ASTNodeRelationalOperator::hasCycle(std::map<CPos, bool, CPosComparator> &visited) const
{
    return (m_left->hasCycle(visited) || m_right->hasCycle(visited));
}


struct ASTNodeUnaryOperator : public ASTNode
{
    ASTNodeUnaryOperator(const std::string & op, std::shared_ptr<ASTNode> l);

    void moveRelativelyBy(const std::pair<long long int, long long int> &offset) override;

    void print(std::ostream &os) const override;

    bool hasCycle(std::map<CPos, bool, CPosComparator> &visited) const override;

    std::string m_op;
};


ASTNodeUnaryOperator::ASTNodeUnaryOperator(const std::string & op, std::shared_ptr<ASTNode> l) : ASTNode(l, nullptr), m_op(op){}

    void ASTNodeUnaryOperator::moveRelativelyBy(const std::pair<long long int, long long int> &offset)
    {
        m_left->moveRelativelyBy(offset);
    }

    void ASTNodeUnaryOperator::print(std::ostream &os) const
    {
        os <<"(-";
        m_left->print(os);
        os <<")";
    }

    bool ASTNodeUnaryOperator :: hasCycle(std::map<CPos, bool, CPosComparator> &visited) const
    {
        return (m_left->hasCycle(visited));
    }

struct ASTNodeAdd : public ASTNodeBinaryOperator
{

    ASTNodeAdd(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

};

struct ASTNodeSub : public ASTNodeBinaryOperator
{

    ASTNodeSub(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

};

struct ASTNodeMul : public ASTNodeBinaryOperator
{

    ASTNodeMul(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

};

struct ASTNodeDiv : public ASTNodeBinaryOperator
{

    ASTNodeDiv(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

};

struct ASTNodePow : public ASTNodeBinaryOperator
{

    ASTNodePow(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

};



struct ASTNodeEq : public ASTNodeRelationalOperator
{

    ASTNodeEq(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

};

struct ASTNodeNe : public ASTNodeRelationalOperator
{

    ASTNodeNe(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

};

struct ASTNodeLt : public ASTNodeRelationalOperator
{

    ASTNodeLt(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

};

struct ASTNodeLe : public ASTNodeRelationalOperator
{

    ASTNodeLe(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

};

struct ASTNodeGt : public ASTNodeRelationalOperator
{

    ASTNodeGt(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

};

struct ASTNodeGe : public ASTNodeRelationalOperator
{

    ASTNodeGe(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

};



struct ASTNodeNeg : public ASTNodeUnaryOperator
{
    ASTNodeNeg(std::shared_ptr<ASTNode> l);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

};



struct ASTStringLiteral: public ASTNode
{
    ASTStringLiteral(const std::string &literal, bool isExp);

    CValue evaluate () override;
    std::shared_ptr<ASTNode> deepCopy (std::map<CPos,std::shared_ptr<ASTNode>, CPosComparator> &map) const override;
    void moveRelativelyBy(const std::pair<long long int, long long int> &offset) override;

    void print(std::ostream &os) const override;
    bool hasCycle(std::map<CPos, bool, CPosComparator> &visited) const override;
private:
    std::string m_literal;
};




struct ASTNodeDouble: public ASTNode
{

    ASTNodeDouble(double value);

    CValue evaluate() override;

    std::shared_ptr<ASTNode> deepCopy (std::map<CPos,std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

    void moveRelativelyBy(const std::pair<long long int, long long int> &offset) override;

    void print(std::ostream &os) const override;

    bool hasCycle(std::map<CPos, bool, CPosComparator> &visited) const override;

private:
    double m_value;
};





struct ASTNodeRoot : public  ASTNode
{
    ASTNodeRoot(bool isExp = false);
    CValue evaluate() override;
    std::shared_ptr<ASTNode> deepCopy(std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;
    void moveRelativelyBy(const std::pair<long long int, long long int> &offset) override;

    void print(std::ostream &os) const override;

    bool hasCycle(std::map<CPos, bool, CPosComparator> &visited) const override;
};




struct ASTNodeReference: public ASTNode {
    ASTNodeReference(std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> *table, CPos cell, bool relCol, bool relRow);

    CValue evaluate() override;
    std::shared_ptr<ASTNode> deepCopy(std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const override;

    void moveRelativelyBy(const std::pair<long long int, long long int> &offset) override;

    void print(std::ostream &os) const override;

    bool hasCycle(std::map<CPos, bool, CPosComparator> &visited) const override;

    std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> *m_table;
    CPos m_pos;
    bool isColumnRelative, isRowRelative;
};



ASTNodeAdd::ASTNodeAdd(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNodeBinaryOperator("+", l, r){}

CValue ASTNodeAdd::evaluate()
{
    auto left = m_left->evaluate();
    auto right = m_right->evaluate();

    if(std::holds_alternative<double>(left) && std::holds_alternative<std::string>(right))
        return CValue {std::to_string(std::get<double>(left)) + std::get<std::string>(right)};

    if(std::holds_alternative<std::string>(left) && std::holds_alternative<double>(right))
        return CValue {std::get<std::string>(left) + std::to_string(std::get<double>(right))};

    if(std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
        return CValue {std::get<std::string>(left) + std::get<std::string>(right)};

    if(std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
        return  CValue{std::get<double>(left) + std::get<double>(right)};

    return CValue {};
}

std::shared_ptr<ASTNode> ASTNodeAdd::deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    std::shared_ptr<ASTNodeAdd> copy = std::make_shared<ASTNodeAdd>(nullptr, nullptr);
    copy->m_left = m_left->deepCopy(map);
    copy->m_right = m_right->deepCopy(map);
    return copy;
}

ASTNodeSub::ASTNodeSub(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNodeBinaryOperator("-", l, r){}

CValue ASTNodeSub::evaluate()
{
    auto left = m_left->evaluate();
    auto right = m_right->evaluate();

    if(!std::holds_alternative<double>(left) || !std::holds_alternative<double>(right))
        return CValue {};
    return  CValue{std::get<double>(left) - std::get<double>(right)};
}

std::shared_ptr<ASTNode> ASTNodeSub::deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    std::shared_ptr<ASTNodeSub> copy = std::make_shared<ASTNodeSub>(nullptr, nullptr);
    copy->m_left = m_left->deepCopy(map);
    copy->m_right = m_right->deepCopy(map);
    return copy;
}

ASTNodeMul::ASTNodeMul(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNodeBinaryOperator("*", l, r){}

CValue ASTNodeMul::evaluate()
{
    auto left = m_left->evaluate();
    auto right = m_right->evaluate();

    if(!std::holds_alternative<double>(left) || !std::holds_alternative<double>(right))
        return CValue {};
    return  CValue{std::get<double>(left) * std::get<double>(right)};
}

std::shared_ptr<ASTNode> ASTNodeMul::deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    std::shared_ptr<ASTNodeMul> copy = std::make_shared<ASTNodeMul>(nullptr, nullptr);
    copy->m_left = m_left->deepCopy(map);
    copy->m_right = m_right->deepCopy(map);
    return copy;
}

ASTNodeDiv::ASTNodeDiv(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNodeBinaryOperator("/", l, r){}

CValue ASTNodeDiv::evaluate()
{
    auto left = m_left->evaluate();
    auto right = m_right->evaluate();

    if(!std::holds_alternative<double>(left) || !std::holds_alternative<double>(right))
        return CValue {};
    if(std::get<double>(right) == 0.0)
        return CValue {};
    return  CValue{std::get<double>(left) / std::get<double>(right)};
}

std::shared_ptr<ASTNode> ASTNodeDiv::deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    std::shared_ptr<ASTNodeDiv> copy = std::make_shared<ASTNodeDiv>(nullptr, nullptr);
    copy->m_left = m_left->deepCopy(map);
    copy->m_right = m_right->deepCopy(map);
    return copy;
}

ASTNodePow::ASTNodePow(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNodeBinaryOperator("^", l, r){}

CValue ASTNodePow::evaluate()
{
    auto left = m_left->evaluate();
    auto right = m_right->evaluate();

    if(!std::holds_alternative<double>(left) || !std::holds_alternative<double>(right))
        return CValue {};
    return  CValue{std::pow(std::get<double>(left), std::get<double>(right))};
}

std::shared_ptr<ASTNode> ASTNodePow::deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    std::shared_ptr<ASTNodePow> copy = std::make_shared<ASTNodePow>(nullptr, nullptr);
    copy->m_left = m_left->deepCopy(map);
    copy->m_right = m_right->deepCopy(map);
    return copy;
}

ASTNodeEq::ASTNodeEq(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNodeRelationalOperator("=", l, r){}

CValue ASTNodeEq::evaluate()
{
    auto left = m_left->evaluate();
    auto right = m_right->evaluate();

    if(std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
    {
        return std::get<double>(left) == std::get<double>(right) ? CValue{1.0} : CValue {0.0};
    }
    else if(std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
    {
        return std::get<std::string>(left) == std::get<std::string>(right) ? CValue{1.0} : CValue {0.0};
    }
    else
        return CValue {};
}

std::shared_ptr<ASTNode> ASTNodeEq::deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    std::shared_ptr<ASTNodeEq> copy = std::make_shared<ASTNodeEq>(nullptr, nullptr);
    copy->m_left = m_left->deepCopy(map);
    copy->m_right = m_right->deepCopy(map);
    return copy;
}

ASTNodeNe::ASTNodeNe(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNodeRelationalOperator("<>", l, r){}

CValue ASTNodeNe::evaluate()
{
    auto left = m_left->evaluate();
    auto right = m_right->evaluate();

    if(std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
    {
        return std::get<double>(left) != std::get<double>(right) ? CValue{1.0} : CValue {0.0};
    }
    else if(std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
    {
        return std::get<std::string>(left) != std::get<std::string>(right) ? CValue{1.0} : CValue {0.0};
    }
    else
        return CValue {};
}

std::shared_ptr<ASTNode> ASTNodeNe::deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    std::shared_ptr<ASTNodeNe> copy = std::make_shared<ASTNodeNe>(nullptr, nullptr);
    copy->m_left = m_left->deepCopy(map);
    copy->m_right = m_right->deepCopy(map);
    return copy;
}

ASTNodeLt::ASTNodeLt(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNodeRelationalOperator("<", l, r){}

CValue ASTNodeLt::evaluate()
{
    auto left = m_left->evaluate();
    auto right = m_right->evaluate();

    if(std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
    {
        return std::get<double>(left) < std::get<double>(right) ? CValue{1.0} : CValue {0.0};
    }
    else if(std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
    {
        return std::get<std::string>(left) < std::get<std::string>(right) ? CValue{1.0} : CValue {0.0};
    }
    else
        return CValue {};
}

std::shared_ptr<ASTNode> ASTNodeLt::deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    std::shared_ptr<ASTNodeLt> copy = std::make_shared<ASTNodeLt>(nullptr, nullptr);
    copy->m_left = m_left->deepCopy(map);
    copy->m_right = m_right->deepCopy(map);
    return copy;
}

ASTNodeLe::ASTNodeLe(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNodeRelationalOperator("<=", l, r){}

CValue ASTNodeLe::evaluate()
{
    auto left = m_left->evaluate();
    auto right = m_right->evaluate();

    if(std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
    {
        return std::get<double>(left) <= std::get<double>(right) ? CValue{1.0} : CValue {0.0};
    }
    else if(std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
    {
        return std::get<std::string>(left) <= std::get<std::string>(right) ? CValue{1.0} : CValue {0.0};
    }
    else
        return CValue {};
}

std::shared_ptr<ASTNode> ASTNodeLe::deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    std::shared_ptr<ASTNodeLe> copy = std::make_shared<ASTNodeLe>(nullptr, nullptr);
    copy->m_left = m_left->deepCopy(map);
    copy->m_right = m_right->deepCopy(map);
    return copy;
}

ASTNodeGt::ASTNodeGt(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNodeRelationalOperator(">", l, r){}

CValue ASTNodeGt::evaluate()
{
    auto left = m_left->evaluate();
    auto right = m_right->evaluate();

    if(std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
    {
        return std::get<double>(left) > std::get<double>(right) ? CValue{1.0} : CValue {0.0};
    }
    else if(std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
    {
        return std::get<std::string>(left) > std::get<std::string>(right) ? CValue{1.0} : CValue {0.0};
    }
    else
        return CValue {};
}

std::shared_ptr<ASTNode> ASTNodeGt::deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    std::shared_ptr<ASTNodeGt> copy = std::make_shared<ASTNodeGt>(nullptr, nullptr);
    copy->m_left = m_left->deepCopy(map);
    copy->m_right = m_right->deepCopy(map);
    return copy;
}

ASTNodeGe::ASTNodeGe(std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) : ASTNodeRelationalOperator(">=", l, r){}

CValue ASTNodeGe::evaluate()
{
    auto left = m_left->evaluate();
    auto right = m_right->evaluate();

    if(std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
    {
        return std::get<double>(left) >= std::get<double>(right) ? CValue{1.0} : CValue {0.0};
    }
    else if(std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
    {
        return std::get<std::string>(left) >= std::get<std::string>(right) ? CValue{1.0} : CValue {0.0};
    }
    else
        return CValue {};
}

std::shared_ptr<ASTNode> ASTNodeGe::deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    std::shared_ptr<ASTNodeGe> copy = std::make_shared<ASTNodeGe>(nullptr, nullptr);
    copy->m_left = m_left->deepCopy(map);
    copy->m_right = m_right->deepCopy(map);
    return copy;
}

ASTNodeNeg::ASTNodeNeg(std::shared_ptr<ASTNode> l) : ASTNodeUnaryOperator("-", l) {}

CValue ASTNodeNeg::evaluate() {
    auto left = m_left->evaluate();
    if(!std::holds_alternative<double>(left))
        return CValue{};
    return  CValue{ -1 * std::get<double>(left)};
}

std::shared_ptr<ASTNode> ASTNodeNeg::deepCopy (std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    std::shared_ptr<ASTNodeNeg> copy = std::make_shared<ASTNodeNeg>(nullptr);
    copy->m_left = m_left->deepCopy(map);
    return copy;
}

ASTStringLiteral::ASTStringLiteral(const std::string &literal, bool isExp) : ASTNode(nullptr, nullptr, isExp), m_literal(literal){}

    CValue ASTStringLiteral::evaluate ()
    {
        return CValue(m_literal);
    }

    std::shared_ptr<ASTNode> ASTStringLiteral::deepCopy (std::map<CPos,std::shared_ptr<ASTNode>, CPosComparator> &map) const
    {
        auto copy = std::make_shared<ASTStringLiteral>(m_literal, isExpression);
        return copy;
    }

    void ASTStringLiteral::moveRelativelyBy(const std::pair<long long int, long long int> &offset) {}

    void ASTStringLiteral::print(std::ostream &os) const
    {
        if(isExpression)
        {
            std::string result;
            result.push_back('\"');
            for(const auto &ch : m_literal)
            {
                if(ch == '\"')
                    result.push_back(ch);
                result.push_back(ch);
            }
            result.push_back('\"');
            os << result;
        }
        else
            os << m_literal;
    }
    bool ASTStringLiteral::hasCycle(std::map<CPos, bool, CPosComparator> &visited) const
    {
        return false;
    }

ASTNodeDouble::ASTNodeDouble(double value) : ASTNode(nullptr, nullptr), m_value(value){}

CValue ASTNodeDouble::evaluate()
{
    return m_value;
}
std::shared_ptr<ASTNode> ASTNodeDouble::deepCopy (std::map<CPos,std::shared_ptr<ASTNode>, CPosComparator> &map) const
{
    auto copy = std::make_shared<ASTNodeDouble>(m_value);
    return copy;
}
void ASTNodeDouble::moveRelativelyBy(const std::pair<long long int, long long int> &offset){}

void ASTNodeDouble::print(std::ostream &os) const
{
    os << std::to_string(m_value);
}
bool ASTNodeDouble::hasCycle(std::map<CPos, bool, CPosComparator> &visited) const
{
    return false;
}


ASTNodeRoot::ASTNodeRoot(bool isExp) : ASTNode(nullptr, nullptr, isExp) {}
    CValue ASTNodeRoot::evaluate()
    {
        if(m_left)
            return m_left->evaluate();

        return CValue {};
    }
    std::shared_ptr<ASTNode> ASTNodeRoot::deepCopy(std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
    {
        auto copy = std::make_shared<ASTNodeRoot>(isExpression);
        if(m_left)
            copy->m_left = m_left->deepCopy(map);
        return copy;
    }
    void ASTNodeRoot::moveRelativelyBy(const std::pair<long long int, long long int> &offset)
    {
        if(m_left)
            m_left->moveRelativelyBy(offset);
    }

    void ASTNodeRoot::print(std::ostream &os) const
    {
        if(isExpression)
            os << "=";
        if(m_left)
            m_left->print(os);
    }

    bool ASTNodeRoot::hasCycle(std::map<CPos, bool, CPosComparator> &visited) const
    {
        bool detected = false;
        if(m_left)
            detected = m_left->hasCycle(visited);
        return detected;
    }

ASTNodeReference::ASTNodeReference(std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> *table, CPos cell, bool relCol, bool relRow) :
            ASTNode(nullptr, nullptr),
            m_table(table),
            m_pos(cell),
            isColumnRelative(relCol),
            isRowRelative(relRow) {}


    CValue ASTNodeReference::evaluate()
    {
        if(auto cell = m_table->find(m_pos); cell != m_table->end())
        {
            if(cell->second != nullptr)
            {
                return cell->second->evaluate();
            }
        }
        return CValue {};
    }

    std::shared_ptr<ASTNode> ASTNodeReference::deepCopy(std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> &map) const
    {
        if(map[m_pos] == nullptr)
        {
            map[m_pos] = std::make_shared<ASTNodeRoot>();
        }
        auto copy = std::make_shared<ASTNodeReference>(&map, m_pos, isColumnRelative, isRowRelative);
        return copy;
    }
    void ASTNodeReference::moveRelativelyBy(const std::pair<long long int, long long int> &offset)
    {
        auto offs = offset;
        if(isColumnRelative)
            offs.first = 0;
        if(isRowRelative)
            offs.second = 0;
        m_pos = m_pos + offs;
    }

    void ASTNodeReference::print(std::ostream &os) const
    {
        if(isColumnRelative)
            os <<"$";
        os << m_pos.getColumnStr();
        if(isRowRelative)
            os << "$";
        os << m_pos.getRow();
    }
    bool ASTNodeReference::hasCycle(std::map<CPos, bool, CPosComparator> &visited) const
    {
        if(visited.find(m_pos) != visited.end() && visited[m_pos])
            return true;
        visited[m_pos] = true;
        bool detected = false;
        if(auto cell = m_table->find(m_pos); cell != m_table->end())
        {
            if(cell->second != nullptr)
            {
                detected = cell->second->hasCycle(visited);
            }
        }
        if(!detected)
        {
            visited[m_pos] = false;
        }

        return detected;
    }


struct CCells
{
    std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> m_cells;
};


class CBuilder : public CExprBuilder
{
public:
    CBuilder(CCells *cells, bool isExp = false);
    void opAdd () override;
    void opSub () override;
    void opMul () override;
    void opDiv () override;
    void opPow () override;
    void opNeg () override;
    void opEq () override;
    void opNe () override;
    void opLt () override;
    void opLe () override;
    void opGt () override;
    void opGe () override;
    void valNumber (double val) override;
    void valString (std::string val) override;
    void valReference (std::string val) override;
    void valRange (std::string val) override;
    void funcCall (std::string fnName, int paramCount) override;
    void getRoot (std::shared_ptr<ASTNode> &root, bool isExp) const;
private:
    CCells *m_table;
    std::stack<std::shared_ptr<ASTNode>> m_stack;
    bool isExpression;
};



CBuilder::CBuilder(CCells *cells, bool isExp) : m_table(cells), isExpression(isExp) {}
void CBuilder::opAdd ()
    {
        auto r  = m_stack.top();
        m_stack.pop();
        auto l = m_stack.top();
        m_stack.pop();
        m_stack.push(std::make_shared<ASTNodeAdd>(l, r));
    }
    void CBuilder::opSub ()
    {
        auto r  = m_stack.top();
        m_stack.pop();
        auto l = m_stack.top();
        m_stack.pop();
        m_stack.push(std::make_shared<ASTNodeSub>(l, r));
    }
    void CBuilder::opMul ()
    {
        auto l  = m_stack.top();
        m_stack.pop();
        auto r = m_stack.top();
        m_stack.pop();
        m_stack.push(std::make_shared<ASTNodeMul>(l, r));
    }
    void CBuilder::opDiv ()
    {   //check zero divide
        auto r  = m_stack.top();
        m_stack.pop();
        auto l = m_stack.top();
        m_stack.pop();
        m_stack.push(std::make_shared<ASTNodeDiv>(l, r));
    }
    void CBuilder::opPow ()
    {
        auto r  = m_stack.top();
        m_stack.pop();
        auto l = m_stack.top();
        m_stack.pop();
        m_stack.push(std::make_shared<ASTNodePow>(l, r));
    }
    void CBuilder::opNeg ()
    {
        auto l  = m_stack.top();
        m_stack.pop();
        m_stack.push(std::make_shared<ASTNodeNeg>(l));
    }
    void CBuilder::opEq ()
    {
        auto r  = m_stack.top();
        m_stack.pop();
        auto l = m_stack.top();
        m_stack.pop();
        m_stack.push(std::make_shared<ASTNodeEq>(l, r));
    }
    void CBuilder::opNe ()
    {
        auto r  = m_stack.top();
        m_stack.pop();
        auto l = m_stack.top();
        m_stack.pop();
        m_stack.push(std::make_shared<ASTNodeNe>(l, r));
    }
    void CBuilder::opLt ()
    {
        auto r  = m_stack.top();
        m_stack.pop();
        auto l = m_stack.top();
        m_stack.pop();
        m_stack.push(std::make_shared<ASTNodeLt>(l, r));
    }
    void CBuilder::opLe ()
    {
        auto r  = m_stack.top();
        m_stack.pop();
        auto l = m_stack.top();
        m_stack.pop();
        m_stack.push(std::make_shared<ASTNodeLe>(l, r));
    }
    void CBuilder::opGt ()
    {
        auto r  = m_stack.top();
        m_stack.pop();
        auto l = m_stack.top();
        m_stack.pop();
        m_stack.push(std::make_shared<ASTNodeGt>(l, r));
    }
    void CBuilder::opGe ()
    {
        auto r  = m_stack.top();
        m_stack.pop();
        auto l = m_stack.top();
        m_stack.pop();
        m_stack.push(std::make_shared<ASTNodeGe>(l, r));
    }
    void CBuilder::valNumber (double val)
    {
        m_stack.push(std::make_shared<ASTNodeDouble>(val));
    }
    void CBuilder::valString (std::string val)
    {
        m_stack.push(std::make_shared<ASTStringLiteral>(val, isExpression));
    }
    void CBuilder::valReference (std::string val)
    {
        size_t pos = 0;
        bool relRow = false, relCol = false;
        std::string column, row;

        if(val.size() > 0 && val[0] == '$')
        {
            pos++;
            relCol = true;
        }
        while(std::isalpha(val[pos]))
        {
            column.push_back(val[pos++]);
        }
        if(pos < val.size() && val[pos] == '$')
        {
            relRow = true;
            pos++;
        }
        row = val.substr(pos);
        std::string cell(column + row);
        if(m_table->m_cells[CPos(cell)] == nullptr)
        {
            m_table->m_cells[CPos(cell)] = std::make_shared<ASTNodeRoot>();
        }
        auto ref = std::make_shared<ASTNodeReference>(&m_table->m_cells, CPos(cell), relCol, relRow);
        m_stack.push(ref);
    }
    void CBuilder::valRange (std::string val){}
    void CBuilder::funcCall (std::string fnName, int paramCount){}

    void CBuilder::getRoot (std::shared_ptr<ASTNode> &root, bool isExp) const
    {
        if(root == nullptr)
        {
            root = std::make_shared<ASTNodeRoot>(isExp);
        }
        root->m_left = m_stack.top();
    }


class CSpreadsheet
{
public:
    static unsigned capabilities ()
    {
        return SPREADSHEET_CYCLIC_DEPS;
    }

    CSpreadsheet () = default;
    ~CSpreadsheet() = default;
    CSpreadsheet (const CSpreadsheet &other);

    CSpreadsheet & operator = (const CSpreadsheet &other);

    bool load (std::istream &is);

    bool save (std::ostream &os) const;

    bool setCell (CPos pos, std::string contents);

    CValue getValue (CPos pos);

    void copyRect (CPos dst, CPos src, int w = 1, int h = 1);

    void print() const;
private:
    CCells m_table;
};



    CSpreadsheet::CSpreadsheet (const CSpreadsheet &other)
    {
        for(const auto &cell : other.m_table.m_cells)
        {
            if(m_table.m_cells[cell.first] != nullptr)
            {
                if(cell.second->m_left)
                    m_table.m_cells[cell.first]->m_left = cell.second->m_left->deepCopy(m_table.m_cells);
            }
            else
            {
                m_table.m_cells[cell.first] = cell.second->deepCopy(m_table.m_cells);
            }
        }
    }
    CSpreadsheet& CSpreadsheet::operator = (const CSpreadsheet &other)
    {
        if(this != &other)
        {
            m_table.m_cells.clear();
            for(const auto &cell : other.m_table.m_cells)
            {
                if(m_table.m_cells[cell.first] != nullptr)
                {
                    if(cell.second->m_left)
                        m_table.m_cells[cell.first]->m_left = cell.second->m_left->deepCopy(m_table.m_cells);
                }
                else
                {
                    m_table.m_cells[cell.first] = cell.second->deepCopy(m_table.m_cells);
                }
            }
        }
        return  *this;
    }
    bool CSpreadsheet::load (std::istream &is)
    {
        if (!is)
        {
            return false;
        }

        char ch;
        std::string pos, expression;

        if (!(is >> ch) || ch != '{')
        {
            return false;
        }

        if (!(is >> ch) || ch != static_cast<char>(31))
        {
            return false;
        }
        while (true)
        {
            while(is.get(ch) && ch != static_cast<char>(30))
                pos.push_back(ch);
            if(!is)
                return false;
            if(!(is >> ch) && ch != ':')
                return false;
            if(!(is >> ch) && ch != static_cast<char>(30))
                return false;
            while(is.get(ch) && ch != static_cast<char>(31))
                expression.push_back(ch);
            if(!is)
                return false;
            if(!setCell(CPos(pos), expression))
                return false;
            pos.clear();
            expression.clear();
            if(is.peek() == '}')
                break;
        }
        return true;
    }
    bool CSpreadsheet::save (std::ostream &os) const
    {
        if (!os)
        {
            return false;
        }
        os << '{' << static_cast<char>(31);

        for (const auto& cell : m_table.m_cells)
        {
            if (!os)
            {
                return false;
            }
            os << cell.first.getColumnStr() + std::to_string(cell.first.getRow()) << static_cast<char>(30) << ':' << static_cast<char>(30) << *cell.second << static_cast<char>(31);
        }

        if(!(os << '}'))
            return false;
        return true;
    }
    bool CSpreadsheet::setCell (CPos pos, std::string contents)
    {
        try
        {
            bool isExp = false;
            if(contents[0] == '=')
                isExp = true;
            CBuilder builder(&m_table, isExp);
            parseExpression(contents, builder);
            builder.getRoot(m_table.m_cells[pos], isExp);
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << e.what();
            return false;
        }
        return true;
    }
    CValue CSpreadsheet::getValue (CPos pos)
    {
        auto cell = m_table.m_cells.find(pos);
        if(cell == m_table.m_cells.end())
        {
            return CValue{};
        }
        std::map<CPos, bool , CPosComparator> visited;
        visited[pos] = true;
        if(cell->second->hasCycle(visited))
            return CValue {};
        auto test = cell->second->evaluate();

        return cell->second->evaluate();
    }
    void CSpreadsheet::copyRect (CPos dst, CPos src, int w, int h)
    {
        std::map<CPos, std::shared_ptr<ASTNode>, CPosComparator> temp;

        for(auto y = 0; y < h; y++)
        {
            for(auto x = 0; x < w; x++)
            {
                std::pair<long long, long long> offset = std::make_pair(x,y);
                CPos toCopy = src + offset;
                if(auto found = m_table.m_cells.find(toCopy); found == m_table.m_cells.end())
                    continue;
                temp[toCopy] = m_table.m_cells[toCopy]->m_left;
                temp[toCopy]->isExpression = m_table.m_cells[toCopy]->isExpression;
            }
        }
        for(int y = 0; y < h; y++)
        {
            for(int x = 0; x < w; x++)
            {
                std::pair<long long, long long> offset = std::make_pair(x,y);
                auto from = src + offset;
                auto to = dst + offset;

                if(auto found = temp.find(from); found == temp.end())
                {
                    if(auto ptr = m_table.m_cells.find(to); ptr != m_table.m_cells.end())
                        m_table.m_cells.erase(ptr);
                    continue;
                }
                if(m_table.m_cells[to] == nullptr)
                    m_table.m_cells[to] = std::make_shared<ASTNodeRoot>();

                m_table.m_cells[to]->m_left = temp[from]->deepCopy(m_table.m_cells);
                m_table.m_cells[to]->isExpression = temp[from]->isExpression;

                auto relativeOffset = std::make_pair(to.getColumn() - from.getColumn(), to.getRow() - from.getRow());
                m_table.m_cells[to]->moveRelativelyBy(relativeOffset);
            }
        }
    }
    void CSpreadsheet::print() const
    {
        for(const auto& cell : m_table.m_cells)
        {
            if(cell.second->m_left)
                std::cout << cell.first.getColumnStr() << cell.first.getRow() << ":" << *cell.second << std::endl;
        }
    }
