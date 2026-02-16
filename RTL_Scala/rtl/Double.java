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

    public Boolean eq(Double other) {
        return Boolean.valueOf(java.lang.Double.compare(this.value, other.value) == 0);
    }

    public Boolean $eq$eq(Double other) {
        return Boolean.valueOf(java.lang.Double.compare(this.value, other.value) == 0);
    }

    public Boolean ne(Double other) {
        return Boolean.valueOf(java.lang.Double.compare(this.value, other.value) != 0);
    }

    public Boolean $bang$eq(Double other) {
        return Boolean.valueOf(java.lang.Double.compare(this.value, other.value) != 0);
    }

    public Int toInt() { return new Int((int) this.value); }

    public String toStringValue() {
        return new String(java.lang.Double.toString(this.value));
    }

    @Override
    public boolean equals(Object o) {
        return (o instanceof Double)
            && java.lang.Double.compare(((Double)o).value, this.value) == 0;
    }

    @Override
    public int hashCode() {
        return java.lang.Double.hashCode(value);
    }

    @Override
    public java.lang.String toString() {
        return java.lang.Double.toString(value);
    }
}