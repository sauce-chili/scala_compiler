// Test 3: Local variables - val/var, scoping, shadowing, multiple types
final class A {
    def main(args: Array[String]): Unit = {
        new LocalVars().localVarsMethod()
    }
}

final class LocalVars {

    def localVarsMethod(): Unit = {
        Predef.println("=== Local Variables Test ===")

        // val (immutable)
        val intVal: Int = 42
        val doubleVal: Double = 3.14
        val stringVal: String = "hello"
        val charVal: Char = 'A'
        val boolVal: Boolean = true

        Predef.print("intVal = ")
        Predef.println(intVal)
        Predef.print("doubleVal = ")
        Predef.println(doubleVal)
        Predef.print("stringVal = ")
        Predef.println(stringVal)
        Predef.print("charVal = ")
        Predef.println(charVal)
        Predef.print("boolVal = ")
        Predef.println(boolVal)

        // var (mutable) + reassignment
        var counter: Int = 0
        Predef.print("counter before = ")
        Predef.println(counter)
        counter = counter + 10
        Predef.print("counter after +10 = ")
        Predef.println(counter)
        counter = counter * 2
        Predef.print("counter after *2 = ")
        Predef.println(counter)

        // Block scoping and shadowing
        val x: Int = 100
        Predef.print("outer x = ")
        Predef.println(x)
        {
            val x: Int = 200
            Predef.print("inner x = ")
            Predef.println(x)
        }
        Predef.print("outer x again = ")
        Predef.println(x)

        // Multiple variables in sequence
        val a: Int = 1
        val b: Int = 2
        val c: Int = a + b
        Predef.print("a + b = ")
        Predef.println(c)
    }
}
