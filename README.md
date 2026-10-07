> **Epitech · `G-CCP-400` ("theplazza")** — team project built with
> [pnyxxx](https://github.com/pnyxxx).
>
> **My role:** lead developer.
>
> This repository is my own copy of the assignment, published as a portfolio
> piece. The original repository is private.

---

# Plazza

Epitech G-CCP-400 — Multi-process pizzeria simulation in C++20.

**[Documentation & Architecture Diagrams](https://epitechpge2-2025.github.io/G-CCP-400-LIL-4-1-theplazza-21/)**

## Usage

```
./plazza <timeMultiplier> <numCooks> <restockMs>
```

| Argument | Description |
|---|---|
| `timeMultiplier` | Scales all bake times (0.01 = 100x faster) |
| `numCooks` | Cook threads per kitchen |
| `restockMs` | Ingredient restock interval in milliseconds |

**Example:**

```
./plazza 2 5 100
> margarita S x2 ; regina M x1
> status
```

## Build

```bash
make              # build → ./plazza
make tests_run    # unit tests + coverage
make re           # full rebuild
```

## Architecture

```
Reception (main process)
├── Parser        — validates order grammar
└── KitchenProxy  — per-kitchen handle (pid + pipes)

Kitchen (child process, forked on demand)
├── ThreadPool    — N cook threads pulling from SafeQueue
└── Stock         — 9 ingredients, mutex-protected, timer replenishment
```

Kitchens are forked on demand when all existing kitchens are saturated and exit automatically after 5 seconds of idle.
