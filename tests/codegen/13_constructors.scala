// Test 13: Constructor overloading (primary + secondary)
final class A {
    def main(args: Array[String]): Unit = {
        new ConstructorsTest().constructorsTest()
    }
}


class Point(val x: Int, var y: Int) {
    var r: Double = 0.0
    // Secondary constructor: 1 arg
    def this(x: Int) = this(x, 0)

    // Secondary constructor: no args
    def this() = this(0, 0)

    def this(x: Int,r: Double) = {
        this()
        Predef.println("Polar")
        this.r = r
    }

    def distSquared(): Int = x * x + y * y

    def printPoint(): Unit = {
        Predef.print("Point(")
        Predef.print(x)
        Predef.print(", ")
        Predef.print(y)
        Predef.print(", ")
        Predef.print(r)
        Predef.println(")")
    }
}

class Person(val name: String, var age: Int) {
    // Secondary constructor with default age
    def this(name: String) = this(name, 0)

    def printPerson(): Unit = {
        Predef.print("Person(name=")
        Predef.print(name)
        Predef.print(", age=")
        Predef.print(age)
        Predef.println(")")
    }

    def birthday(): Unit = {
        age = age + 1
    }
}

final class ConstructorsTest {
    def constructorsTest(): Unit = {
        Predef.println("=== Constructors Test ===")

        // Point with primary constructor (2 args)
        val p1: Point = new Point(3, 4)
        Predef.print("p1: ")
        p1.printPoint()
        Predef.print("p1.distSquared = ")
        Predef.println(p1.distSquared())

        // Point with secondary constructor (1 arg)
        val p2: Point = new Point(5)
        Predef.print("p2: ")
        p2.printPoint()

        // Point with secondary constructor (no args)
        val p3: Point = new Point(4,2.3)
        Predef.print("p3: ")
        p3.printPoint()

        // Mutable field via var
        p1.y = 10
        Predef.print("p1 after y=10: ")
        p1.printPoint()

        // Person constructors
        Predef.println("--- Person ---")
        val person1: Person = new Person("Alice", 30)
        person1.printPerson()

        val person2: Person = new Person("Bob")
        person2.printPerson()

        person2.birthday()
        person2.birthday()
        Predef.print("Bob after 2 birthdays: ")
        person2.printPerson()
    }
}
