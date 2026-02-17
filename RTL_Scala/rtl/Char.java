package rtl;

/**
 * Обёртка для символа.
 */
public final class Char extends Any {
    private final char value;

    public Char(char value) { this.value = value; }

    public char charValue() { return value; }

    public Int toInt() { return new Int((int) value); }

    public String toStringValue() {
        return new String(java.lang.Character.toString(value));
    }

    public rtl.String $plus(rtl.String other) {
        return new rtl.String(java.lang.Character.toString(this.value) + other.nativeValue());
    }

    public rtl.String concat(rtl.String other) {
        return $plus(other);
    }

    public rtl.Int $plus(rtl.Int other) {
        if (other == null) {
            throw new NullPointerException("other is null");
        }
        int result = (int) this.value + other.intValue();
        return new rtl.Int(result);
    }

    public rtl.Double $plus(rtl.Double other) {
        if (other == null) {
            throw new NullPointerException("other is null");
        }
        float result = (float) this.value + other.doubleValue();
        return new rtl.Double(result);
    }

    public rtl.Int concat(rtl.Int other) {
        return $plus(other);
    }

    public rtl.Double concat(rtl.Double other) {
        return $plus(other);
    }

    @Override
    public boolean equals(Object o) {
        return o instanceof Char && ((Char) o).value == this.value;
    }

    @Override
    public int hashCode() {
        return Character.hashCode(value);
    }

    @Override
    public java.lang.String toString() {
        return java.lang.Character.toString(value);
    }

    @Override
    public String toScalaString() {
        return new String(java.lang.Character.toString(value));
    }
}
