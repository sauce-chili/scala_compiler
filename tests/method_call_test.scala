// Test file for method call validation

class Calculator {
    def add(a: Int, b: Int): Int = a + b
    def multiply(a: Int, b: Int): Int = a * b
    def ++(a: Int) = a + 1
}

class MethodCallTest {
    def testMethod(): Unit = {
        val calc: Calculator = new Calculator()

        // Valid call
        val sum: Int = calc.add(1, 2)

        val s: Int = calc ++ 1

        // Error: wrong argument types
        val result: Int = calc.add("a", "b")

        // Error: method not found
        val x: Int = calc.subtract(5, 3)
    }
}
