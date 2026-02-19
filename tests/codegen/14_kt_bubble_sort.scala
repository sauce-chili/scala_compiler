// Test 14: BubbleSort with range generators
final class A {

    def bubbleSort(arr: Array[Int]): Unit = {
        for (i: Int <- 0 until (arr.length() - 1)) {
            for (j: Int <- 0 until (arr.length() - i - 1)) {
                if (arr(j) > arr(j + 1)) {
                    val temp: Int = arr(j)
                    arr(j) = arr(j + 1)
                    arr(j + 1) = temp
                }
            }
        }
    }

    def printArray(arr: Array[Int]): Unit = {
        for (i: Int <- 0 until arr.length()) {
            Predef.print(arr(i))
            Predef.print(" ")
        }
    }

    def main(args: Array[String]): Unit = {
        Predef.println("=== Bubble Sort Test ===")

        Predef.println("Размер: ")
        val n: Int = StdIn.readInt()
        val array: Array[Int] = new Array[Int](n)
        for(i: Int <- 0 until n) {
            Predef.println("Элемент: ")
            array(i) = StdIn.readInt()
        }

        Predef.println("Before:")
        printArray(array)

        bubbleSort(array)

        Predef.println("\nAfter:")
        printArray(array)
    }
}
