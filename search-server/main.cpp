// Решите загадку: Сколько чисел от 1 до 1000 содержат как минимум одну цифру 3?
// Напишите ответ здесь:
#include <iostream>
#include <set>
#include <string>
#include <clocale>
using namespace std;

void addToSet(int a, set<char>& digits) {
	string number = to_string(a);
	for (const char& i : number)
		digits.insert(i);
}


int main() {
	setlocale(LC_ALL, "Russian");
	set<char> digits;
	int counter = 0;
	for (int i = 0; i <= 1000; ++i)
	{
		addToSet(i, digits);
		if (digits.count('3'))
			++counter;
		digits.clear();
	}
	cout << "Цифру 3 содержат(ит) " << counter << " числа(о)" << endl;
	return 0;
}


// Закомитьте изменения и отправьте их в свой репозиторий.
