package rtl;

/**
 * Обёртка для строк (в пакете rtl).
 */
public final class String extends Any {
    private final java.lang.String value;

    public String(java.lang.String value) {
        this.value = (value == null) ? "" : value;
    }

    public java.lang.String nativeValue() { return value; }

    public Int length() { return new Int(value.length()); }

    public Char charAt(Int idx) {
        int i = idx.intValue();
        if (i < 0 || i >= value.length())
            throw new IndexOutOfBoundsException("String index out of range: " + i);
        return new Char(value.charAt(i));
    }

    public String concat(String other) { return new String(this.value + other.value); }

    public String substring(Int fromInclusive, Int toExclusive) {
        int f = fromInclusive.intValue();
        int t = toExclusive.intValue();
        return new String(this.value.substring(f, t));
    }

    public Int toInt() { return new Int(java.lang.Integer.parseInt(this.value.trim())); }

    public Double toFloat() {
        return new Double(java.lang.Double.parseDouble(this.value.trim()));
    }

    public Char toChar() {
        if (this.value.length() == 0)
            throw new IllegalArgumentException("Cannot convert empty string to Char");
        return new Char(this.value.charAt(0));
    }

    public Boolean equalsString(String other) {
        return Boolean.valueOf(this.value.equals(other.value));
    }

    @Override
    public boolean equals(Object o) {
        return (o instanceof String) && ((String) o).value.equals(this.value);
    }

    @Override
    public int hashCode() { return value.hashCode(); }

    @Override
    public java.lang.String toString() {
        return value;
    }
}
