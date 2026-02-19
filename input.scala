final class A {
    def main(args: Array[String]): Unit = {
        new B().myM2()
    }
}

class Point(val x: Int, val y: Int){
}

class B {
    def createNullPoint(): Point = new Point(0,0)
    def myM2(): Unit = {
        val p: Point = createNullPoint()
        Predef.println(p.x)
        Predef.println(p.y)
    }
}
