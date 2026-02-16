package rtl;

public final class Predef extends Any {

    public Predef() { }

    /**
     * Prints an object using its toString().
     */
    public rtl.Unit print(Any x) {
        System.out.print(x == null ? "null" : x.toString());
        return new Unit();
    }

    /**
     * Prints a newline.
     */
    public rtl.Unit println() {
        System.out.println();
        return new Unit();
    }

    /**
     * Prints an object followed by a newline.
     */
    public rtl.Unit println(Any x) {
        System.out.println(x == null ? "null" : x.toString());
        return new Unit();
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
