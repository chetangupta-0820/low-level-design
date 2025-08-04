# Movie Booking System - Class Diagram

## System Design

```mermaid
classDiagram
direction TB

class Movie {
    -string name
    -int duration
}

class Seat {
    -int row
    -int number
}

class Screen {
    -int screenNumber
    -List~Seat~ seats
}

class Theatre {
    -string name
    -string location
    -List~Screen~ screens
}

class Show {
    -Movie movie
    -Screen screen
    -DateTime startTime
    -double price
}

class User {
    -string name
    -string email
}

class Booking {
    -Show show
    -List~Seat~ seats
    -User user
    -Status status
    -double amount
    +makePayment()
}

class SeatLock {
    -Seat seat
    -Show show
    -int timeoutInSeconds
    -DateTime lockTime
    -string lockedBy
    +isLockExpired()
}

class iSeatLockProvider {
    <<interface>>
    +lockSeats()
    +unlockSeats()
    +validateLock()
}

class SeatLockProvider {
    -Map~Show,Map~Seat,Lock~~ locks
    +lockSeats()
    +unlockSeats()
    +validateLock()
}

class SeatAvailabilityService {
    -iSeatLockProvider lockProvider
    +getAvailableSeats()
}

class BookingService {
    -SeatLockProvider lockProvider
    -PaymentStrategy paymentStrategy
    +createBooking()
    +confirmBooking()
}

class PaymentStrategy {
    <<interface>>
    +processPayment()
}

class CreditCard {
    +processPayment()
}

Theatre --* Screen
Screen --* Seat
Show --* Movie
Show --* Screen
Booking --* Show
Booking --* Seat
Booking --* User
SeatLockProvider ..|> iSeatLockProvider
CreditCard ..|> PaymentStrategy
BookingService --> PaymentStrategy
SeatAvailabilityService --> iSeatLockProvider
```

## Design Patterns Used
1. Strategy Pattern: For payment processing (PaymentStrategy interface)
2. Singleton: Could be used for BookingService and SeatLockProvider
3. Observer: Could be used to notify users about show updates
4. Factory: Could be used for creating different types of seats/shows

## Implementation Steps
1. Create base entities (Movie, Seat, Screen, Theatre)
2. Implement Show management with schedules
3. Create User and Booking classes with states
4. Implement SeatLockProvider for concurrent booking handling
5. Add SeatAvailabilityService for real-time seat status
6. Create BookingService with payment integration
7. Implement payment processing strategy
8. Add concurrency control for seat booking
9. Implement booking timeout and auto-cancellation
10. Add validation and error handling
