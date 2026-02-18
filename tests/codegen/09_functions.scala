// Test 9: Functions (procedures) + recursion (Recursion.kt translation)

final class A {
    def main(args: Array[String]): Unit = {
        new FunctionsTest().functionsTest()
    }
}

class MathHelper {
    // Recursive Fibonacci
    def fibonacci(n: Int): Int = {
        if (n == 0) { return 0 }
        if (n == 1) { return 1 }
        return fibonacci(n - 1) + fibonacci(n - 2)
    }

    // Recursive factorial
    def factorial(n: Int): Int = {
        if (n <= 1) { return 1 }
        return n * factorial(n - 1)
    }
}

final class FunctionsTest {

    // Procedure (returns Unit)
    def greet(name: String): Unit = {
        Predef.print("Hello, ")
        Predef.print(name)
        Predef.println("!")
    }

    // Value-returning function
    def add(a: Int, b: Int): Int = a + b

    def multiply(a: Int, b: Int): Int = a * b

    // Function calling other functions
    def compute(x: Int, y: Int): Int = {
        val sum: Int = add(x, y)
        val product: Int = multiply(x, y)
        return sum + product
    }

    // Function with explicit return
    def isEven(n: Int): Boolean = {
        if (n % 2 == 0) {
            return true
        }
        return false
    }

    def functionsTest(): Unit = {
        Predef.println("=== Functions Test ===")

        // Procedure call
        greet("World")
        greet("Scala")

        // Value-returning calls
        Predef.print("add(3, 4) = ")
        Predef.println(add(3, 4))
        Predef.print("multiply(5, 6) = ")
        Predef.println(multiply(5, 6))
        Predef.print("compute(3, 4) = ")
        Predef.println(compute(3, 4))

        // Boolean function
        Predef.print("isEven(4) = ")
        Predef.println(isEven(4))
        Predef.print("isEven(7) = ")
        Predef.println(isEven(7))

        // Recursion (Recursion.kt)
        Predef.println("=== Fibonacci ===")
        val math: MathHelper = new MathHelper()
        var i: Int = 0
        while (i <= 10) {
            Predef.print("fibonacci(")
            Predef.print(i)
            Predef.print(") = ")
            Predef.println(math.fibonacci(i))
            i = i + 1
        }

        Predef.println("=== Factorial ===")
        i = 0
        while (i <= 7) {
            Predef.print("factorial(")
            Predef.print(i)
            Predef.print(") = ")
            Predef.println(math.factorial(i))
            i = i + 1
        }
    }
}
