// Test 7: Numeric, character, and string literals with escape sequences
final class A {
    def main(args: Array[String]): Unit = {
        new BranchingTest().branchingTest()
    }
}

final class BranchingTest {

    def classify(n: Int): Unit = {
        if (n > 0) {
            Predef.println("positive")
        } else if (n < 0) {
            Predef.println("negative")
        } else {
            Predef.println("zero")
        }
    }

    def abs(x: Int): Int = {
        if (x < 0) {
            return -x
        }
        x
    }

    def max(a: Int, b: Int): Int = {
        if (a > b) a else b
    }

    def min(a: Int, b: Int): Int = {
        if (a < b) a else b
    }

    def branchingTest(): Unit = {
        Predef.println("=== Branching Test ===")

        // simple if
        val x: Int = 10
        if (x > 5) {
            Predef.println("x > 5")
        }

        // if-else
        if (x > 20) {
            Predef.println("x > 20")
        } else {
            Predef.println("x <= 20")
        }

        // nested if-else if-else
        Predef.println("--- classify ---")
        classify(5)
        classify(-3)
        classify(0)

        // if as expression
        val sign: Int = if (x > 0) 1 else -1
        Predef.print("sign of 10 = ")
        Predef.println(sign)

        // abs
        Predef.print("abs(-7) = ")
        Predef.println(abs(-7))
        Predef.print("abs(3) = ")
        Predef.println(abs(3))

        // max, min
        Predef.print("max(3, 7) = ")
        Predef.println(max(3, 7))
        Predef.print("min(3, 7) = ")
        Predef.println(min(3, 7))

        // chained conditions
        val a: Int = 15
        if (a > 10 && a < 20) {
            Predef.println("a is between 10 and 20")
        }
        if (a == 15 || a == 20) {
            Predef.println("a is 15 or 20")
        }
    }
}
