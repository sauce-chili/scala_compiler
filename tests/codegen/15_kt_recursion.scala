// Test 15: Recursion(2).kt translation - Fibonacci

final class A {
    def main(args: Array[String]): Unit = {
        new RecursionTest().mains(args)
    }
}

final class RecursionTest {

    def fibonacci(n: Int): Int = {
        if (n == 0) { return 1 }
        if (n == 1) { return 1 }
        return fibonacci(n - 1) + fibonacci(n - 2)
    }

    def mains(args: Array[String]): Unit = {
        Predef.println("=== Fibonacci Test ===")

        var n: Int = 0
        while (n <= 10) {
            Predef.print("fibonacci(")
            Predef.print(n)
            Predef.print(") = ")
            Predef.println(fibonacci(n))
            n = n + 1
        }
    }
}
