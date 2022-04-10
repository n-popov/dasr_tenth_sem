#include <iostream>

#include <vector>
#include <list>
#include <map>
#include <numeric>

class NotEnoughMoneyInAtmException : public std::exception {
public:
    explicit NotEnoughMoneyInAtmException(const std::string &message) :
            message(message) {}

    const char *what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

class ATM {
public:
    // Конструктор, создаёт пустой банкомат
    ATM() : storage({
                            {0, {}},
                            {1, {}},
                            {2, {}},
                    }) {}

    // Деструктор, если нужен

    // Вносит в устройство набор купюр
    // - в векторе notes перечислены достоинства вносимых купюр (разумеется, возможны повторы)
    // - в поле currency указан код валюты
    void deposit(const std::vector<unsigned int> &notes,
                 unsigned short int currency) {
        auto &needed_list = storage[currency];
        std::copy(std::begin(notes), std::end(notes),
                  std::back_inserter(needed_list));
    }

    // Снять сумму amount в валюте currency, выдав её максимально крупными купюрами
    // - если операция прошла успешно, учесть выданные купюры в банкомате и вернуть их в возвращаемом vector-е
    // - если целиком запрошенную сумму выдать невозможно, не выдавать ничего и вернуть пустой vector
    std::vector<unsigned int> withdraw_large(unsigned int amount, unsigned short int currency) {
        return withdraw<std::greater<>>(amount, currency);
    }

    // Снять сумму amount в валюте currency, выдав её максимально мелкими купюрами
    // - если операция прошла успешно, учесть выданные купюры в банкомате и вернуть их в возвращаемом vector-е
    // - если целиком запрошенную сумму выдать невозможно, не выдавать ничего и вернуть пустой vector
    std::vector<unsigned int> withdraw_small(unsigned int amount, unsigned short int currency) {
        return withdraw<std::less<>>(amount, currency);
    }

    // Вернуть максимальную сумму, доступную в валюте currency
    unsigned int check_reserve(unsigned short int currency) {
        auto &needed_list = storage[currency];
        return std::accumulate(std::begin(needed_list),
                               std::end(needed_list),
                               0, [](auto x, auto y) {
                    return x + y;
                });
    }

private:
    template<typename Comparator>
    std::vector<unsigned int> withdraw(unsigned int amount, unsigned short int currency) {
        auto &needed_list = storage[currency];
        needed_list.sort(Comparator());
        std::list<unsigned> result;
        for (auto it = needed_list.begin();
             it != needed_list.end();) {
            auto next_iter = std::next(it);
            if (amount >= *it) {
                amount -= *it;
                result.splice(result.begin(), needed_list, it, std::next(it));
            }
            it = next_iter;
        }
        if (amount == 0) {
            std::vector<unsigned> result_vector(std::size(result));
            std::copy(result.begin(), result.end(), std::begin(result_vector));
            return result_vector;
        } else {
            needed_list.splice(std::begin(needed_list), result);

            throw NotEnoughMoneyInAtmException("Required too much");
        }
    }

    std::map<unsigned short, std::list<unsigned>> storage;
};


#include <iostream>
#include <vector>

using namespace std;

const unsigned short int RUB = 0;
const unsigned short int USD = 1;
const unsigned short int NCC = 2;

int main() {
    vector<unsigned int> rubles = {50, 100, 100, 500, 50, 1000, 100, 100, 1000, 5000};
    vector<unsigned int> dollars = {100, 100, 100};
    vector<unsigned int> nuka_cola_caps = {1, 1, 1, 1, 1};

    ATM atm;
    atm.deposit(rubles, RUB);
    atm.deposit(dollars, USD);
    atm.deposit(nuka_cola_caps, NCC);

    cout << "Reserves: " << endl;
    for (unsigned short int code = 0; code <= 2; code++) {
        cout << "Currency " << code << ": " << atm.check_reserve(code) << endl;
    }

    vector<unsigned int> result;
    result = atm.withdraw_large(1000, RUB);
    cout << "Asked withdraw_large for 1000 RUB, got " << result.size() << " banknotes" << endl;

    result = atm.withdraw_small(1000, RUB);
    cout << "Asked withdraw_small for 1000 RUB, got " << result.size() << " banknotes" << endl;

    try {
        result = atm.withdraw_small(1000, USD);
        cout << "Asked withdraw_small for 1000 USD, got " << result.size() << " banknotes" << endl;
    } catch (NotEnoughMoneyInAtmException& ex) {
        std::cout << ex.what() << std::endl;
    }
    result = atm.withdraw_large(3, NCC);
    cout << "Asked withdraw_large for 3 NCC, got " << result.size() << " banknotes" << endl;

    cout << "Reserves: " << endl;
    for (unsigned short int code = 0; code <= 2; code++) {
        cout << "Currency " << code << ": " << atm.check_reserve(code) << endl;
    }

    return 0;
}
