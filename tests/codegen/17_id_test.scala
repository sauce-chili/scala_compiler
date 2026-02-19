final class A {
    def main(args: Array[String]): Unit = {
        new OverloadingTest().overloadingTest()
    }
}

class Vec(var x: Int,var y: Int) {
    def +(other: Vec): Vec = {
        return new Vec(x+other.x, y - other.y)
    }
}

final class OverloadingTest {
    def overloadingTest(): Unit = {
        var v1: Vec = new Vec(5,5)
        var v2: Vec = new Vec(1,1)
        var v3: Vec = v1 + v2
        Predef.println(v3.x)
        Predef.println(v3.y)
    }
}
