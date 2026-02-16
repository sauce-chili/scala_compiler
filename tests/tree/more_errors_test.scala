// Test file for more type errors

class MoreErrorsTest {
    def testMethod(): Unit = {
        // Error 1: Undefined variable
        val a: Int = unknownVar

        // Error 2: Assignment to val
        val immutable: Int = 5
        immutable = 10

        // Error 3: Type mismatch in var assignment
        var mutable: Int = 5
        mutable = "string"

        // Error 4: Non-boolean condition in if
        if (42) {
            val x: Int = 1
        }
    }
}
