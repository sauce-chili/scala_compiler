#!/bin/bash

# Bash script to run semantic tests
# Usage: ./run_tests.sh

COMPILER="../../../cmake-build-debug/scala_lexer"
TESTS_DIR="$(dirname "$0")"

echo "=== Running Semantic Tests ==="
echo ""

# Valid tests (should have no errors)
echo "--- Valid Tests ---"

for file in "$TESTS_DIR/valid/"*.scala; do
    if [ -f "$file" ]; then
        name=$(basename "$file")
        echo -n "Testing $name... "

        output=$("$COMPILER" "$file" 2>&1)

        if echo "$output" | grep -q "Error"; then
            echo "FAILED (unexpected errors)"
            echo "$output"
        else
            echo "PASSED"
        fi
    fi
done

echo ""

# Error tests (should have expected errors)
echo "--- Error Tests ---"

for file in "$TESTS_DIR/errors/"*.scala; do
    if [ -f "$file" ]; then
        name=$(basename "$file" .scala)
        expected="$TESTS_DIR/expected/$name.txt"

        echo -n "Testing $name... "

        output=$("$COMPILER" "$file" 2>&1)

        if [ -f "$expected" ]; then
            if echo "$output" | grep -q "Error"; then
                echo "PASSED (errors detected)"
            else
                echo "FAILED (expected errors but none found)"
            fi
        else
            if echo "$output" | grep -q "Error"; then
                echo "OK (errors detected, no expected file)"
                echo "  Output: $output"
            else
                echo "FAILED (expected errors)"
            fi
        fi
    fi
done

echo ""
echo "=== Tests Complete ==="
