// Test 6: Access control - private/protected properties and methods
final class A {
    def main(args: Array[String]): Unit = {
        new AccessControlTest().accessControlTest()
    }
}

class Account {
    private var balance: Int = 100
    protected var ownerName: String = "DefaultOwner"
    var id: Int = 1

    def getBalance(): Int = balance

    protected def addFunds(amount: Int): Unit = {
        balance = balance + amount
    }

    def printInfo(): Unit = {
        Predef.print("Account id=")
        Predef.print(id)
        Predef.print(" owner=")
        Predef.print(ownerName)
        Predef.print(" balance=")
        Predef.println(balance)
    }
}

class SavingsAccount extends Account {
    def deposit(amount: Int): Unit = {
        // protected method - accessible from subclass
        addFunds(amount)
    }

    def getOwner(): String = {
        // protected field - accessible from subclass
        ownerName
    }

    def setOwner(name: String): Unit = {
        ownerName = name
    }
}

final class AccessControlTest {
    def accessControlTest(): Unit = {
        Predef.println("=== Access Control Test ===")

        val acc: Account = new Account()
        // public field - OK
        acc.id = 42
        // public method - OK
        val bal: Int = acc.getBalance()
        Predef.print("Balance via public getter: ")
        Predef.println(bal)
        acc.printInfo()

        Predef.println("--- SavingsAccount (subclass) ---")
        val savings: SavingsAccount = new SavingsAccount()
        savings.setOwner("Alice")
        savings.deposit(50)
        Predef.print("Owner via protected access: ")
        Predef.println(savings.getOwner())
        savings.printInfo()

        // The following would cause semantic errors:
        // acc.balance       -- private field
        // acc.ownerName     //-- protected field from outside hierarchy
        // acc.addFunds(10)  //-- protected method from outside hierarchy
    }
}
