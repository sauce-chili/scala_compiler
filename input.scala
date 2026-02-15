// Test file for method call validation
// o
class B{
    def myM(): Unit = {
        var c: Char = 's'
    }
}


class Calculator {
    val c:Calculator = null
    def add(a: Int, b: Int): Int = a + b
    def multiply(a: Int, b: Int): Int = a * b
    def ++(a: Int): Int = 57


}

class MethodCallTest {
    def testMethod(): Unit = {
        val calc: Calculator = new Calculator()

        // Valid call
        val sum: Int = calc.c.add(1, 2)

        val s: Int = calc ++ 1
        //val arr: Array[String] = Array("4","4")
        //val x:Int = arr(5)
        //5 - "7"i

        // Error: wrong argument types
        val result: Double = calc.add(5, sum)

        // Error: method not found
//        val x: Int = calc.subtract(sum, 3)

        val overloadedInt: Int = overloadByParam(4)
        val overloadedDouble: Double = overloadByParam(4.0)
        val overloadedString: String = overloadByParam("S")
    }

    def overloadByParam(fVal: Int): Int = {
        5
    }

    def overloadByParam(fVal: Double): Double = {
        return 5.8
    }

    def overloadByParam(fVal: String): String = {
        return "43"
    }

    def overloadByReturnVal(iVal: Int, fVal: Int): Int = {
        return iVal + fVal
    }

    def overloadByReturnVal(): Int = {
        return 0
    }
}

final
// class A

class A {

    def arr(x: Int): String = {
        return "this  func call"
    }

    def main(args: Array[String]): Unit = {
        val c: Calculator = new Calculator()
        Predef.println("Hi")
        val d: Int = 4
        if (true) {
            val d:Int = 0
        }
        //val + : Int = StdIn.readInt()
        val arrt: Array[Int] = Array(3 + 1,3)
        Predef.println(arrt(-1))
        //Predef.println(+)
    }
}