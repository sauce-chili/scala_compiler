// Test 20: Abstract method and var in class
final class A {
    def main(args: Array[String]): Unit = {
        new AbstractCallTest().abstractCallTest()
    }
}

abstract class MyAbstractClass {
    def myAbstractMethod(c: Char): Int

    var abstractVar: Int
}

class NonAbstractClass extends MyAbstractClass {
    override def myAbstractMethod(c: Char): Int = {
        Predef.println(c)
        Predef.println(abstractVar)
        5
    }

    var abstractVar: Int = 4
}

class AbstractCallTest {
    def abstractCallTest(): Unit = {
        var abstractClass: MyAbstractClass = new NonAbstractClass()
        abstractClass.myAbstractMethod('x')
    }
}
