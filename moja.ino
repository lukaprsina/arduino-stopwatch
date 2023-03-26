class TimeInfo
{
public:
    TimeInfo(unsigned long miliseconds);

    uint8_t minutes;
    uint8_t seconds;
};

void display_time_info(TimeInfo time_info);

template <class T>
class PreviousState
{
public:
    PreviousState(T current, T previous);
    void new_value(T value);
    T difference();
    bool has_changed();

    T previous;
    T current;
};

class Toggle
{
public:
    Toggle(int pin);
    void update();
    bool is_pressed();
    bool is_released();

    void (*on_press)();
    void (*on_release)();

    int pin;
    PreviousState<bool> value;
    unsigned long press_time;
};

class InputState
{
public:
    InputState();
    void update();

    Toggle reset;
    Toggle start_stop;

    enum Pin
    {
        Reset = A4,
        StartStop = A5,
    };
};

bool DISPLAY_INSTRUCTIONS[11][7] = {
    {1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 0, 1},
    {1, 1, 1, 1, 0, 0, 1},
    {0, 1, 1, 0, 0, 1, 1},
    {1, 0, 1, 1, 0, 1, 1},
    {1, 0, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 1, 1},
    {0, 0, 0, 0, 0, 0, 0},
};

bool LED_INSTRUCTIONS[5][4] = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
    {0, 0, 0, 0},
};

uint8_t dp = 9;
bool KEEPING_TIME = false;
unsigned long TIME_STOPPED = 0;

PreviousState<unsigned int> TIME((unsigned int)millis(), (unsigned int)0);
InputState STATE;

void on_pressed_reset()
{
    KEEPING_TIME = false;
    TIME_STOPPED = millis();
    Serial.print("PRESSED RESET");
    Serial.println();
}

void on_pressed_start_stop()
{
    KEEPING_TIME = !KEEPING_TIME;
    Serial.print("PRESSED START STOP, KEEPING TIME: ");
    Serial.print(KEEPING_TIME);
    Serial.println();
}

void setup()
{
    Serial.begin(9600);

    for (uint8_t pin = 2; pin <= 13; pin++)
        pinMode(pin, OUTPUT);

    pinMode(InputState::Pin::Reset, INPUT_PULLUP);
    pinMode(InputState::Pin::StartStop, INPUT_PULLUP);

    STATE.reset.on_press = &on_pressed_reset;
    STATE.start_stop.on_press = &on_pressed_start_stop;
}

void loop()
{
    TIME.new_value(millis());

    if (!KEEPING_TIME)
        TIME_STOPPED += TIME.difference();

    STATE.update();

    TimeInfo time_info(TIME.current - TIME_STOPPED);
    display_time_info(time_info);

    Serial.print("TIME.current: ");
    Serial.print(TIME.current);
    Serial.print(", TIME.previous: ");
    Serial.print(TIME.previous);
    Serial.print(", TIME_STOPPED: ");
    Serial.print(TIME_STOPPED);
    Serial.print(", KEEPING_TIME: ");
    Serial.print(KEEPING_TIME);
    Serial.print(", min: ");
    Serial.print(time_info.minutes);
    Serial.print(", sec: ");
    Serial.print(time_info.seconds);
    Serial.println();

    delay(500);
}

InputState::InputState()
    : reset(InputState::Pin::Reset), start_stop(InputState::Pin::StartStop)
{
    // arduino restarts wtf
    Serial.println("WTFAWTAWAWTAWTAWTAWTAWTAWT");
}

void InputState::update()
{
    this->reset.update();
    this->start_stop.update();
}

Toggle::Toggle(int pin)
    : value(false, false)
{
    this->pin = pin;
    this->press_time = 0;
}

void Toggle::update()
{
    this->value.new_value(digitalRead(this->pin));
    if (this->is_pressed())
    {
        this->on_press();

        if (!this->value.current && this->value.previous)
            this->press_time = TIME.current;
    }

    if (this->is_released())
    {
        this->on_release();
    }
}

bool Toggle::is_pressed()
{
    unsigned long sheise = abs((long long)this->press_time - (long long)TIME.current);

    Serial.print("Sheise: ");
    Serial.print(sheise);
    Serial.println();
    if (sheise < 500)
        return false;

    return this->value.current && !this->value.previous;
}

bool Toggle::is_released()
{
    // TODO: debounce
    return !this->value.current && this->value.previous;
}

TimeInfo::TimeInfo(unsigned long miliseconds)
{
    this->seconds = miliseconds / 1000;
    this->minutes = this->seconds / 60;
}

template <typename T>
PreviousState<T>::PreviousState(T current, T previous)
{
    this->current = current;
    this->previous = previous;
}

template <typename T>
void PreviousState<T>::new_value(T value)
{
    this->previous = this->current;
    this->current = value;
}

template <typename T>
T PreviousState<T>::difference()
{
    return this->current - this->previous;
}

template <typename T>
bool PreviousState<T>::has_changed()
{
    return this->current != this->previous;
}

void mux(uint8_t modul)
{
    if (modul == 2)
        digitalWrite(dp, 1);
    else
        digitalWrite(dp, 0);

    for (uint8_t i = 0; i < 4; i++)
    {
        digitalWrite(i + 10, LED_INSTRUCTIONS[modul][i]);
    }
    delay(1);
}

void display_number(uint8_t number)
{
    for (uint8_t i = 0; i < 7; i++)
    {
        digitalWrite(i + 2, DISPLAY_INSTRUCTIONS[number][i]);
        digitalWrite(dp, 0);
    }
}

void display_time_info(TimeInfo time_info)
{
    uint8_t digits[4] = {time_info.seconds % 10, time_info.seconds / 10, time_info.minutes % 10, time_info.minutes / 10};

    for (uint8_t i = 0; i < 4; i++)
    {
        display_number(10);
        mux(4);
        display_number(digits[i]);
        mux(i);
    }

    display_number(10);
    mux(4);
}