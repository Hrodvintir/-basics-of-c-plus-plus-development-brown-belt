#pragma once

// Ð—Ð´ÐµÑÑŒ Ð±ÑƒÐ´ÐµÑ‚ Ð¿Ð¾Ð´ÐºÐ»ÑŽÑ‡Ñ‘Ð½ Ð²Ð°Ñˆ Ñ„Ð°Ð¹Ð» Ñ Ð¾Ð¿Ñ€ÐµÐ´ÐµÐ»ÐµÐ½Ð¸ÐµÐ¼ ÑˆÐ°Ð±Ð»Ð¾Ð½Ð½Ð¾Ð³Ð¾ ÐºÐ»Ð°ÑÑÐ° Booking Ð² Ð¿Ñ€Ð¾ÑÑ‚Ñ€Ð°Ð½ÑÑ‚Ð²Ðµ Ð¸Ð¼Ñ‘Ð½ RAII
#include "booking.h"

#include <stdexcept>
#include <string>

using namespace std;

class FlightProvider {
public:
  using BookingId = int;
  using Booking = RAII::Booking<FlightProvider>;
  friend Booking;  // Ð¯Ð²Ð½Ð¾ Ñ€Ð°Ð·Ñ€ÐµÑˆÐ°ÐµÐ¼ Ñ„ÑƒÐ½ÐºÑ†Ð¸ÑÐ¼ ÐºÐ»Ð°ÑÑÐ° Booking Ð²Ñ‹Ð·Ñ‹Ð²Ð°Ñ‚ÑŒ private-Ñ„ÑƒÐ½ÐºÑ†Ð¸Ð¸ Ð½Ð°ÑˆÐµÐ³Ð¾ ÐºÐ»Ð°ÑÑÐ° FlightProvider

  struct BookingData {
    string city_from;
    string city_to;
    string date;
  };

  Booking Book(const BookingData& data) {
    if (counter >= capacity) {
      throw runtime_error("Flight overbooking");
    }
    ++counter;
    return {this, counter};
  }

private:
  // Ð¡ÐºÑ€Ñ‹Ð²Ð°ÐµÐ¼ ÑÑ‚Ñƒ Ñ„ÑƒÐ½ÐºÑ†Ð¸ÑŽ Ð² private, Ñ‡Ñ‚Ð¾Ð±Ñ‹ ÐµÑ‘ Ð¼Ð¾Ð³ Ð¿Ð¾Ð·Ð²Ð°Ñ‚ÑŒ Ñ‚Ð¾Ð»ÑŒÐºÐ¾ ÑÐ¾Ð¾Ñ‚Ð²ÐµÑ‚ÑÑ‚Ð²ÑƒÑŽÑ‰Ð¸Ð¹ friend-ÐºÐ»Ð°ÑÑ Booking
  void CancelOrComplete(const Booking& booking) {
    --counter;
  }

public:
  static int capacity;
  static int counter;
};


class HotelProvider {
public:
  using BookingId = int;
  using Booking = RAII::Booking<HotelProvider>;
  friend Booking;

  struct BookingData {
    string city;
    string date_from;
    string date_to;
  };

  Booking Book(const BookingData& data) {
    if (counter >= capacity) {
      throw runtime_error("Hotel overbooking");
    }
    ++counter;
    return {this, counter};
  }

private:
  void CancelOrComplete(const Booking& booking) {
    --counter;
  }

public:
  static int capacity;
  static int counter;
};
