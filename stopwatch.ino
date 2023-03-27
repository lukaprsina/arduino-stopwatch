#include "include/stopwatch.h"

/// @brief Number segments for 7-segment display plus a clear screen digit.
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

/// @brief Multiplexer configuration.
bool TRANSISTOR_INSTRUCTIONS[5][4] = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
    {0, 0, 0, 0},
};

uint8_t decimal_point_pin = 9;

/// @brief True if the stopwatch is running
bool KEEPING_TIME = false;

/// @brief Append passed milliseconds every loop if the stopwatch is not running. This is then subtracted from the displayed time.
unsigned long TIME_STOPPED = 0;

/// @brief Keep the previous value of milliseconds to get the delta time.
PreviousState<unsigned int> TIME_DELTA((unsigned int)millis(), (unsigned int)0);
StopwatchTime DISPLAY_TIME;
InputOutputState STATE;

void on_reset_press()
{
    // stop the stopwatch
    KEEPING_TIME = false;

    // display t=0
    TIME_STOPPED = TIME_DELTA.current;
}

void on_start_stop_press()
{
    // toggle the stopwatch
    KEEPING_TIME = !KEEPING_TIME;
}

void setup()
{
    Serial.begin(9600);

    // initialize 7-segment display pin to OUTPUT
    for (uint8_t pin = 2; pin <= 13; pin++)
        pinMode(pin, OUTPUT);

    // the reset and start/stop buttons are pullup buttons
    pinMode(InputOutputState::Pin::Reset, INPUT_PULLUP);
    pinMode(InputOutputState::Pin::StartStop, INPUT_PULLUP);

    // set callbacks for button toggles
    STATE.reset.on_press = &on_reset_press;
    STATE.start_stop.on_press = &on_start_stop_press;
}

void loop()
{
    // store the current and previous time to get the time since the last loop
    TIME_DELTA.new_value(millis());

    // TIME_STOPPED is subtracted from the current time to get the display time
    if (!KEEPING_TIME)
        TIME_STOPPED += TIME_DELTA.difference();

    // update the IO state
    STATE.update();

    // set the display time by subtracting TIME_STOPPED from the current time
    DISPLAY_TIME.update(TIME_DELTA.current - TIME_STOPPED);
    display_stopwatch_time(DISPLAY_TIME);
}

void StopwatchTime::update(unsigned long miliseconds)
{
    this->seconds = miliseconds / 1000;
    this->minutes = this->seconds / 60;
}

bool get_reset_value()
{
    return digitalRead(InputOutputState::Pin::Reset);
}

bool get_start_stop_value()
{
    return digitalRead(InputOutputState::Pin::StartStop);
}

InputOutputState::InputOutputState()
{
    // set callbacks for reading the reset and start stop toggle buttons
    this->reset.get_state = &get_reset_value;
    this->start_stop.get_state = &get_start_stop_value;
}

void InputOutputState::update()
{
    // call callbacks of buttons
    this->reset.update();
    this->start_stop.update();
}

void display_stopwatch_time(StopwatchTime &time)
{
    // two digits for minutes and two digits for seconds
    uint8_t digits[4] = {time.seconds % 10, time.seconds / 10, time.minutes % 10, time.minutes / 10};

    for (uint8_t i = 0; i < 4; i++)
    {
        // empty digit
        display_digit(10);
        // no transistors
        mux(4);

        // display the digit "digits[i]" in the i-th place
        display_digit(digits[i]);
        mux(i);
    }

    display_digit(10);
    mux(4);
}

void display_digit(uint8_t digit)
{
    for (uint8_t i = 0; i < 7; i++)
    {
        // turn the i-th segment on or off
        digitalWrite(i + 2, DISPLAY_INSTRUCTIONS[digit][i]);

        // hide the decimal point
        digitalWrite(decimal_point_pin, 0);
    }
}

void mux(uint8_t digit_position)
{
    // set the separating decimal point between hours and minutes
    if (digit_position == 2)
        digitalWrite(decimal_point_pin, 1);
    else
        digitalWrite(decimal_point_pin, 0);

    // adjust the multiplexer transistors
    for (uint8_t i = 0; i < 4; i++)
    {
        digitalWrite(i + 10, TRANSISTOR_INSTRUCTIONS[digit_position][i]);
    }

    delay(1);
}