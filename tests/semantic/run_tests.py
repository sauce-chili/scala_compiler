#!/usr/bin/env python3
"""
Скрипт для запуска семантических тестов.
Сравнивает фактический вывод компилятора с ожидаемым.
"""

import os
import sys
import subprocess
import shutil
from pathlib import Path
from typing import List, Tuple

TESTS_DIR = Path(__file__).parent
PROJECT_ROOT = TESTS_DIR.parent.parent
COMPILER = PROJECT_ROOT / "cmake-build-debug" / "scala_lexer.exe"

def find_test_dirs() -> List[Path]:
    """Находит все директории с тестами (содержащие input.scala)."""
    tests = []
    for item in sorted(TESTS_DIR.iterdir()):
        if item.is_dir() and (item / "input.scala").exists():
            tests.append(item)
    return tests

def run_compiler(input_file: Path, work_dir: Path) -> bool:
    """Запускает компилятор на входном файле."""
    try:
        result = subprocess.run(
            [str(COMPILER), str(input_file)],
            cwd=str(work_dir),
            capture_output=True,
            text=True,
            timeout=30
        )
        return result.returncode == 0
    except Exception as e:
        print(f"  ERROR: {e}")
        return False

def compare_files(expected: Path, actual: Path) -> Tuple[bool, str]:
    """Сравнивает два CSV файла."""
    if not actual.exists():
        return False, f"Missing: {actual.name}"

    with open(expected, 'r', encoding='utf-8') as f:
        expected_lines = [line.strip() for line in f.readlines() if line.strip()]

    with open(actual, 'r', encoding='utf-8') as f:
        actual_lines = [line.strip() for line in f.readlines() if line.strip()]

    if expected_lines != actual_lines:
        diff = []
        max_lines = max(len(expected_lines), len(actual_lines))
        for i in range(max_lines):
            exp = expected_lines[i] if i < len(expected_lines) else "<missing>"
            act = actual_lines[i] if i < len(actual_lines) else "<missing>"
            if exp != act:
                diff.append(f"  Line {i+1}:")
                diff.append(f"    Expected: {exp}")
                diff.append(f"    Actual:   {act}")
        return False, "\n".join(diff)

    return True, "OK"

def compare_dirs(expected_dir: Path, actual_dir: Path) -> List[Tuple[str, bool, str]]:
    """Рекурсивно сравнивает директории."""
    results = []

    for exp_file in expected_dir.rglob("*.csv"):
        rel_path = exp_file.relative_to(expected_dir)
        act_file = actual_dir / rel_path

        success, message = compare_files(exp_file, act_file)
        results.append((str(rel_path), success, message))

    return results

def run_test(test_dir: Path) -> bool:
    """Запускает один тест."""
    test_name = test_dir.name
    print(f"\n{'='*60}")
    print(f"TEST: {test_name}")
    print(f"{'='*60}")

    input_file = test_dir / "input.scala"
    expected_dir = test_dir / "expected"

    # Создаём временную директорию для вывода
    work_dir = test_dir / "actual"
    if work_dir.exists():
        shutil.rmtree(work_dir)
    work_dir.mkdir()

    # Копируем входной файл в рабочую директорию
    shutil.copy(input_file, work_dir / "input.scala")

    # Запускаем компилятор
    print(f"  Running compiler...")
    if not run_compiler(work_dir / "input.scala", work_dir):
        print(f"  FAILED: Compiler error")
        return False

    actual_export = work_dir / "out_export"
    if not actual_export.exists():
        print(f"  FAILED: No out_export directory created")
        return False

    # Сравниваем результаты
    print(f"  Comparing outputs...")
    all_passed = True

    for rel_path, success, message in compare_dirs(expected_dir, actual_export):
        status = "PASS" if success else "FAIL"
        print(f"    [{status}] {rel_path}")
        if not success:
            print(message)
            all_passed = False

    return all_passed

def main():
    if not COMPILER.exists():
        print(f"ERROR: Compiler not found at {COMPILER}")
        print("Please build the project first.")
        sys.exit(1)

    tests = find_test_dirs()
    print(f"Found {len(tests)} tests")

    passed = 0
    failed = 0

    for test_dir in tests:
        if run_test(test_dir):
            passed += 1
        else:
            failed += 1

    print(f"\n{'='*60}")
    print(f"SUMMARY: {passed} passed, {failed} failed")
    print(f"{'='*60}")

    sys.exit(0 if failed == 0 else 1)

if __name__ == "__main__":
    main()
