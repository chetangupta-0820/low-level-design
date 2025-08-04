# Car Rental System - Class Diagram

## System Design

```mermaid
classDiagram
direction TB

class Vehicle {
    -string id
    -string model
    -Status status
    -double basePrice
    -string type
    +calculatePrice()
}

class SUVCar {
    +calculatePrice()
}

class Bike {
    +calculatePrice()
}

class Location {
    -string address
    -string city
    -string state
    -string zipCode
}

class RentalStore {
    -string name
    -Location location
    -List~Vehicle~ vehicles
    +addVehicle()
    +removeVehicle()
    +getVehicles()
}

class User {
    -string name
    -string email
    -string license
}

class Reservation {
    -Vehicle vehicle
    -User user
    -DateTime startTime
    -DateTime endTime
    -Status status
    -double totalPrice
    +calculatePrice()
    +cancelReservation()
}

class ReservationManager {
    -Map~string, List [Reservation]~ userReservations
    +createReservation()
    +getReservations()
    +updateReservation()
}

class PaymentProcessor {
    <<interface>>
    +processPayment()
}

class CreditCardProcessor {
    +processPayment()
}

class RentalSystem {
    -List~RentalStore~ stores
    -ReservationManager reservationManager
    -PaymentProcessor paymentProcessor
    +searchVehicles()
    +createReservation()
    +processPayment()
}

Vehicle <|-- SUVCar
Vehicle <|-- Bike
RentalStore --> Location
RentalStore --> Vehicle
Reservation --> Vehicle
Reservation --> User
ReservationManager --> Reservation
PaymentProcessor <|.. CreditCardProcessor
RentalSystem --> RentalStore
RentalSystem --> ReservationManager
RentalSystem --> PaymentProcessor
```

## Design Patterns Used
1. Strategy Pattern: For payment processing
2. Factory Pattern: For vehicle creation
3. Observer Pattern: For reservation status updates
4. Command Pattern: For reservation operations
5. Singleton: For ReservationManager

## Implementation Steps
1. Create Vehicle hierarchy with different types
2. Implement Location and RentalStore management
3. Build User and authentication system
4. Create Reservation system with status tracking
5. Implement ReservationManager for booking handling
6. Add payment processing with strategy pattern
7. Create main RentalSystem class
8. Implement vehicle search and filtering
9. Add pricing calculation logic
10. Implement reservation workflows
