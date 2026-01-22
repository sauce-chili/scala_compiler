package rtl;

/**
 * Обёртка для вещественного типа (double).
 */
public final class Double extends Any {
    private final double value;

    public Double(double value) { this.value = value; }

    public static Double valueOf(double v) { return new Double(v); }

    public double doubleValue() { return value; }

    public Double add(Double other) { return new Double(this.value + other.value); }
    public Double sub(Double other) { return new Double(this.value - other.value); }
    public Double mul(Double other) { return new Double(this.value * other.value); }
    public Double div(Double other) { return new Double(this.value / other.value); }

    public static Double add(Double a, Double b) { return a.add(b); }
    public static Double sub(Double a, Double b) { return a.sub(b); }
    public static Double mul(Double a, Double b) { return a.mul(b); }
    public static Double div(Double a, Double b) { return a.div(b); }

    public rtl.Boolean lt(Double other) { return rtl.Boolean.valueOf(this.value < other.value); }
    public rtl.Boolean le(Double other) { return rtl.Boolean.valueOf(this.value <= other.value); }
    public rtl.Boolean gt(Double other) { return rtl.Boolean.valueOf(this.value > other.value); }
    public rtl.Boolean ge(Double other) { return rtl.Boolean.valueOf(this.value >= other.value); }
    public rtl.Boolean eq(Double other) { return rtl.Boolean.valueOf(Double.compare(this.value, other.value) == 0); }
    public rtl.Boolean ne(Double other) { return rtl.Boolean.valueOf(Double.compare(this.value, other.value) != 0); }

    public static rtl.Boolean lt(Double a, Double b) { return a.lt(b); }
    public static rtl.Boolean le(Double a, Double b) { return a.le(b); }
    public static rtl.Boolean gt(Double a, Double b) { return a.gt(b); }
    public static rtl.Boolean ge(Double a, Double b) { return a.ge(b); }
    public static rtl.Boolean eq(Double a, Double b) { return a.eq(b); }
    public static rtl.Boolean ne(Double a, Double b) { return a.ne(b); }

    public Int toInt() { return new Int((int) this.value); }
    public String toStringValue() { return new String(java.lang.Double.toString(this.value)); }

    @Override
    public boolean equals(Object o) {
        return (o instanceof Double) && Double.compare(((Double)o).value, this.value) == 0;
    }

    @Override
    public int hashCode() { return Double.hashCode(value); }

    @Override
    public java.lang.String toString() {
        return java.lang.Double.toString(value);
    }
}