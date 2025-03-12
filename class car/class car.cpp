#include<iostream>
#include<conio.h>
#include<Windows.h>
#include<thread>

using namespace std::literals::chrono_literals;
using std::cout;
using std::cin;
using std::endl;


#define Enter              13
#define Escape             27
#define UpArrow            72
#define DownArrow          80

#define MIN_TANK_CAPACITY  20
#define MAX_TANK_CAPACITY  120
//define (определить).
//Директива define создает макроопределение, которое показывает компилятору ЧТО_ЗАМЕНИТЬ

class Tank
{
    const int CAPACITY;
    double fuel_level;
public:
    int get_CAPACITY() const
    {
        return CAPACITY;
    }
    double get_fuel_level()const
    {
        return fuel_level;
    }
    double fill(double amount)
    {
        if (amount < 0) return fuel_level;
        fuel_level += amount;
        if (fuel_level > CAPACITY) fuel_level = CAPACITY;
        return fuel_level;
    }
    double give_fuel(double amount)
    {
        fuel_level -= amount;
        if (fuel_level < 0) fuel_level = 0;
        return fuel_level;
    }

    // CONSTRUCTORS
    Tank(int capacity)
        : CAPACITY
        (
            capacity<MIN_TANK_CAPACITY ? MIN_TANK_CAPACITY :
            capacity>MAX_TANK_CAPACITY ? MAX_TANK_CAPACITY :
            capacity
        ), fuel_level(0) //Инициализация в заголовке.
        //Если в классе есть константа, то ее можно проинициализировать только в заголовке.
        //Инициализация в заголовке отрабатывает еще до того, как отрабатывает тело конструктора
    {
        //this->CAPACITY = capacity;
        //this->fuel_level = 0;
        cout << "Tank is ready:\t" << this << "\n";
    }
    ~Tank()
    {
        cout << "Destructor for:\t" << this << "\n";
    }

    void info()const
    {
        cout << "Capacity\t" << get_CAPACITY() << " liters\n";
        cout << "Fuel level\t" << get_fuel_level() << " liters\n";
    }


};

#define MIN_ENGINE_CONSUMPTION   4
#define MAX_ENGINE_CONSUMPTION  30


class Engine {
    const double CONSUMPTION;
    double consumption_per_second;
    const double DEFAULT_CONSUMPTION_PER_SECOND;
    bool is_started;
public:
    double get_CONSUMPTION()const {
        return CONSUMPTION;
    }
    double get_consumption_per_second()const {
        return consumption_per_second;
    }
    double get_consumption_per_second(int speed) {
        if (speed == 0) this->consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND;
        else if (speed < 60) this->consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 20 / 3;
        else if (speed < 100) this->consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 14 / 3;
        else if (speed < 140) this->consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 20 / 3;
        else if (speed < 200) this->consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 25 / 3;
        else this->consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 10;
        return consumption_per_second;
    }

    // CONSTRUCTORS
    Engine(double consumption) :CONSUMPTION (
        consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
        consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
        consumption
    ), DEFAULT_CONSUMPTION_PER_SECOND(CONSUMPTION*3e-5) {
        //set_consumption_per_second();
        get_consumption_per_second();
        is_started = false;
        cout << "Engine is ready:\t" << this << "\n";
    }
    ~Engine() {
        cout << "Engine is over:\t" << this << "\n";
    }

    void start() {
        is_started = true;
    }

    void stop() {
        is_started = false;
    }

    bool started()const {
        return is_started;
    }

    void info()const
    {
        cout << "Consumption:\t\t" << get_CONSUMPTION() << " liters\n";
        cout << "Consumption:\t\t" << get_consumption_per_second() << " liters/s\n";
    }

};

#define MIN_SPEED_LIMIT 30
#define MAX_SPEED_LIMIT 408

class Car {
    Engine engine;
    Tank tank;
    bool driver_inside;
    int speed;
    const int MAX_SPEED = 180;
    int acceleration;
    struct
    {
        std::thread panel_thread;
        std::thread engine_idle_thread;
        std::thread free_wheeling_thread;
    }control;

public:
    Car(double consumption, int capacity, int max_speed) :
        engine(consumption),
        tank(capacity),
        MAX_SPEED(
        max_speed < MIN_SPEED_LIMIT ? MIN_SPEED_LIMIT :
        max_speed > MAX_SPEED_LIMIT ? MAX_SPEED_LIMIT :
        max_speed){
        acceleration = 20;
        driver_inside = false;
        speed = 0;
        cout << "Your car is ready to go!" << "\n";
        cout << "Press Enter to get in." << "\n";
    }
    ~Car() {
        cout << "Your car is over.";
    }
    void get_in() {
        driver_inside = true;
        control.panel_thread = std::thread(&Car::panel, this);
    }
    void get_out() {
        driver_inside = false;
        if (control.panel_thread.joinable()) {
            control.panel_thread.join();
            cout << "Вы вышли из машины!\n";
        }
    }
    void panel()const {
        while (driver_inside) {
            system("cls");
            for (int i = 0; i < speed/2.5; i++) cout << "|";
            cout << "Speed " << speed << " km/h" << endl;
            cout << "Fuel level: " << tank.get_fuel_level() << " litters." << endl;
            if (tank.get_fuel_level() < 5) {
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, 0xCF);
                cout << "LOW FUEL!!!";
                SetConsoleTextAttribute(hConsole, 0x07);
            }
            cout << endl;
            cout << "Engine is " << (engine.started() ? "started" : "stopped") << endl;
            cout << "Consumption: " << engine.get_consumption_per_second() << " liters/s" << endl;
            cout << "DEBUG:" << endl;
            cout << "Friction enabled: " << control.free_wheeling_thread.joinable() << endl;
            Sleep(500);
        }
    }

    void engine_idle() {
        while (tank.give_fuel(engine.get_consumption_per_second(this->speed)) && engine.started()) {
            std::this_thread::sleep_for(1s);
        }
    }

    void start() {
        if (tank.get_fuel_level()) {
            engine.start();
            control.engine_idle_thread = std::thread(&Car::engine_idle, this);
        }
        else cout << "Проверьте уровень топлива\n";
    }

    void stop() {
        engine.stop();
        if (control.engine_idle_thread.joinable())
            control.engine_idle_thread.join();
    }

    void free_wheeling() {
        while (speed > 0) {
            speed -= 3;
            if (speed < 0) speed = 0;
            std::this_thread::sleep_for(1s);
            
        }
    }

    void accelerate() {
        if (driver_inside && engine.started()) {
            speed += acceleration;
            if (speed > MAX_SPEED) speed = MAX_SPEED;
            if (speed > 0 && !control.free_wheeling_thread.joinable())
                control.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
            std::this_thread::sleep_for(1s);
        }
    }

    void slow_down() {
        if (driver_inside && speed > 0) {
            speed -= acceleration;
            if (speed < 0) speed = 0;
            if (speed == 0 && control.engine_idle_thread.joinable())
                control.free_wheeling_thread.join();
            std::this_thread::sleep_for(1s);
        }
    }

    void control_car() {
        char key = 0;
        do {
            key = 0;
            //функция _kbhit() в фоновом режиме отслеживает нажатие клавиш
            //если клавиша была нажата то функция возвращает true. иначе - false
            if (_kbhit()) {
                key = _getch();
                switch (key) {
                case Enter:
                    driver_inside ? get_out() : get_in();
                    break;
                case 'F':
                case 'f':
                    if (engine.started() || driver_inside) {
                        cout << "Для начала заглушите мотор." << "\n";
                    }
                    else {
                        double fuel_level;
                        cout << "Введите объём топлива: ";
                        cin >> fuel_level;
                        tank.fill(fuel_level);
                    }
                    break;
                case 'I':
                case 'i':
                    if (driver_inside) engine.started() ? stop() : start();
                    break;
                case 'W':
                case 'w':
                case UpArrow:
                    accelerate();
                    break;
                case 'S':
                case 's':
                case DownArrow:
                    slow_down();
                    break;
                case Escape:
                    stop();
                    get_out();
                }
                if (speed == 0 && control.free_wheeling_thread.joinable())
                    control.free_wheeling_thread.join();
            }
            //функция _getch() ожидает нажатие клавиши и возвращает ASCII-код нажатой клавиши
            //т.е _kbhit() отслеживает сам факт нажатия на клавишу(неважно какую), а getch() опрежедяет,
            //какая клавиша была нажата
        } while (key != Escape);
    }
    void info()const {
        engine.info();
        tank.info();
    }
};


//#define TANK_CHECK //определяем TANK_CHECK

void main(){
    system("chcp 1251 > NUL");



#ifdef TANK_CHECK // если определён
    Tank tank(60);
    tank.info();

    int fuel;
    do {
        std::cout << "Введите объем топлива: "; std::cin >> fuel;
        tank.fill(fuel);
        tank.info();
    } while (fuel > 0);
#endif //TANK_CHECK

#ifdef ENGINE_CHECK
    Engine engine(10);
    engine.info();
#endif

    Car bmw(10.0, 80, 180);
    bmw.control_car();
    bmw.info();

}