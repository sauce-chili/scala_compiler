// Test 12: Function overloading by parameter types
final class A {
    def main(args: Array[String]): Unit = {
        new OverloadingTest().overloadingTest()
    }
}

class Printer {
    // Overload by parameter type
    def show(x: Int): Unit = {
        Predef.print("Int: ")
        Predef.println(x)
    }

    def show(x: Double): Unit = {
        Predef.print("Double: ")
        Predef.println(x)
    }

    def show(x: String): Unit = {
        Predef.print("String: ")
        Predef.println(x)
    }

    def show(x: Boolean): Unit = {
        Predef.print("Boolean: ")
        Predef.println(x)
    }

    // Overload by parameter count
    def calc(a: Int): Int = a * 2

    def calc(a: Int, b: Int): Int = a + b

    def calc(a: Int, b: Int, c: Int): Int = a + b + c
}

final class OverloadingTest {
    def overloadingTest(): Unit = {
        Predef.println("=== Function Overloading Test ===")

        val p: Printer = new Printer()

        // Overload by type
        Predef.println("--- By type ---")
        p.show(42)
        p.show(3.14)
        p.show("hello")
        p.show(true)

        // Overload by arity
        Predef.println("--- By arity ---")
        Predef.print("calc(5) = ")
        Predef.println(p.calc(5))
        Predef.print("calc(3, 4) = ")
        Predef.println(p.calc(3, 4))
        Predef.print("calc(1, 2, 3) = ")
        Predef.println(p.calc(1, 2, 3))
    }
}
