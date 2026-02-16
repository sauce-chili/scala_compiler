package rtl;

/**
 * Обёртка для целого числа.
 */
public final class Int extends Any {
    private final int value;

    public Int(int value) { this.value = value; }

    public Int(Int other) { this.value = other.value; }

    public int intValue() { return value; }

    public Int add(Int other)    { return new Int(this.value + other.value); }
    public Int $plus(Int other)  { return new Int(this.value + other.value); }
    public Int sub(Int other)    { return new Int(this.value - other.value); }
    public Int $minus(Int other) { return new Int(this.value - other.value); }
    public Int mul(Int other)    { return new Int(this.value * other.value); }
    public Int $times(Int other) { return new Int(this.value * other.value); }

    public Int div(Int other) {
        if (other.value == 0) throw new ArithmeticException("Division by zero");
        return new Int(this.value / other.value);
    }

    public Int $div(Int other) {
        if (other.value == 0) throw new ArithmeticException("Division by zero");
        return new Int(this.value / other.value);
    }


    public Int mod(Int other) {
        if (other.value == 0) throw new ArithmeticException("Division by zero");
        return new Int(this.value % other.value);
    }

    public Int $percent(Int other) {
        if (other.value == 0) throw new ArithmeticException("Division by zero");
        return new Int(this.value % other.value);
    }

    public Int neg() { return new Int(-this.value); }

    public Int unary_$plus()  { return this; }
    public Int unary_$minus() { return new Int(-this.value); }
    public Int unary_$tilde() { return new Int(~this.value); }

    public Boolean lt(Int other)          { return Boolean.valueOf(this.value < other.value); }
    public Boolean $less(Int other)       { return Boolean.valueOf(this.value < other.value); }
    public Boolean le(Int other)          { return Boolean.valueOf(this.value <= other.value); }
    public Boolean $less$eq(Int other)    { return Boolean.valueOf(this.value <= other.value); }
    public Boolean gt(Int other)          { return Boolean.valueOf(this.value > other.value); }
    public Boolean $greater(Int other)    { return Boolean.valueOf(this.value > other.value); }
    public Boolean ge(Int other)          { return Boolean.valueOf(this.value >= other.value); }
    public Boolean $greater$eq(Int other) { return Boolean.valueOf(this.value >= other.value); }
    public Boolean eq(Int other)          { return Boolean.valueOf(this.value == other.value); }
    public Boolean $eq$eq(Int other)      { return Boolean.valueOf(this.value == other.value); }
    public Boolean ne(Int other)          { return Boolean.valueOf(this.value != other.value); }
    public Boolean $bang$eq(Int other)    { return Boolean.valueOf(this.value != other.value); }

    public Double toFloat() { return new Double((float) this.value); }
    public Char toChar() { return new Char((char) this.value); }

    public String toStringValue() {
        return new String(java.lang.Integer.toString(this.value));
    }

    @Override
    public boolean equals(Object o) {
        return (o instanceof Int) && ((Int)o).value == this.value;
    }

    @Override
    public int hashCode() {
        return java.lang.Integer.hashCode(value);
    }

    @Override
    public java.lang.String toString() {
        return java.lang.Integer.toString(value);
    }
}
