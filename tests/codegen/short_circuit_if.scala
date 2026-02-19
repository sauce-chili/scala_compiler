final class A {
    def main(args: Array[String]): Unit = {
        new Spiral().myM()
    }
}

class Spiral {
  def myM(): Unit = {
    if (false && hello()) {
        Predef.println("If condition true")
    }
    Predef.println("end")

    var myBool: Boolean = true && hello()
  }

  def hello(): Boolean = {
    Predef.println("Hello")
    return false
  }
}

