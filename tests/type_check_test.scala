// Test file for type checking

class Animal {
    val name: String = "Animal"
    def speak(): String = "..."
}

class Dog extends Animal {
    override val name: String = "Dog"
    override def speak(): String = "Woof!"

    def wagTail(): Unit = {}
}

class Cat extends Animal {
    override val name: String = "Cat"
    override def speak(): String = "Meow!"
}

class TypeCheckTest {
    // Test 1: Basic variable assignment
    val x: Int = 42
    val y: Double = 3.14
    val s: String = "hello"
    val b: Boolean = true

    // Test 2: Numeric widening (Int -> Double)
    val d: Double = 10

    // Test 3: Inheritance assignment
    val animal: Animal = new Dog()

    // Test 4: null assignment to reference type
    val nullAnimal: Animal = null

    // Test 5: Array types
    val arr: Array[Int] = new Array[Int](10)

    def testMethod(): Unit = {
        // Test 6: Local variable assignment
        var localVar: Int = 5
        localVar = 10

        // Test 7: Field access
        val animalName: String = animal.name

        // Test 8: Method call
        val sound: String = animal.speak()

        // Test 9: Conditions must be boolean
        if (b) {
            localVar = 20
        }

        // Test 10: While with boolean condition
        while (localVar < 100) {
            localVar = localVar + 1
        }
    }
}
