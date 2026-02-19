// Test 16: Peredacha.kt translation - passing objects and arrays to functions

class Container {
    var n: Int = 10
    var s: String = "Old String"

    def printObject(): Unit = {
        Predef.print("Object number: ")
        Predef.println(n)
        Predef.print("Object string: ")
        Predef.println(s)
    }
}

final class PassingTest {

    def modifyObject(c: Container, number: Int, text: String): Unit = {
        // Parameters number and text are val (immutable), can't reassign
        // But we can modify the object's fields
        //number = 5
        //text = "534"
        c.n = number
        c.s = text
    }

    def clearArray(arr: Array[Int], size: Int): Unit = {
        var i: Int = 0
        while (i < size) {
            arr(i) = 0
            i = i + 1
        }
    }

    def main(args: Array[String]): Unit = {
        Predef.println("=== Pass-by-Reference Test ===")

        val array: Array[Int] = Array(1, 2, 3)
        val number: Int = 999
        val text: String = "New String"
        val c: Container = new Container()

        Predef.println("Before:")
        Predef.print("Number: ")
        Predef.println(number)
        Predef.print("String: ")
        Predef.println(text)
        c.printObject()
        Predef.print("Array: ")
        var i: Int = 0
        while (i < 3) {
            Predef.print(array(i))
            Predef.print(" ")
            i = i + 1
        }
        Predef.println()

        // Modify object and array through functions
        modifyObject(c, number, text)
        clearArray(array, 3)

        Predef.println("After:")
        Predef.print("Number (unchanged): ")
        Predef.println(number)
        Predef.print("String (unchanged): ")
        Predef.println(text)
        c.printObject()
        Predef.print("Array (zeroed): ")
        i = 0
        while (i < 3) {
            Predef.print(array(i))
            Predef.print(" ")
            i = i + 1
        }
        Predef.println()
    }
}
