#include "new_booking_providers.h"

#include <vector>

using namespace std;

class Trip {
public:
  vector<HotelProvider::Booking> hotels;
  vector<FlightProvider::Booking> flights;
  // vector<RAII::Booking<HotelProvider>>
  // vector<RAII::Booking<FlightProvider>>

  Trip() = default;
  Trip(const Trip&) = delete;
  Trip(Trip&&) = default;

  Trip& operator=(const Trip&) = delete;
  Trip& operator=(Trip&&) = default;

  void Cancel() {
    hotels.clear();
    flights.clear();
  }
};


class TripManager {
public:
  struct BookingData {
    string city_from;
    string city_to;
    string date_from;
    string date_to;
  };

  Trip Book(const BookingData& data) {
    Trip trip;
    {
      FlightProvider::BookingData data;
      trip.flights.push_back(flight_provider.Book(move(data)));
    }
    {
      HotelProvider::BookingData data;
      trip.hotels.push_back(hotel_provider.Book(move(data)));
    }
    {
      FlightProvider::BookingData data;
      trip.flights.push_back(flight_provider.Book(move(data)));
    }
    return trip;
  }

  void Cancel(Trip& trip) {
    trip.Cancel();
  }

private:
  HotelProvider hotel_provider;
  FlightProvider flight_provider;
};
