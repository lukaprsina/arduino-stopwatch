#include "include/utils.h"

// Number of milliseconds to ignore button input if a button bounces (2 is enough).
#define DEBOUNCE_TIME 100

Toggle::Toggle()
    : value(false, false), last_press_time(0)
{
}

void Toggle::update()
{
    if (this->get_state)
    {
        // update the state to check if it is toggled
        bool state = this->get_state();
        this->value.new_value(state);
    }

    if (this->is_pressed())
    {
        // call the rising edge callback
        if (this->on_press)
            this->on_press();

        // set the last press time for debounce check
        this->last_press_time = TIME_DELTA.current;
    }

    if (this->is_released())
    {
        // call the falling edge callback
        if (this->on_release)
            this->on_release();
    }
}

bool Toggle::is_pressed()
{
    // time since last press
    // absolute value because of negative value when initializing
    unsigned long debounce_timer = abs((long long)this->last_press_time - (long long)TIME_DELTA.current);

    // ignore press if too soon
    if (debounce_timer < DEBOUNCE_TIME)
        return false;

    return this->value.current && !this->value.previous;
}

bool Toggle::is_released()
{
    unsigned long debounce_timer = abs((long long)this->last_press_time - (long long)TIME_DELTA.current);

    if (debounce_timer < DEBOUNCE_TIME)
        return false;

    return !this->value.current && this->value.previous;
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
