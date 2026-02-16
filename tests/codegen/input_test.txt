final class A {
    def main(args: Array[String]): Unit = {
        new B().myM2()
    }
}

class B {
    def myM2(): Unit = {
        Predef.print("Enter double: ")
        var doubleValue: Double = StdIn.readDouble()
        Predef.print("Your double value: ")
        Predef.println(doubleValue)
        Predef.println()

        Predef.print("Enter char: ")
        var charValue: Char = StdIn.readChar()
        Predef.print("Your char value: ")
        Predef.println(charValue)
        Predef.println()

        Predef.print("Enter integer: ")
        var intValue: Int = StdIn.readInt()
        Predef.print("Your integer value: ")
        Predef.println(intValue)
        Predef.println()

        Predef.print("Enter string: ")
        var stringValue: String = StdIn.readLine()
        Predef.print("Your string value: ")
        Predef.println(stringValue)
        Predef.println()

        Predef.print("Enter boolean: ")
        var booleanValue: Boolean = StdIn.readBoolean()
        Predef.print("Your boolean value: ")
        Predef.println(booleanValue)
    }
}