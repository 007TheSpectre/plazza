#!/bin/bash
PASS=0
FAIL=0
BIN="${1:-./plazza}"

pass() { printf "PASS  %s\n" "$1"; PASS=$((PASS + 1)); }
fail() { printf "FAIL  %s\n" "$1"; FAIL=$((FAIL + 1)); }

check() {
    local name="$1"; shift
    if "$@"; then pass "$name"; else fail "$name"; fi
}

# ----------- argument validation -----------

args_no_args()      { "$BIN" >/dev/null 2>&1; [ $? -eq 84 ]; }
args_too_few()      { "$BIN" 1 2 >/dev/null 2>&1; [ $? -eq 84 ]; }
args_non_numeric()  { "$BIN" abc 2 100 >/dev/null 2>&1; [ $? -eq 84 ]; }
args_neg_mult()     { "$BIN" -- -1 2 100 >/dev/null 2>&1; [ $? -eq 84 ]; }
args_zero_mult()    { "$BIN" 0 2 100 >/dev/null 2>&1; [ $? -eq 84 ]; }
args_zero_cooks()   { "$BIN" 1 0 100 >/dev/null 2>&1; [ $? -eq 84 ]; }
args_valid_exits_ok() { timeout 3 "$BIN" 1 2 100 < /dev/null >/dev/null 2>&1; }

check "no arguments → exit 84"          args_no_args
check "too few arguments → exit 84"     args_too_few
check "non-numeric argument → exit 84"  args_non_numeric
check "negative multiplier → exit 84"   args_neg_mult
check "zero multiplier → exit 84"       args_zero_mult
check "zero cooks → exit 84"            args_zero_cooks
check "valid arguments → exit 0"        args_valid_exits_ok

# ----------- order dispatching -----------

order_dispatched() {
    { echo "margarita S x1"; sleep 0.5; } \
        | timeout 5 "$BIN" 0.01 2 100 2>/dev/null \
        | grep -q "Order #1"
}

order_completes() {
    { echo "margarita S x1"; sleep 0.5; } \
        | timeout 10 "$BIN" 0.01 2 100 2>/dev/null \
        | grep -q "is ready"
}

order_invalid_syntax() {
    echo "not_a_pizza" \
        | timeout 5 "$BIN" 0.01 2 100 2>&1 \
        | grep -q "Order error"
}

order_empty_lines_ignored() {
    { printf "\n\nmargarita S x1\n"; sleep 0.5; } \
        | timeout 5 "$BIN" 0.01 2 100 2>/dev/null \
        | grep -q "Order #1"
}

order_multi_pizza_x3() {
    { echo "margarita S x3"; sleep 0.5; } \
        | timeout 5 "$BIN" 0.01 2 100 2>/dev/null \
        | grep -q "Order #3"
}

order_multi_all_complete() {
    local count
    count=$({ echo "margarita S x2"; sleep 0.5; } \
        | timeout 10 "$BIN" 0.01 2 100 2>/dev/null \
        | grep -c "is ready")
    [ "$count" -eq 2 ]
}

order_mixed_types() {
    { echo "regina M x1;americana L x1"; sleep 0.5; } \
        | timeout 5 "$BIN" 0.01 2 100 2>/dev/null \
        | grep -q "Order #2"
}

check "valid order is dispatched"              order_dispatched
check "pizza reaches ready state"              order_completes
check "invalid syntax → Order error message"  order_invalid_syntax
check "empty lines are silently ignored"       order_empty_lines_ignored
check "x3 multiplier dispatches 3 pizzas"     order_multi_pizza_x3
check "x2 order produces 2 ready messages"    order_multi_all_complete
check "mixed types in one line dispatched"    order_mixed_types

# ----------- status command -----------

status_no_kitchens() {
    echo "status" \
        | timeout 5 "$BIN" 0.01 2 100 2>/dev/null \
        | grep -q "No kitchens"
}

status_active_kitchen() {
    { echo "margarita S x1"; sleep 0.2; echo "status"; } \
        | timeout 5 "$BIN" 0.01 2 100 2>/dev/null \
        | grep -q "Kitchen"
}

check "status before orders → No kitchens"  status_no_kitchens
check "status after order shows Kitchen"    status_active_kitchen

# ----------- summary -----------

printf "\n%d passed, %d failed\n" "$PASS" "$FAIL"
[ "$FAIL" -eq 0 ]
