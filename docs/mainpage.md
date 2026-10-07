# Plazza {#mainpage}

**Epitech G-CCP-400** — Multi-process pizzeria simulation in C++20.

## What it does

`plazza` simulates a pizzeria: a reception process takes orders on stdin, distributes
individual pizzas across one or more kitchen child processes, and prints each completion
to stdout. Kitchens are forked on demand and exit automatically after 5 seconds of idle.

```
./plazza <timeMultiplier> <numCooks> <restockMs>
```

| Argument | Description |
|---|---|
| `timeMultiplier` | Scales all bake times (0.01 = 100x faster) |
| `numCooks` | Cook threads per kitchen |
| `restockMs` | Ingredient restock interval in milliseconds |

## Build

```bash
make              # build → ./plazza
make tests_run    # run unit tests + coverage report
make functional_tests  # run end-to-end bash tests
make docs         # generate this documentation
```

## Module Overview

| Module | Description |
|---|---|
| `reception/` | Shell, load balancer, kitchen lifecycle |
| `kitchen/` | Child-process cook loop, thread pool, ingredient stock |
| `ipc/` | POSIX named pipes with length-prefixed framing |
| `pizza/` | Pizza type hierarchy and factory |
| `concurrency/` | IMutex, IThread, ScopedLock, SafeQueue abstractions |

## Architecture Diagrams

See the [Architecture page](../architecture.html) for process graph, order lifecycle,
class hierarchy, IPC wire format, and CI/CD pipeline diagrams.
