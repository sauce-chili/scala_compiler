final class A {
    def main(args: Array[String]): Unit = {
        new ConstructorsTest().constructorsTest()
    }
}

final class ConstructorsTest {
    def constructorsTest(): Unit = {
        var i: Int = 2
        var d: Double = 11.6
        Predef.println(i + d)
        Predef.println(i - d)
        Predef.println(i * d)
        Predef.println(i / d)

        Predef.println(5 / 2)
    }
}