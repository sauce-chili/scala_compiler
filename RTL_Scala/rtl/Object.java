package rtl;

public abstract class Object {

    public java.lang.String toJvmString() {
        return super.toString();
    }

    public String ToString() {
        return new String(toJvmString());
    }

    @Override
    public java.lang.String toString() {
        return ToString().nativeValue();
    }

    public abstract boolean equals(Object other);
    public abstract int hashCode();

    public Boolean notEquals(Object other) {
        return new Boolean(!equals(other));
    }

    public Boolean is(Object other) {
        return new Boolean(this == other);
    }

    public Boolean isNot(Object other) {
        return new Boolean(this != other);
    }
}
