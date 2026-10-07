# Plazza — Architecture

## Process Architecture

```mermaid
graph TD
    main["main.cpp<br/>arg parsing"] --> Reception

    subgraph "Main Process"
        Reception --> Parser
        Reception --> |"manages N"| KP1["KitchenProxy 1"]
        Reception --> |"manages N"| KP2["KitchenProxy 2"]
        Reception --> KPN["KitchenProxy …"]
        Reception --> Poller["resultPollerLoop\n(background thread)"]
    end

    KP1 -. "fork()" .-> K1
    KP2 -. "fork()" .-> K2

    subgraph "Child Process 1"
        K1["Kitchen 1"] --> TP1["ThreadPool\n(N cook threads)"]
        K1 --> S1["Stock\n(9 ingredients)"]
        TP1 --> Cook1["Cook Thread"]
        TP1 --> CookN["Cook Thread …"]
    end

    subgraph "Child Process 2"
        K2["Kitchen 2"] --> TP2["ThreadPool"]
        K2 --> S2["Stock"]
    end

    KP1 <-->|"FIFO order/result"| K1
    KP2 <-->|"FIFO order/result"| K2
```

---

## Order Lifecycle

```mermaid
sequenceDiagram
    actor User
    participant Reception
    participant Parser
    participant KitchenProxy
    participant Kitchen
    participant ThreadPool
    participant Stock

    User->>Reception: "margarita S x2; regina M x1"
    Reception->>Parser: parse(line)
    Parser-->>Reception: [(Margarita,S),(Margarita,S),(Regina,M)]

    loop for each pizza
        Reception->>KitchenProxy: send(PizzaPacket{type,size,orderId})
        KitchenProxy->>Kitchen: write to order FIFO
        Kitchen->>ThreadPool: submit(cookPizza)
        ThreadPool->>Stock: consume(ingredients)
        Stock-->>ThreadPool: ok
        Note over ThreadPool: sleep(bakeTime × multiplier)
        ThreadPool->>Kitchen: write result to result FIFO
        Kitchen-->>KitchenProxy: result FIFO readable
        KitchenProxy-->>Reception: pollResult() → PizzaPacket
        Reception-->>User: "Order #N (Margarita S) is ready!"
    end
```

---

## Class Hierarchy

```mermaid
classDiagram
    class IPizza {
        <<interface>>
        +type() PizzaType
        +size() PizzaSize
        +bakeTime() double
        +ingredients() vector~Ingredient~
        +name() string
    }
    class IIPC {
        <<interface>>
        +send(data)
        +receive() vector~uint8~
    }
    class IKitchen {
        <<interface>>
        +status()
    }

    IPizza <|-- Margarita
    IPizza <|-- Regina
    IPizza <|-- Americana
    IPizza <|-- Fantasia
    IIPC   <|-- NamedPipe
    IKitchen <|-- Kitchen

    class Reception {
        +run()
        -leastBusyKitchen()
        -spawnKitchen()
        -resultPollerLoop()
    }
    class KitchenProxy {
        -pid pid_t
        +send(PizzaPacket)
        +pollResult(PizzaPacket) bool
        +pendingCount() size_t
    }
    class Kitchen {
        -pool ThreadPool
        -stock Stock
        +run()
    }
    class ThreadPool {
        -tasks SafeQueue
        +submit(task)
        +pendingCount() size_t
        +isSaturated(max) bool
    }
    class SafeQueue~T~ {
        +push(value)
        +pop() T
        +close()
    }
    class Stock {
        -items map~Ingredient,int~
        +consume(ingredients) bool
        +status()
    }
    class Parser {
        +parse(line)$ Order
    }
    class PizzaFactory {
        +create(type, size)$ unique_ptr~IPizza~
    }

    Reception "1" *-- "*" KitchenProxy
    KitchenProxy *-- "2" NamedPipe
    Kitchen *-- "1" ThreadPool
    Kitchen *-- "1" Stock
    ThreadPool *-- "1" SafeQueue
    Reception ..> Parser : uses
    Kitchen ..> PizzaFactory : uses
```

---

## Kitchen Lifecycle

```mermaid
stateDiagram-v2
    [*]      --> Idle    : fork() + Kitchen::run() starts
    Idle     --> Active  : order received via FIFO
    Active   --> Active  : more orders / pendingCount > 0
    Active   --> Idle    : pendingCount == 0
    Idle     --> [*]     : 5 s idle timeout → exit(0)
```

---

## CI/CD Pipeline

```mermaid
graph LR
    Build["Build\nuploads ./plazza artifact"]
    Tests["Tests & Coverage\ngcovr report"]
    Functional["Functional Tests\n16 bash end-to-end tests"]
    Valgrind["Valgrind\nleak check on unit tests"]

    Build --> Tests
    Build --> Functional
    Tests --> Valgrind
    Functional --> Valgrind
```
