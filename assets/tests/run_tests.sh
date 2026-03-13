#!/bin/bash
# ============================================================
# miniRT Automated Parser Tester
# Tests parsing validation by running miniRT on various scenes
# that should either FAIL or PASS parsing.
# ============================================================

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
RESET='\033[0m'

MINIRT="./build/bin/miniRT"
FAIL_DIR="assets/tests/should_fail"
PASS_DIR="assets/tests/should_pass"

pass_count=0
fail_count=0
total=0

if [ ! -f "$MINIRT" ]; then
    echo -e "${RED}Error: $MINIRT not found. Run 'make' first.${RESET}"
    exit 1
fi

echo ""
echo -e "${BOLD}${CYAN}╔══════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}${CYAN}║        miniRT PARSING VALIDATION TESTER      ║${RESET}"
echo -e "${BOLD}${CYAN}╚══════════════════════════════════════════════╝${RESET}"
echo ""

# -----------------------------------------------
# Test cases that SHOULD FAIL (return non-zero)
# -----------------------------------------------
echo -e "${BOLD}${YELLOW}━━━ SECTION 1: Tests that SHOULD FAIL (error on invalid input) ━━━${RESET}"
echo ""

if [ -d "$FAIL_DIR" ]; then
    for file in "$FAIL_DIR"/*; do
        total=$((total + 1))
        name=$(basename "$file")
        # Run with a timeout - parsing shouldn't take long
        timeout 3 "$MINIRT" "$file" > /dev/null 2>&1
        exit_code=$?

        if [ $exit_code -ne 0 ]; then
            echo -e "  ${GREEN}✓ PASS${RESET}  $name  ${CYAN}(correctly rejected)${RESET}"
            pass_count=$((pass_count + 1))
        else
            echo -e "  ${RED}✗ FAIL${RESET}  $name  ${RED}(should have been rejected but was accepted!)${RESET}"
            fail_count=$((fail_count + 1))
        fi
    done
else
    echo -e "  ${YELLOW}⚠ Directory $FAIL_DIR not found${RESET}"
fi

echo ""

# -----------------------------------------------
# Test no arguments
# -----------------------------------------------
echo -e "${BOLD}${YELLOW}━━━ SECTION 2: Special argument tests ━━━${RESET}"
echo ""

total=$((total + 1))
timeout 3 "$MINIRT" > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo -e "  ${GREEN}✓ PASS${RESET}  no_arguments  ${CYAN}(correctly rejected)${RESET}"
    pass_count=$((pass_count + 1))
else
    echo -e "  ${RED}✗ FAIL${RESET}  no_arguments  ${RED}(should have been rejected)${RESET}"
    fail_count=$((fail_count + 1))
fi

total=$((total + 1))
timeout 3 "$MINIRT" "nonexistent_file.rt" > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo -e "  ${GREEN}✓ PASS${RESET}  nonexistent_file  ${CYAN}(correctly rejected)${RESET}"
    pass_count=$((pass_count + 1))
else
    echo -e "  ${RED}✗ FAIL${RESET}  nonexistent_file  ${RED}(should have been rejected)${RESET}"
    fail_count=$((fail_count + 1))
fi

total=$((total + 1))
timeout 3 "$MINIRT" "file1.rt" "file2.rt" > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo -e "  ${GREEN}✓ PASS${RESET}  too_many_arguments  ${CYAN}(correctly rejected)${RESET}"
    pass_count=$((pass_count + 1))
else
    echo -e "  ${RED}✗ FAIL${RESET}  too_many_arguments  ${RED}(should have been rejected)${RESET}"
    fail_count=$((fail_count + 1))
fi

echo ""

# ---------------------------------------------------------------------------------
# Test cases that SHOULD PASS parsing
# (will be killed after parsing succeeds since we can't interact with GUI)
# We check that exit code is 0 OR the process was killed (137/143 from timeout)
# ---------------------------------------------------------------------------------
echo -e "${BOLD}${YELLOW}━━━ SECTION 3: Tests that SHOULD PASS parsing ━━━${RESET}"
echo -e "  ${CYAN}(These test parsing only; window is killed after 30s)${RESET}"
echo ""

if [ -d "$PASS_DIR" ]; then
    for file in "$PASS_DIR"/*.rt; do
        total=$((total + 1))
        name=$(basename "$file")
        # Give it 30 seconds - if it gets past parsing it will try to render/open window
        timeout 5 "$MINIRT" "$file" > /dev/null 2>&1
        exit_code=$?

        # exit code 0 = clean exit, 124 = timeout killed it (means it got past parsing)
        # 137/143 = signal killed
        if [ $exit_code -eq 0 ] || [ $exit_code -eq 124 ] || [ $exit_code -eq 137 ] || [ $exit_code -eq 143 ]; then
            echo -e "  ${GREEN}✓ PASS${RESET}  $name  ${CYAN}(parsing accepted)${RESET}"
            pass_count=$((pass_count + 1))
        else
            echo -e "  ${RED}✗ FAIL${RESET}  $name  ${RED}(parsing rejected, exit=$exit_code)${RESET}"
            fail_count=$((fail_count + 1))
        fi
    done
else
    echo -e "  ${YELLOW}⚠ Directory $PASS_DIR not found${RESET}"
fi

echo ""

# -----------------------------------------------
# Summary
# -----------------------------------------------
echo -e "${BOLD}${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${RESET}"
echo -e "${BOLD}  RESULTS: $pass_count/$total passed${RESET}"
if [ $fail_count -eq 0 ]; then
    echo -e "  ${GREEN}${BOLD}ALL TESTS PASSED!${RESET}"
else
    echo -e "  ${RED}${BOLD}$fail_count TEST(S) FAILED${RESET}"
fi
echo -e "${BOLD}${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${RESET}"
echo ""

exit $fail_count
