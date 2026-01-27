// Test file for various type errors

class TypeErrorsTest {
    // Error 1: Type mismatch in initialization
    val x: Int = "hello"

    // Error 2: Undeclared type
    val y: NonExistent = null

    def testMethod(): Unit = {
        // Error 3: Undefined variable
        val a: Int = unknownVar

        // Error 4: Assignment to val
        val immutable: Int = 5
        immutable = 10
    }
}
