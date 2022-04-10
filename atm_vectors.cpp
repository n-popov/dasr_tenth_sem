#include <vector>
#include <unordered_map>
#include <set>
#include <numeric>

class ATM
{
public:
    // Конструктор, создаёт пустой банкомат
    ATM(): money({{0, {}},
                  {1, {}},
                  {2, {}}}) {}

    // Деструктор, если нужен

    // Вносит в устройство набор купюр
    // - в векторе notes перечислены достоинства вносимых купюр (разумеется, возможны повторы)
    // - в поле currency указан код валюты
    void deposit(const std::vector<unsigned int>& notes, unsigned short int currency) {
        std::copy(std::begin(notes), std::end(notes),
                  std::inserter(money[currency], std::begin(money[currency])));
    }

    template<typename Comparator>
    std::vector<unsigned int> withdraw(unsigned amount, unsigned short currency) {
        std::vector<unsigned int> result = {};
        std::sort(std::begin(money[currency]),
                  std::end(money[currency]),
                  Comparator());
        for(auto& note: money[currency]) {
            if (note <= amount) {
                amount -= note;
                result.push_back(note);
                note = 0;
            }
        }
        auto remove_from = std::remove(std::begin(money[currency]),
                                       std::end(money[currency]),
                                       0u);
        money[currency].erase(remove_from, std::end(money[currency]));
        if (amount > 0) {
            std::copy(std::begin(result), std::end(result),
                      std::back_inserter(money[currency]));
            return {};
        }
        return result;
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
        auto& money_for_currency = money[currency];
        return std::accumulate(std::begin(money_for_currency),
                               std::end(money_for_currency),
                               0u);
    }

private:
    std::unordered_map<unsigned short, std::vector<unsigned>> money;
};
//Указание 1: банкомат поддерживает строго 3 валюты, можно закладываться, что код currency будет всегда строго из набора {0, 1, 2}, некорректные коды можно не обрабатывать.
//
//Указание 2 (важное!): в полной постановке это достойная комбинаторная задача. Но сейчас в контрольной полное решение можно не писать. Достаточно локально жадно перебирать купюры с заданного конца, пропуская заведомо неподходящие и набирая запрошенную сумму из остальных. Например, у вас есть купюры {100, 500, 500, 1000, 2000}. Если запрошена сумма "1000 крупными", то идём от больших к меньшим, находим подходящую 1000, выдаём её - всё хорошо. Но если запрошена сумма "1000 мелкими", то такой алгоритм поведёт себя плохо - найдёт и отложит 100 (текущая сумма 100), затем найдёт и отложит 500 (текущая сумма 600), затем найдёт ещё 500 и с грустью сообщит, что выдать 1000 мелкими невозможно. В жизни так писать, конечно, не надо. Но сейчас в условиях цейтнота на контрольной такой локально жадный алгоритм будет достаточен - сложных случаев просто не будет в тестах.
//
//Ремарка 1: в реальности для кода валюты было бы логичнее использовать enum-ы. Но в контрольной мы не стали.
//
//Ремарка 2: в реальности из операций снятия было бы логично бросать exception-ы. Но в данной задаче мы не стали.
//
//Для базового тестирования можете использовать следующий пример:

#include <iostream>
#include <vector>

using namespace std;

const unsigned short int RUB = 0;
const unsigned short int CNY = 1;
const unsigned short int KZT = 2;

int main()
{
    vector<unsigned int> rubles = {50, 100, 100, 500, 50, 1000, 100, 100, 1000, 5000};
    vector<unsigned int> dollars = {100, 100, 100};
    vector<unsigned int> nuka_cola_caps = {1, 1, 1, 1, 1};

    ATM atm;
    atm.deposit(rubles, RUB);
    atm.deposit(dollars, CNY);
    atm.deposit(nuka_cola_caps, KZT);

    cout << "Reserves: " << endl;
    for(unsigned short int code = 0; code <= 2; code++) {
        cout << "Currency " << code << ": " << atm.check_reserve(code) << endl;
    }

    vector<unsigned int> result;
    result = atm.withdraw_large(1000, RUB);
    cout << "Asked withdraw_large for 1000 RUB, got " << result.size() << " banknotes" << endl;

    result = atm.withdraw_small(1000, RUB);
    cout << "Asked withdraw_small for 1000 RUB, got " << result.size() << " banknotes" << endl;

    result = atm.withdraw_small(1000, CNY);
    cout << "Asked withdraw_small for 1000 CNY, got " << result.size() << " banknotes" << endl;

    result = atm.withdraw_large(3, KZT);
    cout << "Asked withdraw_large for 3 KZT, got " << result.size() << " banknotes" << endl;

    cout << "Reserves: " << endl;
    for(unsigned short int code = 0; code <= 2; code++) {
        cout << "Currency " << code << ": " << atm.check_reserve(code) << endl;
    }

    return 0;
}