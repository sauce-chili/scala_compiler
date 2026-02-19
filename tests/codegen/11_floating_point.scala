// Test 11: Floating point operations (+ FloatingPointNumber.kt translation)
final class A {
    def main(args: Array[String]): Unit = {
        new FloatingPointTest().floatingPointTest()
    }
}

final class FloatingPointTest {

    def floatingPointTest(): Unit = {
        Predef.println("=== Floating Point Test ===")

        Predef.println("Введите f1: ")
        var f1: Double = StdIn.readDouble()
        Predef.println("\nВведите f2: ")
        var f2: Double = StdIn.readDouble()
        Predef.println("\nВведите i1: ")
        var i1: Int = StdIn.readInt()
        Predef.println("\nВведите i2: ")
        var i2: Int = StdIn.readInt()

        // Sum
        Predef.println("\nSum:")
        Predef.print(i1 + " + " + i2 + " = ")
        Predef.println(i1 + i2)

        Predef.print(i1 + " + " + f1 + " = ")
        Predef.println(i1 + f1)

        Predef.print(f1 + " + " + i1 + " = ")
        Predef.println(f1 + i1)

        Predef.print(f1 + " + " + f2 + " = ")
        Predef.println(f1 + f2)
        Predef.println()

        // Sub
        Predef.println("Sub:")
        Predef.print(i1 + " - " + i2 + " = ")
        Predef.println(i1 - i2)

        Predef.print(i1 + " - " + f1 + " = ")
        Predef.println(i1 - f1)

        Predef.print(f1 + " - " + i1 + " = ")
        Predef.println(f1 - i1)

        Predef.print(f1 + " - " + f2 + " = ")
        Predef.println(f1 - f2)
        Predef.println()

        // Mul
        Predef.println("Mul:")
        Predef.print(i1 + " * " + i2 + " = ")
        Predef.println(i1 * i2)

        Predef.print(i1 + " * " + f1 + " = ")
        Predef.println(i1 * f1)

        Predef.print(f1 + " * " + i1 + " = ")
        Predef.println(f1 * i1)

        Predef.print(f1 + " * " + f2 + " = ")
        Predef.println(f1 * f2)
        Predef.println()

        // Div
        Predef.println("Div:")
        Predef.print(i1 + " / " + i2 + " = ")
        Predef.println(i1 / i2)

        Predef.print(i1 + " / " + f1 + " = ")
        Predef.println(i1 / f1)

        Predef.print(f1 + " / " + i1 + " = ")
        Predef.println(f1 / i1)

        Predef.print(f1 + " / " + f2 + " = ")
        Predef.println(f1 / f2)
        Predef.println()

        // Comparisons
        Predef.println("Comparisons:")
        if (f1 > f2) {
            Predef.println(f1 + " > " + f2)
        } else {
            Predef.println(f1 + " <= " + f2)
        }

        if (f1 == f1) {
            Predef.println("f1 == f1: true")
        }

        if (f1 != f2) {
            Predef.println("f1 != f2: true")
        }
    }
}
