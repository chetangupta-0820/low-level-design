# Inventory Management System - Class Diagram

```mermaid
classDiagram
    direction TB

    Product <|-- ElectronicProduct
    ProductBuilder <|-- ElectronicProductBuilder
    Product *-- ProductBuilder
    ElectronicProduct *-- ElectronicProductBuilder
    InventoryManager o-- Warehouse
    InventoryManager o-- ReplenishStrategy
    ReplenishStrategy <|.. JustInTimeReplenish
    ReplenishStrategy <|.. BulkReplenish
    InventoryManager o-- Observer
    Warehouse *-- Product

    class Product {
        <<abstract>>
        +ProductCategory category
        +int quantity
        +double price
        +int threshold
        +string sku
        +~Product()
    }

    class ElectronicProduct {
        -string brand
        -int warranty
    }

    class ProductBuilder {
        <<abstract>>
        #shared_ptr~Product~ product
        +setQuantity(int) Builder&
        +setCategory(ProductCategory) Builder&
        +setPrice(double) Builder&
        +build()* shared_ptr~Product~
    }

    class ElectronicProductBuilder {
        -shared_ptr~ElectronicProduct~ eproduct
        +setBrand(string) Builder&
        +setWarranty(int) Builder&
        +build() shared_ptr~Product~
    }

    class Warehouse {
        +int id
        +string name
        +string location
        +map~string,Product~ products
        +addProduct(Product, int)
        +removeProduct(Product, int)
        +getAvailableQuantity(string) int
    }

    class ReplenishStrategy {
        <<interface>>
        +replenish(Product)* void
    }

    class JustInTimeReplenish {
        +replenish(Product) void
    }

    class BulkReplenish {
        +replenish(Product) void
    }

    class Observer {
        +string name
        +update() void
    }

    class InventoryManager {
        <<singleton>>
        -vector~Warehouse~ warehouses
        -ReplenishStrategy replenishStrategy
        -vector~Observer~ observers
        -InventoryManager(ReplenishStrategy)
        +getInstance(ReplenishStrategy)
        +getProduct(string) Product
        +checkAndReplenish(string)
        +addObserver(Observer)
        +removeObserver(string)
        +notifyObservers()
    }
```

## Design Patterns Used

1. **Builder Pattern**
   - Used for constructing Product objects
   - Separate builders for different product types
   - Allows step-by-step construction with method chaining

2. **Strategy Pattern**
   - Used for different replenishment strategies
   - Easily swap between JustInTime and Bulk replenishment

3. **Observer Pattern**
   - Used to notify stakeholders about inventory changes
   - Observers can subscribe/unsubscribe to notifications

4. **Singleton Pattern**
   - InventoryManager is a singleton
   - Ensures single point of control for inventory

## Implementation Steps

1. **Product System**
   - `enum class ProductCategory`
   - `class Product` (abstract)
     - Base attributes (sku, quantity, price)
   - `class ElectronicProduct : public Product`
     - Additional attributes (brand, warranty)

2. **Builder Pattern**
   - `class Product::Builder` (abstract)
     - Common builder methods
   - `class ElectronicProduct::Builder`
     - Product-specific builder methods
     - Build implementation

3. **Warehouse System**
   - `class Warehouse`
     - `addProduct(Product*, int)`
     - `removeProduct(Product*, int)`
     - `getAvailableQuantity(string)`

4. **Replenishment Strategy**
   - `class ReplenishStrategy` (interface)
     - `virtual void replenish(Product*)`
   - `class JustInTimeReplenish`
   - `class BulkReplenish`

5. **Observer System**
   - `class Observer`
     - `update()`
   - Observer registration in InventoryManager
   - Notification mechanism

6. **Inventory Management**
   - `class InventoryManager` (singleton)
     - `getInstance()`
     - `checkAndReplenish()`
     - Observer management
     - Warehouse management
