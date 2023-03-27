#pragma once

#include "utils.h"

/// @brief Stores the stopwatch time.
class StopwatchTime
{
public:
    /// @brief Converts from miliseconds to minutes and seconds.
    /// @param miliseconds Current time - stopped time.
    void update(unsigned long miliseconds);

    uint8_t minutes;
    uint8_t seconds;
};

/// @brief Represents stopwatch IO.
class InputOutputState
{
public:
    InputOutputState();

    /// @brief Call every loop to communicate with IO.
    void update();

    /// @brief Reset time and stop counting when on rising edge.
    Toggle reset;

    /// @brief If stopped, continue counting and vice versa on rising edge.
    Toggle start_stop;

    /// @brief IO pins.
    enum Pin
    {
        Reset = A4,
        StartStop = A5,
    };
};

/// @brief Call once a loop to update the stopwatch display.
/// @param time The time to show.
void display_stopwatch_time(StopwatchTime &time);

/// @brief Display a digit on the 7-segment display. Keep in mind that the position is determined by the mux calls.
/// @param digit The number to display.
void display_digit(uint8_t digit);

/// @brief Turn on the right transistors to display the digit at a correct place.
/// @param digit_position Which of the four possible digit positions.
void mux(uint8_t digit_position);