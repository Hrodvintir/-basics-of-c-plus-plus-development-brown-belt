//#include "old_trip_manager.h"  // ÑÐ¾ ÑÑ‚Ð°Ñ€Ñ‹Ð¼Ð¸ ÐºÐ»Ð°ÑÑÐ°Ð¼Ð¸ Ð²ÑÐµ Ñ‚ÐµÑÑ‚Ñ‹ Ð¿Ñ€Ð¾Ñ…Ð¾Ð´ÑÑ‚
#include "new_trip_manager.h"

#include "test_runner.h"

#include <stdexcept>

using namespace std;


// Ð­Ñ‚Ð¸ Ð¾Ð¿Ñ€ÐµÐ´ÐµÐ»ÐµÐ½Ð¸Ñ ÑÑ‚Ð°Ñ‚Ð¸Ñ‡ÐµÑÐºÐ¸Ñ… Ð¿ÐµÑ€ÐµÐ¼ÐµÐ½Ð½Ñ‹Ñ… Ð¿Ñ€Ð°Ð²Ð¸Ð»ÑŒÐ½ÐµÐµ Ð±Ñ‹Ð»Ð¾ Ð±Ñ‹ Ð¿Ð¾Ð¼ÐµÑÑ‚Ð¸Ñ‚ÑŒ Ð² ÑÐ¾Ð¾Ñ‚Ð²ÐµÑ‚ÑÑ‚Ð²ÑƒÑŽÑ‰Ð¸Ð¹ cpp-Ñ„Ð°Ð¹Ð»,
// Ð½Ð¾ Ð¼Ñ‹ Ð´Ð»Ñ Ð¿Ñ€Ð¾ÑÑ‚Ð¾Ñ‚Ñ‹ Ñ€Ð°Ð·Ð¼ÐµÑÑ‚Ð¸Ð¼ Ð¸Ñ… Ð·Ð´ÐµÑÑŒ

int FlightProvider::capacity = 0;
int FlightProvider::counter = 0;

int HotelProvider::capacity = 0;
int HotelProvider::counter = 0;


void TestNoOverbooking() {
  FlightProvider::capacity = 100;
  HotelProvider::capacity = 100;
  FlightProvider::counter = 0;
  HotelProvider::counter = 0;
  {
    TripManager tm;
    auto trip = tm.Book({});
  }
  ASSERT_EQUAL(FlightProvider::counter, 0);
  ASSERT_EQUAL(HotelProvider::counter, 0);
}

void TestFlightOverbooking() {
  FlightProvider::capacity = 1;
  HotelProvider::capacity = 100;
  FlightProvider::counter = 0;
  HotelProvider::counter = 0;
  try {
    TripManager tm;
    auto trip = tm.Book({});
  } catch (const runtime_error&) {
    ASSERT_EQUAL(FlightProvider::counter, 0);
    ASSERT_EQUAL(HotelProvider::counter, 0);
    return;
  }
  Assert(false, "Flight overbooking was expected");
}

void TestHotelOverbooking() {
  FlightProvider::capacity = 100;
  HotelProvider::capacity = 0;
  FlightProvider::counter = 0;
  HotelProvider::counter = 0;
  try {
    TripManager tm;
    auto trip = tm.Book({});
  } catch (const runtime_error& ex) {
    ASSERT_EQUAL(FlightProvider::counter, 0);
    ASSERT_EQUAL(HotelProvider::counter, 0);
    return;
  }
  Assert(false, "Hotel overbooking was expected");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestNoOverbooking);
  RUN_TEST(tr, TestFlightOverbooking);
  RUN_TEST(tr, TestHotelOverbooking);
  return 0;
}
