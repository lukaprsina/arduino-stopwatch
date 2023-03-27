#pragma once

/// @brief Keep the current and previous value of T.
/// @tparam T The type of current and previous value.
template <class T>
class PreviousState
{
public:
    PreviousState(T current, T previous);

    /// @brief Set previous to current and current to value.
    /// @param value New value.
    void new_value(T value);

    /// @brief Calculate the difference between current and previous.
    T difference();

    T previous;
    T current;
};

/// @brief Calls callbacks when the value gets toggled (rising edge and falling edge).
class Toggle
{
public:
    Toggle();

    /// @brief Call every loop. Triggers callbacks.
    void update();

    bool is_pressed();
    bool is_released();

    /// @brief Tell the toggle the state (example: if the pin is HIGH return true and false if LOW).
    bool (*get_state)();
    /// @brief Gets called on rising edge (with debouncing)
    void (*on_press)();
    /// @brief Gets called on falling edge (with debouncing)
    void (*on_release)();

    /// @brief Keep a previous state of the value to check if it is toggled.
    PreviousState<bool> value;

    /// @brief Keep time of last press for debouncing the value.
    unsigned long last_press_time;
};