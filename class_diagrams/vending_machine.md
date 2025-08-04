# Vending Machine - Class Diagram

## System Design

```mermaid
classDiagram
direction TB

class VendingMachine {
    -State currentState
    -Inventory inventory
    -PaymentProcessor payment
    -double currentAmount
    +insertMoney()
    +selectProduct()
    +dispenseProduct()
    +returnChange()
}

class State {
    <<interface>>
    +insertMoney()
    +selectProduct()
    +dispenseProduct()
    +returnChange()
}

class IdleState {
    +insertMoney()
    +selectProduct()
    +dispenseProduct()
    +returnChange()
}

class HasMoneyState {
    +insertMoney()
    +selectProduct()
    +dispenseProduct()
    +returnChange()
}

class DispensingState {
    +insertMoney()
    +selectProduct()
    +dispenseProduct()
    +returnChange()
}

class Product {
    -string id
    -string name
    -double price
    -int quantity
}

class Inventory {
    -Map~string,Product~ products
    +addProduct()
    +removeProduct()
    +updateQuantity()
    +getProduct()
}

class PaymentProcessor {
    -double balance
    +acceptMoney()
    +returnChange()
    +processPayment()
}

VendingMachine --> State
State <|.. IdleState
State <|.. HasMoneyState
State <|.. DispensingState
VendingMachine --> Inventory
VendingMachine --> PaymentProcessor
Inventory --> Product
```

## Design Patterns Used
1. State Pattern: For vending machine states
2. Factory Pattern: For product creation
3. Singleton: For vending machine
4. Strategy Pattern: For payment processing

## Implementation Steps
1. Create Product and Inventory classes
2. Implement State interface
3. Create concrete states
4. Build PaymentProcessor
5. Implement VendingMachine controller
6. Add money handling
7. Implement product selection
8. Create dispensing logic
9. Add change calculation
10. Implement error handling
