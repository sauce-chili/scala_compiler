package rtl;

/**
 * Обёртка для символа.
 */
public final class Char extends Any {
    private final char value;

    public Char(char value) { this.value = value; }

    public char charValue() { return value; }

    public Int toInt() { return new Int((int) value); }
    public String toStringValue() { return new String(java.lang.Character.toString(value)); }

    @Override
    public boolean equals(Object o) {
        return (o instanceof Char) && ((Char)o).value == this.value;
    }

    @Override
    public int hashCode() { return Character.hashCode(value); }

    @Override
    public java.lang.String toString() { return java.lang.Character.toString(value); }
}
