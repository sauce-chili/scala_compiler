// Test 1: Multidimensional arrays - spiral matrix fill with 0 and 1

final class SpiralMatrix {

    def fillSpiral(n: Int): Array[Array[Int]] = {
        val matrix: Array[Array[Int]] = new Array[Array[Int]](n)
        var idx: Int = 0
        while (idx < n) {
            matrix(idx) = new Array[Int](n)
            idx = idx + 1
        }

        var top: Int = 0
        var bottom: Int = n - 1
        var left: Int = 0
        var right: Int = n - 1
        var value: Int = 0

        while (top <= bottom && left <= right) {
            // go right
            var j: Int = left
            while (j <= right) {
                matrix(top)(j) = value
                value = 1 - value
                j = j + 1
            }
            top = top + 1

            // go down
            var i: Int = top
            while (i <= bottom) {
                matrix(i)(right) = value
                value = 1 - value
                i = i + 1
            }
            right = right - 1

            // go left
            if (top <= bottom) {
                j = right
                while (j >= left) {
                    matrix(bottom)(j) = value
                    value = 1 - value
                    j = j - 1
                }
                bottom = bottom - 1
            }

            // go up
            if (left <= right) {
                i = bottom
                while (i >= top) {
                    matrix(i)(left) = value
                    value = 1 - value
                    i = i - 1
                }
                left = left + 1
            }
        }

        matrix
    }

    def printMatrix(matrix: Array[Array[Int]], n: Int): Unit = {
        var i: Int = 0
        while (i < n) {
            var j: Int = 0
            while (j < n) {
                Predef.print(matrix(i)(j))
                Predef.print(" ")
                j = j + 1
            }
            Predef.println()
            i = i + 1
        }
    }

    def main(args: Array[String]): Unit = {
        val n: Int = 5
        Predef.println("=== Spiral Matrix 5x5 ===")
        val matrix: Array[Array[Int]] = fillSpiral(n)
        printMatrix(matrix, n)
    }
}
