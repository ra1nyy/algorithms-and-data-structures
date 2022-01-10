#include <optional> // C++17
#include <vector>
#include <list>
#include <iostream>
#include <string>
#include <algorithm>


// ===========================================================//
// класс Horn, представляющий хорновскую формулу //
// =========================================================//
class HornFormula
{
public:
    // класс представляющий импликацию
    class Implication
    {
    private:
        // левые переменные импликации
        std::vector<int> leftSide;

        // правая переменная импликации
        int rightSide;
    public:
        // получение левой части импликации
        const std::vector<int>& getleftSide() const;

        // получение правой части импликации
        int getRightSide() const;

        // добавить в левую часть
        void addInLeftSide(int);

        // выставить правую часть
        void setRightSide(int);
    };


    // класс представляющий ПОД
    class PureNegativeClause
    {
    private:
        // массив переменных, входящих в ПОД
        std::vector<int> clauseVars;
    public:
        // получение массива переменеых входящих в ПОД
        const std::vector<int>& getClauseVars() const;

        // добавление переменных в ПОД
        void addClause(int);
    };

 // static метод для пользователя - возвращает:
 //                                 решение, если решение существует
 //                                 std::nullopt, если решения не существует
    static std::optional<std::vector<bool>> getSatisfyingAssignment(size_t countOfVars);

private:
    HornFormula(size_t count);

    // алгоритм поиска выполняющего набора хорновской формулы
    void findSatisfyingAssignment();

    // проверка массива переменных на исключительно true значения
    const bool isAllVarsTrue(const std::vector<int>& vec) const;

    // возврат флага отвечающего за существование решений
    bool isSolutionsExist() const;

    // получить значения переменных
    const std::vector<bool>& getValuesOfVars() const;

    // массив значений переменных
    std::vector<bool> vars_;

    // список импликаций
    std::list<Implication> implications_;

    // список ПОД
    std::list<PureNegativeClause> disjunctions_;

    // true - если формула уже обработана
    // false - если нет
    bool isSolutionsAlreadyFound_;

    // true - если решения есть
    // false - если нет
    bool isSolutionsExist_;
};


//=================== Реализации методов ============================

//============ реализация методов Implication ==================
const std::vector<int>& HornFormula::Implication::getleftSide() const
{
    return leftSide;
}


int HornFormula::Implication::getRightSide() const
{
    return rightSide;
}


void HornFormula::Implication::addInLeftSide(int var)
{
    leftSide.push_back(var);
}


void HornFormula::Implication::setRightSide(int var)
{
    rightSide = var;
}


//============ реализация методов Disjunction =========
const std::vector<int>& HornFormula::PureNegativeClause::getClauseVars() const
{
    return clauseVars;
}


void HornFormula::PureNegativeClause::addClause(int var)
{
    clauseVars.push_back(var);
}


//========реализация методов Horn ================
HornFormula::HornFormula(size_t count) :
    vars_(count, false), // изначально все переменные - false
    implications_(0),
    disjunctions_(0),
    isSolutionsAlreadyFound_(false),
    isSolutionsExist_(true)
{
    // обработка импликаций
    size_t count_of_implications;
    std::cin >> count_of_implications;
    for (size_t i = 0; i < count_of_implications; ++i)
    {
        Implication implic;
        std::string str;
        while (std::cin >> str && str[0] != '>')
        {
            implic.addInLeftSide(std::stoi(str));
        }
        std::cin >> str;
        implic.setRightSide(std::stoi(str));
        implications_.push_back(implic);
    }

    // обработка ПОД
    size_t count_of_clauses;
    std::cin >> count_of_clauses;
    for (size_t i = 0; i < count_of_clauses; ++i)
    {
        PureNegativeClause disjunct;
        std::string var;
        while (std::cin >> var && var[0] != '.')
        {
            disjunct.addClause(std::stoi(var));
        }
        disjunctions_.push_back(disjunct);
    }
}


// алгоритм прроверки выполнимости хорновской формулы
void HornFormula::findSatisfyingAssignment() // O(L^2)
{
    // если решение для донной хорновской формулы ранее было найдено
    if (isSolutionsAlreadyFound_)
    {
        return;
    }

    isSolutionsAlreadyFound_ = true;

    bool isVarChanged = true;
    while (isVarChanged) // O(I^2 * N)
    {
        isVarChanged = false;
        auto it = implications_.begin();
        while (it != implications_.end() && !isVarChanged) // O(I*N) 1.1
        {
            bool isLeftSideTrue = it->getleftSide().empty()
            || isAllVarsTrue(it->getleftSide()); // O(N)

            auto rightSideOfIMPL = it->getRightSide();

            // если слева истина а справа ложь
            if (!vars_[rightSideOfIMPL] && isLeftSideTrue) // O(1)
            {
                vars_[rightSideOfIMPL] = true;
                isVarChanged = true; 
            }

            // если правая часть истинна
            if (vars_[rightSideOfIMPL]) // O(1)
            {
                auto to_erase = it++;
                // удаляем эту импликацию
                implications_.erase(to_erase); // O(1)!
            }

            // если справа и слева false - смотрим следующую импликацию
            else
            {
                ++it;
            }
        }
    }
    for (auto it = disjunctions_.begin(); it != disjunctions_.end(); ++it) // O(D*N)
    {
        // если все переменные в ПОД положительны при полученном наборе
        if (isAllVarsTrue(it->getClauseVars()))
        {
            // решений нет
            isSolutionsExist_ = false;
            break;
        }
    }
}


const bool HornFormula::isAllVarsTrue(const std::vector<int>& vec) const
{
    for (auto v : vec)
    {
        if (!vars_[v])
        {
            return false;
        }
    }
    return true;
}


bool HornFormula::isSolutionsExist() const
{
    return isSolutionsExist_;
}


const std::vector<bool>& HornFormula::getValuesOfVars() const
{
    return vars_;
}


std::optional<std::vector<bool>> HornFormula::getSatisfyingAssignment(size_t countOfVars)
{
    // конуструирование формулы
    HornFormula horn(countOfVars);

    // поиск выполнняющего набоа
    horn.findSatisfyingAssignment();

    // повека наличия вып. набора
    if (horn.isSolutionsExist())
    {
        return horn.getValuesOfVars();
    }

    // если вып. набора не существует
    return std::nullopt;
}


int main()
{
    size_t countOfVars;
    std::cin >> countOfVars;

    // вызываем static метод Horn, внутри которого будет создан обьект Horn'a
    // на выходе получим либо выполняющий набор для введенной формулы
    // либо std::nullopt
    auto vars = HornFormula::getSatisfyingAssignment(countOfVars);

    // решений не существует
    if (!vars)
    {
        std::cout << std::endl << "Satisfying assignment doesn't exist";
    }

    // решение существует
    else
    {
        auto& vec = *vars;
        std::cout << std::endl << "Solutions: " << std::endl << std::boolalpha;
        for (size_t i = 0; i < vec.size(); ++i)
        {
        std::cout << i << " : " << vec[i] << std::endl;
        }
    }

    std::cout << std::endl;

    return 0;
}
