package rtl;

/**
 * Обёртка для целого числа.
 */
public final class Int extends Any {
    private final int value;

    public Int(int value) { this.value = value; }

    public static Int valueOf(int v) { return new Int(v); }

    public int intValue() { return value; }

    public Int add(Int other) { return new Int(this.value + other.value); }
    public Int sub(Int other) { return new Int(this.value - other.value); }
    public Int mul(Int other) { return new Int(this.value * other.value); }
    public Int div(Int other) {
        if (other.value == 0) throw new ArithmeticException("Division by zero");
        return new Int(this.value / other.value);
    }
    public Int mod(Int other) {
        if (other.value == 0) throw new ArithmeticException("Division by zero");
        return new Int(this.value % other.value);
    }
    public Int neg() { return new Int(-this.value); }

    /* Static arithmetic */
    public static Int add(Int a, Int b) { return a.add(b); }
    public static Int sub(Int a, Int b) { return a.sub(b); }
    public static Int mul(Int a, Int b) { return a.mul(b); }
    public static Int div(Int a, Int b) { return a.div(b); }
    public static Int mod(Int a, Int b) { return a.mod(b); }

    /* Comparisons returning rtl.Boolean */
    public rtl.Boolean lt(Int other) { return rtl.Boolean.valueOf(this.value < other.value); }
    public rtl.Boolean le(Int other) { return rtl.Boolean.valueOf(this.value <= other.value); }
    public rtl.Boolean gt(Int other) { return rtl.Boolean.valueOf(this.value > other.value); }
    public rtl.Boolean ge(Int other) { return rtl.Boolean.valueOf(this.value >= other.value); }
    public rtl.Boolean eq(Int other) { return rtl.Boolean.valueOf(this.value == other.value); }
    public rtl.Boolean ne(Int other) { return rtl.Boolean.valueOf(this.value != other.value); }

    public static rtl.Boolean lt(Int a, Int b) { return a.lt(b); }
    public static rtl.Boolean le(Int a, Int b) { return a.le(b); }
    public static rtl.Boolean gt(Int a, Int b) { return a.gt(b); }
    public static rtl.Boolean ge(Int a, Int b) { return a.ge(b); }
    public static rtl.Boolean eq(Int a, Int b) { return a.eq(b); }
    public static rtl.Boolean ne(Int a, Int b) { return a.ne(b); }

    /* Conversions */
    public Float toFloat() { return new Float((double) this.value); }
    public Char toChar() { return new Char((char) this.value); }
    public String toStringValue() { return new String(java.lang.Integer.toString(this.value)); }

    @Override
    public boolean equals(Object o) {
        return (o instanceof Int) && ((Int)o).value == this.value;
    }

    @Override
    public int hashCode() { return Integer.hashCode(value); }

    @Override
    public java.lang.String toString() {
        return java.lang.Integer.toString(value);
    }
}
