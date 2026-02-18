// Test 7: Numeric, character, and string literals with escape sequences
final class A {
    def main(args: Array[String]): Unit = {
        new LiteralsTest().literalsTest()
    }
}

final class LiteralsTest {

    def literalsTest(): Unit = {
        Predef.println("=== Literals Test ===")

        // Integer literals
        Predef.println("--- Integers ---")
        val zero: Int = 0
        val positive: Int = 42
        val negative: Int = -100
        val hex: Int = 0xFF
        val binary: Int = 0b1010
        val million: Int = 1000000

        Predef.print("zero = ")
        Predef.println(zero)
        Predef.print("positive = ")
        Predef.println(positive)
        Predef.print("negative = ")
        Predef.println(negative)
        Predef.print("hex 0xFF = ")
        Predef.println(hex)
        Predef.print("binary 0b1010 = ")
        Predef.println(binary)
        Predef.print("million = ")
        Predef.println(million)

        // Double literals
        Predef.println("--- Doubles ---")
        val pi: Double = 3.14159
        val negDouble: Double = -2.5
        val zeroDouble: Double = 0.0

        Predef.print("pi = ")
        Predef.println(pi)
        Predef.print("negDouble = ")
        Predef.println(negDouble)
        Predef.print("zeroDouble = ")
        Predef.println(zeroDouble)

        // Char literals with escape sequences
        Predef.println("--- Chars ---")
        val letterA: Char = 'A'
        val letterZ: Char = 'z'
        val digit: Char = '9'
        val newline: Char = '\n'
        val tab: Char = '\t'
        val backslash: Char = '\\'
        val quote: Char = '\''

        Predef.print("letterA = ")
        Predef.println(letterA)
        Predef.print("letterZ = ")
        Predef.println(letterZ)
        Predef.print("digit = ")
        Predef.println(digit)

        // String literals with escape sequences
        Predef.println("--- Strings ---")
        val hello: String = "Hello, World!"
        val withNewline: String = "Line1\nLine2"
        val withTab: String = "Col1\tCol2"
        val withQuote: String = "He said \"hi\""
        val withBackslash: String = "path\\to\\file"
        val empty: String = ""

        Predef.println(hello)
        Predef.println(withNewline)
        Predef.println(withTab)
        Predef.println(withQuote)
        Predef.println(withBackslash)
        Predef.print("empty string length = 0: ")
        Predef.println(empty)

        // Boolean literals
        Predef.println("--- Booleans ---")
        val yes: Boolean = true
        val no: Boolean = false
        Predef.print("yes = ")
        Predef.println(yes)
        Predef.print("no = ")
        Predef.println(no)
    }
}
