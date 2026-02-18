// Test 2: Boolean operations AND, OR, NOT (+ Logic2.kt translation)
final class A {
    def main(args: Array[String]): Unit = {
        new BooleanOps().booleanMethod()
    }
}


final class BooleanOps {

    def hi(b: Boolean): Boolean = {
        Predef.println("hello")
        b
    }

    def booleanMethod(): Unit = {
        Predef.println("=== Boolean Truth Table ===")

        val t: Boolean = true
        val f: Boolean = false

        // AND (&&)
        Predef.println("AND (&&):")
        Predef.print("true && true = ")
        Predef.println(t && t)
        Predef.print("true && false = ")
        Predef.println(t && f)
        Predef.print("false && true = ")
        Predef.println(f && t)
        Predef.print("false && false = ")
        Predef.println(f && f)

        // OR (||)
        Predef.println("OR (||):")
        Predef.print("true || true = ")
        Predef.println(t || t)
        Predef.print("true || false = ")
        Predef.println(t || f)
        Predef.print("false || true = ")
        Predef.println(f || t)
        Predef.print("false || false = ")
        Predef.println(f || f)

        // NOT (!)
        Predef.println("NOT (!):")
        Predef.print("!true = ")
        Predef.println(!t)
        Predef.print("!false = ")
        Predef.println(!f)

        // XOR (^)
        Predef.println("XOR (^):")
        Predef.print("true ^ false = ")
        Predef.println(t ^ f)
        Predef.print("true ^ true = ")
        Predef.println(t ^ t)

        // Combined expression
        val result: Boolean = (t && f) || (!f && t)
        Predef.print("(true && false) || (!false && true) = ")
        Predef.println(result)

        // Logic2.kt translation
        Predef.println("=== Logic2 Test ===")
        val a: Int = 15

        if (a > 10 && hi(true)) {
            Predef.println(a + " > 10 && hi(true)")
        }
        if (a > 5 && hi(true)) {
            Predef.println(a + " > 5 && hi(true)")
        }
        if (a > 20 && hi(true)) {
            Predef.println(a + " > 20 && hi(true)")
        }
    }
}
