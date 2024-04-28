#include <mutex>
#include <array>
#include <thread>
#include <memory>
#include <iostream>
#include <condition_variable>
#include <cstdlib>
#include <vector>

#define BUFFER_CAPACITY 20

class Vehicle
{
public:
    virtual void printProperties() const = 0;
    virtual ~Vehicle() {}

private:
    static int uniqueID;
    static std::mutex idMutex;

protected:
    static int getNextID();
};

int Vehicle::uniqueID = 0;
std::mutex Vehicle::idMutex;

int Vehicle::getNextID()
{
    std::lock_guard<std::mutex> lock(idMutex);
    return ++uniqueID + 1000;
}

class Car : public Vehicle
{
public:
    Car(int maxPassengers) : maxPassengers(maxPassengers)
    {
        id = getNextID();
    }

    virtual void printProperties() const override
    {
        std::cout << "== DEALER ==" << std::endl;
        std::cout << "ID: " << id << std::endl;
        std::cout << "Type: Car" << std::endl;
        std::cout << "Model: V40" << std::endl;
        std::cout << "Max Passengers: " << maxPassengers << std::endl;
        std::cout << std::endl;
    }

private:
    int id;
    int maxPassengers;
};

class Truck : public Vehicle
{
public:
    Truck(int maxLoadWeight) : maxLoadWeight(maxLoadWeight)
    {
        id = getNextID();
    }

    virtual void printProperties() const override
    {
        std::cout << "== DEALER ==" << std::endl;
        std::cout << "ID: " << id << std::endl;
        std::cout << "Type: Truck" << std::endl;
        std::cout << "Model: Vera" << std::endl;
        std::cout << "Max Load Weight: " << maxLoadWeight << std::endl;
        std::cout << std::endl;
    }

private:
    int id;
    int maxLoadWeight;
};

template <typename T>
class Warehouse
{
private:
    size_t head = 0;
    size_t tail = 0;
    size_t size = 0;
    std::mutex mutex;
    std::condition_variable condition;
    std::array<T, BUFFER_CAPACITY> buffer;

public:
    Warehouse() : head(0), tail(0), size(0) {}

    Warehouse(const Warehouse &) = delete;
    Warehouse &operator=(const Warehouse &) = delete;

    void push(const T &item)
    {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this]
                       { return size < BUFFER_CAPACITY; });
        buffer[tail] = item;
        tail = (tail + 1) % BUFFER_CAPACITY;
        ++size;
        lock.unlock();
        condition.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this]
                       { return size > 0; });
        T item = buffer[head];
        head = (head + 1) % BUFFER_CAPACITY;
        --size;
        lock.unlock();
        condition.notify_one();
        return item;
    }
};

void ProducerFunc(Warehouse<std::shared_ptr<Vehicle>> &buffer)
{
    while (true)
    {
        std::shared_ptr<Vehicle> ptr;
        int randomNum = rand() % 2;
        if (randomNum == 0)
            ptr = std::make_shared<Car>(5);
        else
            ptr = std::make_shared<Truck>(4000);
        buffer.push(ptr);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void ConsumerFunc(Warehouse<std::shared_ptr<Vehicle>> &buffer)
{
    static std::mutex outputMutex;

    while (true)
    {
        std::shared_ptr<Vehicle> ptr = buffer.pop();

        std::lock_guard<std::mutex> lock(outputMutex);

        ptr->printProperties();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <number_of_consumers>" << std::endl;
        return 1;
    }

    int numConsumers = std::stoi(argv[1]);
    if (numConsumers < 2)
    {
        std::cerr << "Number of consumers should be at least 2." << std::endl;
        return 1;
    }

    srand(time(nullptr));

    Warehouse<std::shared_ptr<Vehicle>> buffer;

    std::thread producer_thread(ProducerFunc, std::ref(buffer));

    std::vector<std::thread> consumer_threads;
    for (int i = 0; i < numConsumers; ++i)
    {
        consumer_threads.emplace_back(ConsumerFunc, std::ref(buffer));
    }

    producer_thread.join();
    for (auto &thread : consumer_threads)
    {
        thread.join();
    }

    return 0;
}
