final class A {
    def main(args: Array[String]): Unit = {
        new B().myM2()
    }
}

class B {
    def myM2(): Unit = {
        var threeDArray: Array[Array[Array[Int]]] = Array(
            Array(Array(1, 2, 3), Array(4, 5, 6)),
            Array(Array(10, 11, 12), Array(13, 14, 15))
        )

        for (twoDArray: Array[Array[Int]] <- threeDArray) {
           Predef.println("\nTwo d array: ")
           for (oneDArray: Array[Int] <- twoDArray) {
               Predef.println("One d array: ")
               for (arrNum: Int <- oneDArray) {
                   Predef.println(arrNum)
               }
           }
        }
    }
}
