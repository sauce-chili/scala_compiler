package rtl;

public final class Predef extends Any {

    public Predef() { }

    /**
     * Prints an object using its toString().
     */
    public void print(Any x) {
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
    public void println(Any x) {
        System.out.println(x == null ? "null" : x.toString());
    }

    @Override
    public boolean equals(Object o) {
        return o instanceof Object;
    }

    @Override
    public int hashCode() {
        return 0;
    }
}
