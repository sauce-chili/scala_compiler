// Test 5: Classes, properties, methods, single inheritance with dynamic dispatch

final class A {
    def main(args: Array[String]): Unit = {
        new OopTest().oopTest()
    }
}

class Mother {
    def speak(): Unit = {
        Predef.println("Mother")
    }
}

class Son extends Mother {
    override def speak(): Unit = {
        super.speak()
        Predef.println("Son")
    }
}

class Daughter extends Mother {
    override def speak(): Unit = {
        Predef.println("Daughter")
    }
}

final class OopTest {
    def oopTest(): Unit = {
        Predef.println("=== OOP Inheritance Test ===")

        val mom: Mother = new Mother()
        val son: Mother = new Son()
        val daughter: Mother = new Daughter()

        Predef.println("--- Individual calls ---")
        mom.speak()
        son.speak()
        daughter.speak()

        Predef.println("--- Polymorphic array ---")
        val family: Array[Mother] = Array(mom, son, daughter)
        for (member: Mother <- family) {
            member.speak()
            Predef.println("---")
        }
    }
}
