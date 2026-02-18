// Test 4: Control structures - loops (while, do/while, for)
final class A {
    def main(args: Array[String]): Unit = {
        new Loops().loopsTest()
    }
}

final class Loops {

    def loopsTest(): Unit = {
        // while loop
        Predef.println("=== While Loop ===")
        var i: Int = 0
        while (i < 5) {
            Predef.print(i)
            Predef.print(" ")
            i = i + 1
        }
        Predef.println()

        // do-while loop
        Predef.println("=== Do-While Loop ===")
        var j: Int = 10
        do {
            Predef.print(j)
            Predef.print(" ")
            j = j - 3
        } while (j > 0)
        Predef.println()

        // for loop over array
        Predef.println("=== For Loop (array) ===")
        val arr: Array[Int] = Array(10, 20, 30, 40, 50)
        for (x: Int <- arr) {
            Predef.print(x)
            Predef.print(" ")
        }
        Predef.println()

        // nested while
        Predef.println("=== Nested While (multiplication table 3x3) ===")
        var r: Int = 1
        while (r <= 3) {
            var c: Int = 1
            while (c <= 3) {
                Predef.print(r * c)
                Predef.print("\t")
                c = c + 1
            }
            Predef.println()
            r = r + 1
        }
    }
}
