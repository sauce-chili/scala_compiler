package rtl;

/**
 * Обёртка для вещественного типа (double).
 */
public final class Double extends Any {
    private final float value;

    public Double(float value) { this.value = value; }

    public Double(Double other) {
        this.value = other.value;
    }

    public float doubleValue() { return value; }

    public Double add(Double other) { return new Double(this.value + other.value); }
    public Double $plus(Double other) { return new Double(this.value + other.value); }
    public Double sub(Double other) { return new Double(this.value - other.value); }
    public Double $minus(Double other) { return new Double(this.value - other.value); }
    public Double mul(Double other) { return new Double(this.value * other.value); }
    public Double $times(Double other) { return new Double(this.value * other.value); }
    public Double div(Double other) { return new Double(this.value / other.value); }
    public Double $div(Double other) { return new Double(this.value / other.value); }

    public Double unary_$plus()  { return this; }
    public Double unary_$minus() { return new Double(-this.value); }

    public Boolean lt(Double other) { return Boolean.valueOf(this.value < other.value); }
    public Boolean $less(Double other) { return Boolean.valueOf(this.value < other.value); }
    public Boolean le(Double other) { return Boolean.valueOf(this.value <= other.value); }
    public Boolean $less$eq(Double other) { return Boolean.valueOf(this.value <= other.value); }
    public Boolean gt(Double other) { return Boolean.valueOf(this.value > other.value); }
    public Boolean $greater(Double other) { return Boolean.valueOf(this.value > other.value); }
    public Boolean ge(Double other) { return Boolean.valueOf(this.value >= other.value); }
    public Boolean $greater$eq(Double other) { return Boolean.valueOf(this.value >= other.value); }


    // Int section -----------------------------------------------------------------------------------------------------

    public Double add(Int other)    { return new Double(this.value + other.intValue()); }
    public Double $plus(Int other)  { return new Double(this.value + other.intValue()); }
    public Double sub(Int other)    { return new Double(this.value - other.intValue()); }
    public Double $minus(Int other) { return new Double(this.value - other.intValue()); }
    public Double mul(Int other)    { return new Double(this.value * other.intValue()); }
    public Double $times(Int other) { return new Double(this.value * other.intValue()); }
    public Double div(Int other) {
        if (other.intValue() == 0.0) throw new ArithmeticException("Division by zero");
        return new Double(this.value / other.intValue());
    }
    public Double $div(Int other) {
        if (other.intValue() == 0.0) throw new ArithmeticException("Division by zero");
        return new Double(this.value / other.intValue());
    }

    public Boolean lt(Int other)          { return Boolean.valueOf(this.value < other.intValue()); }
    public Boolean $less(Int other)       { return Boolean.valueOf(this.value < other.intValue()); }
    public Boolean le(Int other)          { return Boolean.valueOf(this.value <= other.intValue()); }
    public Boolean $less$eq(Int other)    { return Boolean.valueOf(this.value <= other.intValue()); }
    public Boolean gt(Int other)          { return Boolean.valueOf(this.value > other.intValue()); }
    public Boolean $greater(Int other)    { return Boolean.valueOf(this.value > other.intValue()); }
    public Boolean ge(Int other)          { return Boolean.valueOf(this.value >= other.intValue()); }
    public Boolean $greater$eq(Int other) { return Boolean.valueOf(this.value >= other.intValue()); }
    public Boolean eq(Int other)          { return Boolean.valueOf(this.value == other.intValue()); }
    public Boolean $eq$eq(Int other)      { return Boolean.valueOf(this.value == other.intValue()); }
    public Boolean ne(Int other)          { return Boolean.valueOf(this.value != other.intValue()); }
    public Boolean $bang$eq(Int other)    { return Boolean.valueOf(this.value != other.intValue()); }

    // -----------------------------------------------------------------------------------------------------------------

    public rtl.String $plus(rtl.String other) {
        return new String(java.lang.Float.toString(this.value) + other.nativeValue());
    }
    public rtl.String concat(rtl.String other) {
        return $plus(other);
    }

    public rtl.Double $plus(rtl.Char other) {
        return other.concat(this);
    }
    public rtl.Double concat(rtl.Char other) {
        return $plus(other);
    }

    public Boolean eq(Double other) {
        return Boolean.valueOf(java.lang.Float.compare(this.value, other.value) == 0);
    }

    public Boolean $eq$eq(Double other) {
        return Boolean.valueOf(java.lang.Float.compare(this.value, other.value) == 0);
    }

    public Boolean ne(Double other) {
        return Boolean.valueOf(java.lang.Float.compare(this.value, other.value) != 0);
    }

    public Boolean $bang$eq(Double other) {
        return Boolean.valueOf(java.lang.Float.compare(this.value, other.value) != 0);
    }

    public Int toInt() { return new Int((int) this.value); }

    public String toStringValue() {
        return new String(java.lang.Float.toString(this.value));
    }

    @Override
    public boolean equals(Object o) {
        return (o instanceof Float)
            && java.lang.Float.compare(((Double)o).value, this.value) == 0;
    }

    @Override
    public int hashCode() {
        return java.lang.Float.hashCode(value);
    }

    @Override
    public java.lang.String toString() {
        return java.lang.Float.toString(this.value);
    }
}