// Test 10: Console I/O - input/output of basic types

final class A {
    def main(args: Array[String]): Unit = {
        new ConsoleIOTest().consoleIOTest()
    }
}

final class ConsoleIOTest {

    def consoleIOTest(): Unit = {
        Predef.println("=== Console I/O Test ===")

        // Output of different types
        Predef.println("--- Output ---")
        Predef.println(42)
        Predef.println(3.14)
        Predef.println("Hello")
        Predef.println(true)
        Predef.println('X')
        Predef.println()

        // print (no newline) + println
        Predef.print("A")
        Predef.print("B")
        Predef.print("C")
        Predef.println()

        // Input and computation
        Predef.println("--- Input ---")
        Predef.print("Enter an integer: ")
        val num: Int = StdIn.readInt()
        Predef.print("You entered: ")
        Predef.println(num)

        Predef.print("Doubled: ")
        Predef.println(num * 2)

        Predef.print("Enter your name: ")
        val name: String = StdIn.readLine()
        Predef.print("Hello, ")
        Predef.print(name)
        Predef.println("!")

        Predef.print("Enter a double: ")
        val d: Double = StdIn.readDouble()
        Predef.print("Your double: ")
        Predef.println(d)
    }
}
