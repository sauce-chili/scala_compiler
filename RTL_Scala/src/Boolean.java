package rtl;

/**
 * Обёртка для булевого типа.
 * Имеет статические TRUE/FALSE и операции both instance и static для удобства генерации.
 */
public final class Boolean extends Any {
    public static final Boolean TRUE  = new Boolean(true);
    public static final Boolean FALSE = new Boolean(false);

    private final boolean value;

    private Boolean(boolean v) { this.value = v; }

    public static Boolean valueOf(boolean v) {
        return v ? TRUE : FALSE;
    }

    public boolean booleanValue() { return value; }

    public Boolean and(Boolean other) { return valueOf(this.value && other.value); }
    public Boolean or(Boolean other)  { return valueOf(this.value || other.value); }
    public Boolean xor(Boolean other) { return valueOf(this.value ^ other.value); }
    public Boolean not()              { return valueOf(!this.value); }

    public static Boolean and(Boolean a, Boolean b) { return valueOf(a.value && b.value); }
    public static Boolean or(Boolean a, Boolean b)  { return valueOf(a.value || b.value); }
    public static Boolean xor(Boolean a, Boolean b) { return valueOf(a.value ^ b.value); }
    public static Boolean not(Boolean a)            { return valueOf(!a.value); }

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