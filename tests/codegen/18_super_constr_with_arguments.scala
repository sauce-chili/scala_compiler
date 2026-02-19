final class A {
    def main(args: Array[String]): Unit = {
        new ConstructorsTest().constructorsTest()
    }
}

class Creation(val name: String) {

}

class Person(override val name: String, var age: Int) extends Creation(name) {

}

final class ConstructorsTest {
    def constructorsTest(): Unit = {
        var person: Person = new Person("Oleg", 67)
        Predef.println(person.name)
        Predef.println(person.age)
    }
}
