package rtl;

/**
 * Обёртка для булевого типа.
 */
public final class Boolean extends Any {

    private final boolean value;

    public Boolean(boolean v) {
        this.value = v;
    }

    public Boolean(Boolean other) {
        this.value = other.value;
    }

    public static Boolean valueOf(boolean v) {
        return new Boolean(v);
    }

    public boolean booleanValue() { return value; }

    public Boolean and(Boolean other)      { return valueOf(this.value && other.value); }
    public Boolean $amp(Boolean other)     { return valueOf(this.value && other.value); }
    public Boolean $amp$amp(Boolean other) { return valueOf(this.value && other.value); }
    public Boolean or(Boolean other)       { return valueOf(this.value || other.value); }
    public Boolean $bar(Boolean other)     { return valueOf(this.value || other.value); }
    public Boolean $bar$bar(Boolean other) { return valueOf(this.value || other.value); }
    public Boolean xor(Boolean other)      { return valueOf(this.value ^ other.value); }
    public Boolean $up(Boolean other)      { return valueOf(this.value ^ other.value); }
    public Boolean not()                   { return valueOf(!this.value); }
    public Boolean $bang()                 { return valueOf(!this.value); }
    public Boolean unary_$bang()           { return valueOf(!this.value); }

    public rtl.String $plus(rtl.String other) {
        return new String(java.lang.Boolean.toString(this.value) + other.nativeValue());
    }

    public rtl.String concat(rtl.String other) {
        return $plus(other);
    }

    @Override
    public boolean equals(Object o) {
        return (o instanceof Boolean) && ((Boolean)o).value == this.value;
    }

    @Override
    public int hashCode() {
        return value ? 1 : 0;
    }

    @Override
    public java.lang.String toString() {
        return value ? "true" : "false";
    }

    @Override
     public String toScalaString() {
        return new String(value ? "true" : "false");
     }
}