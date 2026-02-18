// Test 14: BubbleSort.kt translation
final class A {
    def main(args: Array[String]): Unit = {
        new BubbleSort().bubbleSort()
    }
}

final class BubbleSort {

    def bubbleSort(arr: Array[Int], arraySize: Int): Unit = {
        var i: Int = 0
        while (i < arraySize - 1) {
            var j: Int = 0
            while (j < arraySize - i - 1) {
                if (arr(j) > arr(j + 1)) {
                    val temp: Int = arr(j)
                    arr.update(j, arr(j + 1))
                    arr.update(j + 1, temp)
                }
                j = j + 1
            }
            i = i + 1
        }
    }

    def printArray(arr: Array[Int], size: Int): Unit = {
        var i: Int = 0
        while (i < size - 1) {
            Predef.print(arr.apply(i))
            Predef.print(" ")
            i = i + 1
        }
    }

    def bubbleSort(): Unit = {
        Predef.println("=== Bubble Sort Test ===")

        val arraySize: Int = 7
        val array: Array[Int] = Array(64, 34, 25, 12, 22, 11, 90)

        Predef.println("Before:")
        printArray(array, arraySize)

        bubbleSort(array, arraySize)

        Predef.println("After:")
        printArray(array, arraySize)

        // Also test with for loop using until
        Predef.println("--- Sort another array ---")
        val arr2: Array[Int] = Array(5, 1, 4, 2, 8, 3, 7, 6)
        val size2: Int = 8

        Predef.print("Before: ")
        for (x: Int <- arr2) {
            Predef.print(x)
            Predef.print(" ")
        }
        Predef.println()

        bubbleSort(arr2, size2)

        Predef.print("After:  ")
        for (x: Int <- arr2) {
            Predef.print(x)
            Predef.print(" ")
        }
        Predef.println()
    }
}