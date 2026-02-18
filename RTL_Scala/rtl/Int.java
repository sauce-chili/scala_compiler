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

    // Double section -------------------------------------------------------------------------------------------------

    public Double add(Double other)    { return new Double(this.value + other.doubleValue()); }
    public Double $plus(Double other)  { return new Double(this.value + other.doubleValue()); }
    public Double sub(Double other)    { return new Double(this.value - other.doubleValue()); }
    public Double $minus(Double other) { return new Double(this.value - other.doubleValue()); }
    public Double mul(Double other)    { return new Double(this.value * other.doubleValue()); }
    public Double $times(Double other) { return new Double(this.value * other.doubleValue()); }
    public Double div(Double other) {
        if (other.doubleValue() == 0.0) throw new ArithmeticException("Division by zero");
        return new Double(this.value / other.doubleValue());
    }
    public Double $div(Double other) {
        if (other.doubleValue() == 0.0) throw new ArithmeticException("Division by zero");
        return new Double(this.value / other.doubleValue());
    }

    public Boolean lt(Double other)          { return Boolean.valueOf(this.value < other.doubleValue()); }
    public Boolean $less(Double other)       { return Boolean.valueOf(this.value < other.doubleValue()); }
    public Boolean le(Double other)          { return Boolean.valueOf(this.value <= other.doubleValue()); }
    public Boolean $less$eq(Double other)    { return Boolean.valueOf(this.value <= other.doubleValue()); }
    public Boolean gt(Double other)          { return Boolean.valueOf(this.value > other.doubleValue()); }
    public Boolean $greater(Double other)    { return Boolean.valueOf(this.value > other.doubleValue()); }
    public Boolean ge(Double other)          { return Boolean.valueOf(this.value >= other.doubleValue()); }
    public Boolean $greater$eq(Double other) { return Boolean.valueOf(this.value >= other.doubleValue()); }
    public Boolean eq(Double other)          { return Boolean.valueOf(this.value == other.doubleValue()); }
    public Boolean $eq$eq(Double other)      { return Boolean.valueOf(this.value == other.doubleValue()); }
    public Boolean ne(Double other)          { return Boolean.valueOf(this.value != other.doubleValue()); }
    public Boolean $bang$eq(Double other)    { return Boolean.valueOf(this.value != other.doubleValue()); }

    // ------------------------------------------------------------------------------------------------------------

    public rtl.String $plus(rtl.String other) {
        return new String(java.lang.Integer.toString(this.value) + other.nativeValue());
    }

    public rtl.String concat(rtl.String other) {
        return $plus(other);
    }

    public rtl.Int $plus(rtl.Char other) {
        return other.concat(this);
    }
    public rtl.Int concat(rtl.Char other) {
        return $plus(other);
    }

    public Double toFloat() { return new Double((float) this.value); }
    public Char toChar() { return new Char((char) this.value); }

    /** Создаёт массив [this, this+1, ..., end] (включительно). */
    public Array to(Int end) {
        int s = this.value;
        int e = end.intValue();
        int size = Math.max(0, e - s + 1);
        Any[] elems = new Any[size];
        for (int i = 0; i < size; i++) {
            elems[i] = new Int(s + i);
        }
        return new Array(elems);
    }

    /** Создаёт массив [this, this+1, ..., end-1] (не включая end). */
    public Array until(Int end) {
        int s = this.value;
        int e = end.intValue();
        int size = Math.max(0, e - s);
        Any[] elems = new Any[size];
        for (int i = 0; i < size; i++) {
            elems[i] = new Int(s + i);
        }
        return new Array(elems);
    }

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
