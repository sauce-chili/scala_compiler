// Test 11: Floating point operations (+ FloatingPointNumber.kt translation)
final class A {
    def main(args: Array[String]): Unit = {
        new FloatingPointTest().floatingPointTest()
    }
}

final class FloatingPointTest {

    def floatingPointTest(): Unit = {
        Predef.println("=== Floating Point Test ===")

        var f1: Double = 3.7
        var f2: Double = -5.2
        var i1: Int = 10
        var i2: Int = -5

        // Sum
        Predef.println("Sum:")
        Predef.print(i1 + " + " + i2 + " = ")
        Predef.println(i1 + i2)

        Predef.print(i1 + " + " + f1 + " = ")
        Predef.println(i1.toFloat() + f1)

        Predef.print(f1 + " + " + i1 + " = ")
        Predef.println(f1 + i1.toFloat())

        Predef.print(f1 + " + " + f2 + " = ")
        Predef.println(f1 + f2)
        Predef.println()

        // Sub
        Predef.println("Sub:")
        Predef.print(i1 + " - " + i2 + " = ")
        Predef.println(i1 - i2)

        Predef.print(i1 + " - " + f1 + " = ")
        Predef.println(i1.toFloat() - f1)

        Predef.print(f1 + " - " + i1 + " = ")
        Predef.println(f1 - i1.toFloat())

        Predef.print(f1 + " - " + f2 + " = ")
        Predef.println(f1 - f2)
        Predef.println()

        // Mul
        Predef.println("Mul:")
        Predef.print(i1 + " * " + i2 + " = ")
        Predef.println(i1 * i2)

        Predef.print(i1 + " * " + f1 + " = ")
        Predef.println(i1.toFloat() * f1)

        Predef.print(f1 + " * " + i1 + " = ")
        Predef.println(f1 * i1.toFloat())

        Predef.print(f1 + " * " + f2 + " = ")
        Predef.println(f1 * f2)
        Predef.println()

        // Div
        Predef.println("Div:")
        Predef.print(i1 + " / " + i2 + " = ")
        Predef.println(i1 / i2)

        Predef.print(i1 + " / " + f1 + " = ")
        Predef.println(i1.toFloat() / f1)

        Predef.print(f1 + " / " + i1 + " = ")
        Predef.println(f1 / i1.toFloat())

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
