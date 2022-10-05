#include <iostream>
#include <list>
#include <algorithm>

struct Data {
    double temp_celsius;
    double pressure_hpa;
    double humidity;
};

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void update(const Data &) = 0;
};

class Station {
public:
    enum Flag {
        Temp = 1,
        Pressure = 2,
        Humidity = 4,
        ALL = 7
    };

    void set_temp(double new_temp) {
        data.temp_celsius = new_temp;
        notify_observers(Flag::Temp);
    }
    
    void set_pressure(double new_pressure) {
        data.pressure_hpa = new_pressure;
        notify_observers(Flag::Pressure);
    }

    void set_hum(double new_humidity) {
        data.humidity = new_humidity;
        notify_observers(Flag::Humidity);
    }
    
    double get_temp() const {
        return data.temp_celsius;
    }
    double get_pressure() const {
        return data.pressure_hpa;
    }
    double get_hum() const {
        return data.humidity;
    }
    int get_number_of_observers() const {
        return observers.size();
    }

    void register_observer(IObserver *obs, int flags) {
        auto it = std::find(observers.begin(), observers.end(), obs);

        if (it != observers.end())
            it->flags = flags;
        else
            observers.push_back({flags, obs});
    }
    void unregister_observer(IObserver *obs) {
        auto it = std::find(observers.begin(), observers.end(), obs);
        
        if (it != observers.end())
            observers.erase(it);
    }
private:
    struct RegObserver {
        int flags;
        IObserver *ptr;

        bool operator==(const IObserver *obs) const {
            return ptr == obs;
        }
    };

    Data data;
    std::list<RegObserver> observers;

    void notify_observers(int flags) const {
        for (auto &v : observers)
            if (flags & v.flags)
                v.ptr->update(data);
    }
};

class DeviceTemp : public IObserver {
public:
    void update(const Data &data) override {
        temp = data.temp_celsius;
        std::cout << "Update to DeviceTemp\n";
    }
    double get_temp() const {
        return temp;
    }
private:
    double temp;
};

class Phone : public IObserver {
public:
    void update(const Data &_data) override {
        data = _data;
        std::cout << "Update to Phone\n";
    }
    double get_temp() const {
        return data.temp_celsius;
    }
    double get_hum() const {
        return data.humidity;
    }
    double get_pressure() const {
        return data.pressure_hpa;
    }
private:
    Data data;
};

int main()
{
    Station station;
    Phone phone;
    DeviceTemp dev;

    station.register_observer(&dev, Station::Flag::Temp);

    station.register_observer(&phone, Station::Flag::ALL);

    std::cout << "Updating temp\n";
    station.set_temp(5);
    
    std::cout << "Updating pressure\n";
    station.set_pressure(900);
    
    std::cout << "Updating humidity\n";
    station.set_hum(50);
}