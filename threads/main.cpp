#include<iostream>
#include<thread>
#include<chrono>

//потоки
using namespace std::literals::chrono_literals;
using std::cin;
using std::endl;
using std::cout;

bool finish = false;

void Plus() {
	while (!finish) {
		cout << "+ ";
		std::this_thread::sleep_for(100ms);
	}
}

void Minus() {
	while (!finish) {
		cout << "- ";
		std::this_thread::sleep_for(100ms);
	}
}

void main() {
	system("chcp 1251 > NUL");
	//Plus();
	//Minus();

	std::thread plus_thread(Plus);
	std::thread minus_thread(Minus);

	cin.get(); //waits for Enter
	finish = true;

	plus_thread.join();
	minus_thread.join();


}