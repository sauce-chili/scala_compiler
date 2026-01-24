package rtl;

/**
 * Обёртка для булевого типа.
 */
public final class Boolean extends Any {

    private final boolean value;

    private Boolean(boolean v) { this.value = v; }

    public boolean booleanValue() { return value; }

    public Boolean and(Boolean other) { return valueOf(this.value && other.value); }
    public Boolean or(Boolean other)  { return valueOf(this.value || other.value); }
    public Boolean xor(Boolean other) { return valueOf(this.value ^ other.value); }
    public Boolean not()              { return valueOf(!this.value); }

    @Override
    public boolean equals(Object o) {
        return (o instanceof Boolean) && ((Boolean)o).value == this.value;
    }

    @Override
    public int hashCode() {
        return value ? 1 : 0;
    }

    @Override
    public String toString() {
        return value ? "true" : "false";
    }
}