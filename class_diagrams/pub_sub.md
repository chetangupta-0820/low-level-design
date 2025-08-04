# Pub/Sub System - Class Diagram

## System Design

```mermaid
classDiagram
direction TB

class Message {
    -string id
    -string content
    -DateTime timestamp
    -Map metadata
}

class Topic {
    -string name
    -List~Message~ messages
    -List~TopicSubscriber~ subscribers
    +addMessage()
    +addSubscriber()
    +removeSubscriber()
}

class Subscriber {
    <<interface>>
    +consume(Message)
}

class DefaultSubscriber {
    -string id
    +consume(Message)
}

class Publisher {
    <<interface>>
    +publish(Topic, Message)
}

class DefaultPublisher {
    -string id
    +publish(Topic, Message)
}

class TopicSubscriber {
    -Topic topic
    -Subscriber subscriber
    -int offset
    +processMessage()
}

class KafkaController {
    -Map~string,Topic~ topics
    -List~Publisher~ publishers
    +createTopic()
    +deleteTopic()
    +registerPublisher()
    +registerSubscriber()
}

Topic --> Message
Topic --> TopicSubscriber
TopicSubscriber --> Subscriber
Subscriber <|.. DefaultSubscriber
Publisher <|.. DefaultPublisher
KafkaController --> Topic
KafkaController --> Publisher
```

## Design Patterns Used
1. Observer Pattern: For pub/sub mechanism
2. Factory Pattern: For creating topics and messages
3. Strategy Pattern: For message processing
4. Singleton: For KafkaController

## Implementation Steps
1. Create Message and Topic entities
2. Implement Publisher interface
3. Create Subscriber interface
4. Build TopicSubscriber for management
5. Implement KafkaController
6. Add message queuing
7. Implement offset tracking
8. Add error handling
9. Create delivery guarantees
10. Implement scaling mechanisms
