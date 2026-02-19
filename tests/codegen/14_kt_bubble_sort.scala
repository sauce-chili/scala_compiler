// Test 14: BubbleSort with range generators
final class A {
    def main(args: Array[String]): Unit = {
        new BubbleSort().bubbleSort()
    }
}

final class BubbleSort {

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

    def bubbleSort(): Unit = {
        Predef.println("=== Bubble Sort Test ===")

        val array: Array[Int] = Array(64, 34, 25, 12, 22, 11, 90)

        Predef.println("Before:")
        printArray(array)

        bubbleSort(array)

        Predef.println("\nAfter:")
        printArray(array)

        Predef.println("\n--- Sort another array ---")
        val arr2: Array[Int] = Array(5, 1, 4, 2, 8, 3, 7, 6)

        Predef.print("Before: ")
        for (x: Int <- arr2) {
            Predef.print(x)
            Predef.print(" ")
        }
        Predef.println()

        bubbleSort(arr2)

        Predef.print("After:  ")
        for (x: Int <- arr2) {
            Predef.print(x)
            Predef.print(" ")
        }
        Predef.println()
    }
}
