package rtl;

public final class Predef {

    public Predef() { }

    /**
     * Prints an object using its toString().
     */
    public void print(Object x) {
        System.out.print(x == null ? "null" : x.toString());
    }

    /**
     * Prints a newline.
     */
    public void println() {
        System.out.println();
    }

    /**
     * Prints an object followed by a newline.
     */
    public void println(Object x) {
        System.out.println(x == null ? "null" : x.toString());
    }
}
